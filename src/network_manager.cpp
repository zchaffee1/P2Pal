#include "network_manager.h"
#include <QNetworkDatagram>
#include <QDateTime>

// Comparison operator for QHostAddress
bool operator<(const QHostAddress& a, const QHostAddress& b) {
  return a.toString() < b.toString();
}

NetworkManager::NetworkManager(quint16 port, QObject* parent) 
    : QObject(parent), listenPort(port), instanceId(QUuid::createUuid()) {
    // Remove ShareAddress flag for exclusive port binding
    if (!udpSocket.bind(port, QUdpSocket::DefaultForPlatform)) {
        qCritical() << "Failed to bind to port" << port << "Error:" << udpSocket.errorString();
        exit(1);
    }
    //udpSocket.bind(port, QUdpSocket::ShareAddress);
    connect(&udpSocket, &QUdpSocket::readyRead, this, &NetworkManager::readPendingDatagrams);

    qDebug() << "Network manager initialized on port" << port 
      << "with address" << udpSocket.localAddress().toString()
      << "Socket state:" << udpSocket.state();

    // Set up a resend timer for unacknowledged messages
    resendTimer.setInterval(2000);
    connect(&resendTimer, &QTimer::timeout, this, &NetworkManager::resendUnacknowledged);
    resendTimer.start();

    // Set up an announcement timer for broadcasting presence
    announcementTimer.setInterval(5000);
    connect(&announcementTimer, &QTimer::timeout, 
        this, &NetworkManager::announcePresence);
    announcementTimer.start();
  }

// Broadcasts a presence announcement with local address and port
void NetworkManager::announcePresence() {
    // Use loopback address for local testing
    Message announcement(
        udpSocket.localAddress().toString(),
        instanceId,
        listenPort
    );
    
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << announcement.toVariantMap();
    
    // Broadcast to local network segment
    udpSocket.writeDatagram(datagram, QHostAddress("127.255.255.255"), listenPort);
}

// Destructor sends a goodbye message and cleans up pending messages
NetworkManager::~NetworkManager() {
  Message goodbye("GOODBYE", instanceId, 0);
  serializeAndSend(goodbye, QHostAddress::Broadcast, listenPort);

  for (auto& pair : pendingMessages) {
    delete pair.first;
  }
  pendingMessages.clear();
}

// Adds a new peer to the list if not already present
void NetworkManager::addPeer(const QHostAddress& address, quint16 port) {
  // Convert address to string for UUID key 
  QUuid tempId = QUuid::createUuid(); 

  if (!peers.contains(tempId)) {
    qDebug() << "Discovered new peer:" << address << ":" << port;
    peers.insert(tempId, qMakePair(address, port));
    emit peerDiscovered(address, port);
  }
}

// Sends a message to all known peers
void NetworkManager::sendMessage(const Message& message) {
  Message* msgCopy = new Message(message);

  for (auto peer = peers.begin(); peer != peers.end(); ++peer) {
    const QHostAddress& targetAddress = peer.value().first;
    quint16 targetPort = peer.value().second;
    serializeAndSend(*msgCopy, targetAddress, targetPort);
  }

  pendingMessages.insert(msgCopy->sequence(), 
      qMakePair(msgCopy, QDateTime::currentDateTime()));
}

// Serializes and sends the message to a target address and port
void NetworkManager::serializeAndSend(const Message& message, 
    const QHostAddress& target, quint16 port) {
  QByteArray datagram;
  QDataStream stream(&datagram, QIODevice::WriteOnly);
  stream << message.toVariantMap();
  udpSocket.writeDatagram(datagram, target, port);
}

// Reads incoming datagrams and processes the message
void NetworkManager::readPendingDatagrams() {
  while (udpSocket.hasPendingDatagrams()) {
    QNetworkDatagram datagram = udpSocket.receiveDatagram();
    Message message = deserializeDatagram(datagram.data());

    if (message.origin() == instanceId) continue; // Ignore own messages

    if (message.sequence() == listenPort) { // Presence announcement
      handlePeerDiscovery(message, 
          datagram.senderAddress(), 
          datagram.senderPort());
    }
    else {
      emit newMessageReceived(message);
    }
  }
}

// Handles the discovery of a new peer by adding it to the list
void NetworkManager::handlePeerDiscovery(const Message& message,
    const QHostAddress& senderAddress,
    quint16 senderPort) {
  const QUuid& peerId = message.origin();

  if (!peers.contains(peerId)) {
    peers.insert(peerId, qMakePair(senderAddress, senderPort));
    qDebug() << "Discovered peer:" << peerId.toString() 
      << "at" << senderAddress << ":" << senderPort;

    // Iterate over all existing peer IDs (keys) in the peers map
    for (auto it = peers.begin(); it != peers.end(); ++it) {
      const QUuid& existingPeerId = it.key();
      if (existingPeerId != peerId) {
        // Send the announcement to each existing peer's address and port
        const QHostAddress& address = it.value().first;
        quint16 port = it.value().second;
        serializeAndSend(message, address, port);
      }
    }
  }
}

// Deserializes a datagram into a Message object
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
