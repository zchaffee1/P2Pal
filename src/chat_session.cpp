#include <QJsonDocument>
#include <QJsonObject>
#include "chat_session.h"

ChatSession::ChatSession(QObject* parent) 
  : QObject(parent),
  m_originId(QUuid::createUuid()),
  m_sequenceNumber(1),
  network(new NetworkManager()),
  antiEntropyTimer(new QTimer(this)) {
    antiEntropyTimer.setInterval(10000);
    connect(&antiEntropyTimer, &QTimer::timeout, 
        this, &ChatSession::performAntiEntropy);
    antiEntropyTimer.start();
  }

// Periodically sends a vector clock to peers for synchronization
void ChatSession::performAntiEntropy() {
  QJsonObject clockJson;
  for (auto it = vectorClock.constBegin(); it != vectorClock.constEnd(); ++it) {
    clockJson[it.key().toString()] = static_cast<qint64>(it.value());
  }

  Message clockMsg(
      "VC:" + QString(QJsonDocument(clockJson).toJson(QJsonDocument::Compact)),
      m_originId,
      0
      );
  network->sendMessage(clockMsg);
}

void ChatSession::handleNetworkMessage(const Message& message) {
  messageHistory[message.origin()][message.sequence()] = new Message(message);
  if (message.chatText().isEmpty()) return;
  if (message.chatText().startsWith("VC:")) {
    handleVectorClock(message);
    return;
  }
  if (message.chatText().startsWith("REQ:")) {
    handleMessageRequest(message);
    return;
  }
  quint64& lastSeq = vectorClock[message.origin()];
  if (message.sequence() == lastSeq + 1) {
    emit messageReceived(message);
    lastSeq++;
  } else if (message.sequence() > lastSeq + 1) {
    qWarning() << "Missing sequence between" << lastSeq << "and" << message.sequence();
    // TODO: Request missing messages
  }
  // Else: duplicate message, ignore
}
// Handles incoming network messages, processing control, chat, and missing sequence messages
void ChatSession::handleMessageRequest(const Message& request) {
  QStringList parts = request.chatText().split(":");
  if (parts.size() != 4) return;

  QUuid origin = QUuid(parts[1]);
  bool ok;
  quint64 start = parts[2].toULongLong(&ok);
  if (!ok) return;
  quint64 end = parts[3].toULongLong(&ok);
  if (!ok) return;

  if (messageHistory.contains(origin)) {
    for (quint64 seq = start; seq <= end; seq++) {
      if (messageHistory[origin].contains(seq)) {
        network->sendMessage(*messageHistory[origin][seq]);
      }
    }
  }
}



// Handles incoming vector clock updates from peers
void ChatSession::handleVectorClock(const Message& message) {
  QJsonDocument doc = QJsonDocument::fromJson(message.chatText().mid(3).toUtf8());
  QVariantMap remoteClock = doc.object().toVariantMap();

  for (auto it = remoteClock.constBegin(); it != remoteClock.constEnd(); ++it) {
    QUuid origin = QUuid(it.key());
    quint64 remoteSeq = it.value().toULongLong();
    quint64 localSeq = vectorClock.value(origin, 0);

    if (remoteSeq > localSeq) {
      requestMissingMessages(origin, localSeq + 1, remoteSeq);
    }
  }
}

void ChatSession::requestMissingMessages(QUuid origin, quint64 start, quint64 end) {
  Message request(
      QString("REQ:%1:%2:%3").arg(origin.toString()).arg(start).arg(end),
      m_originId,
      0  // Explicitly use 0 instead of implicit conversion
      );
  network->sendMessage(request);
}
// Requests missing messages from a specific peer by sending a request message
void ChatSession::submitMessage(const QString& text) {
  if (!text.isEmpty()) {
    Message* msg = new Message(text, m_originId, m_sequenceNumber);
    messageHistory[m_originId][m_sequenceNumber] = msg;
    m_sequenceNumber++;
    network->sendMessage(*msg);  // Dereference pointer
    emit messageReceived(*msg);  // Dereference pointer
  }
}

// Adds a new peer to the network
void ChatSession::addPeer(const QHostAddress& address, quint16 port) {
  network->addPeer(address, port);
}
