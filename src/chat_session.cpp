#include "chat_session.h"

ChatSession::ChatSession(QObject* parent)
    : QObject(parent), m_originId(QUuid::createUuid()) {}

void ChatSession::submitMessage(const QString& text) {
    if (!text.isEmpty()) {
        Message msg(text, m_originId, m_sequenceNumber++);
        qDebug() << "Sending message:" << msg.toVariantMap();
        emit messageReceived(msg);
    }
}
