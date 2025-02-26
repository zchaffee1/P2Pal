#include "network_manager.h"
#include <QNetworkDatagram>
#include <QDateTime>

// Comparison operator for QHostAddress
bool operator<(const QHostAddress& a, const QHostAddress& b) {
    return a.toString() < b.toString();
}

NetworkManager::NetworkManager(quint16 port, QObject* parent) 
    : QObject(parent), listenPort(port), instanceId(QUuid::createUuid()) {
    udpSocket.bind(port, QUdpSocket::ShareAddress);
    connect(&udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::readPendingDatagrams);
    
    resendTimer.setInterval(2000);
    connect(&resendTimer, &QTimer::timeout, this, &NetworkManager::resendUnacknowledged);
    resendTimer.start();

    announcementTimer.setInterval(5000);
    connect(&announcementTimer, &QTimer::timeout, 
            this, &NetworkManager::announcePresence);
    announcementTimer.start();
}

void NetworkManager::announcePresence() {
    Message announcement(
        udpSocket.localAddress().toString(),  // Store IP in chatText
        instanceId,
        listenPort  // Store port in sequence number
    );
    
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << announcement.toVariantMap();
    
    // Broadcast to all potential peers
    udpSocket.writeDatagram(datagram, QHostAddress::Broadcast, listenPort);
}

NetworkManager::~NetworkManager() {
  Message goodbye("GOODBYE", instanceId, 0);
  serializeAndSend(goodbye, QHostAddress::Broadcast, listenPort);

  // Clean up pending messages
  for (auto& pair : pendingMessages) {
    delete pair.first;
  }
  pendingMessages.clear();
}

void NetworkManager::addPeer(const QHostAddress& address, quint16 port) {
    // Convert address to string for UUID key (temporary)
    QUuid tempId = QUuid::createUuid(); // Should be replaced with actual UUID exchange
    
    if (!peers.contains(tempId)) {
        qDebug() << "Discovered new peer:" << address << ":" << port;
        peers.insert(tempId, qMakePair(address, port));
        emit peerDiscovered(address, port);
    }
}

void NetworkManager::sendMessage(const Message& message) {
    Message* msgCopy = new Message(message);
    
    // Iterate through all known peers
    for (auto peer = peers.begin(); peer != peers.end(); ++peer) {
        const QHostAddress& targetAddress = peer.value().first;
        quint16 targetPort = peer.value().second;
        serializeAndSend(*msgCopy, targetAddress, targetPort);
    }
    
    pendingMessages.insert(msgCopy->sequence(), 
        qMakePair(msgCopy, QDateTime::currentDateTime()));
}

void NetworkManager::serializeAndSend(const Message& message, 
                                    const QHostAddress& target, quint16 port) {
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << message.toVariantMap();
    udpSocket.writeDatagram(datagram, target, port);
}

void NetworkManager::readPendingDatagrams() {
    while (udpSocket.hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket.receiveDatagram();
        Message message = deserializeDatagram(datagram.data());

        if (message.chatText() == udpSocket.localAddress().toString()) {
            continue; // Ignore own messages
        }

        if (message.sequence() == listenPort) { // Presence announcement
            handlePeerDiscovery(message);
        } else {
            emit newMessageReceived(message);
        }
        
        // Add peer with actual connection info
        addPeer(datagram.senderAddress(), datagram.senderPort());
    }
}

void NetworkManager::handlePeerDiscovery(const Message& message) {
    const QUuid& peerId = message.origin();
    
    if (!peers.contains(peerId)) {
        // Extract address and port from presence message
        QHostAddress peerAddress(message.chatText());
        quint16 peerPort = static_cast<quint16>(message.sequence());
        
        peers.insert(peerId, qMakePair(peerAddress, peerPort));
        
        // Forward to other peers
        for (auto existingPeer = peers.begin(); existingPeer != peers.end(); ++existingPeer) {
            if (existingPeer.key() != peerId) {
                serializeAndSend(message, 
                               existingPeer.value().first,  // Address
                               existingPeer.value().second  // Port
                );
            }
        }
    }
}

Message NetworkManager::deserializeDatagram(const QByteArray& data) {
    QDataStream stream(data);
    QVariantMap variantMap;
    stream >> variantMap;
    return Message::fromVariantMap(variantMap);
}

void NetworkManager::resendUnacknowledged() {
    auto now = QDateTime::currentDateTime();
    for (auto it = pendingMessages.begin(); it != pendingMessages.end(); ++it) {
        if (it.value().second.secsTo(now) > 2) {
            // Resend to all peers
            for (auto peer = peers.begin(); peer != peers.end(); ++peer) {
                const auto& [address, port] = peer.value();
                serializeAndSend(*(it.value().first), address, port);
            }
            it.value().second = now;
        }
    }
}
