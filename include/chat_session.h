#ifndef CHAT_SESSION_H
#define CHAT_SESSION_H

#include "network_manager.h"
#include <QObject>
#include <QUuid>
#include "message.h"

class ChatSession : public QObject {
  Q_OBJECT
  public:
    explicit ChatSession(quint16 port, QObject* parent = nullptr);
//    explicit ChatSession(QObject* parent = nullptr);
    QUuid originId() const { return m_originId; }

    void addPeer(const QHostAddress& address, quint16 port);

    public slots:
      void submitMessage(const QString& text);

signals:
    void messageReceived(const Message& message);

    private slots:
      void performAntiEntropy();
    void handleVectorClock(const Message& message);
    void handleMessageRequest(const Message& request);
    void requestMissingMessages(QUuid origin, quint64 start, quint64 end);
    void handleNetworkMessage(const Message& message);

  private:
    NetworkManager* network;
    QUuid m_originId;
    quint64 m_sequenceNumber = 1;
    QMap<QUuid, quint64> vectorClock;
    QTimer antiEntropyTimer;
    QHash<QUuid, QMap<quint64, Message*>> messageHistory;
};
#endif // CHAT_SESSION_H
