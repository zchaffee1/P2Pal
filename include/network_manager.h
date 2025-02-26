#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QMap>
#include "message.h"

class NetworkManager : public QObject {
    Q_OBJECT
public:
    explicit NetworkManager(quint16 port = 55000, QObject* parent = nullptr);
    ~NetworkManager();
    
    void sendMessage(const Message& message);
    void addPeer(const QHostAddress& address, quint16 port);
    void announcePresence();

signals:
    void newMessageReceived(const Message& message);
    void peerDiscovered(const QHostAddress& address, quint16 port);

private slots:
    void readPendingDatagrams();
    void resendUnacknowledged();

private:
    QUdpSocket udpSocket;
    QTimer resendTimer;
    quint16 listenPort;
    
    QMap<QUuid, QPair<QHostAddress, quint16>> peers;  
    QMap<quint64, QPair<Message*, QDateTime>> pendingMessages;
    
    void serializeAndSend(const Message& message, const QHostAddress& target, quint16 port);
    Message deserializeDatagram(const QByteArray& data);

    void handlePeerDiscovery(const Message& message, const QHostAddress& senderAddress, quint16 senderPort);
    QTimer announcementTimer;
    QUuid instanceId;
};

#endif // NETWORK_MANAGER_H
