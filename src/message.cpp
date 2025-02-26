#include "message.h"

Message::Message(QObject* parent) 
    : QObject(parent), m_origin(QUuid()), m_sequence(0) {}

Message::Message(const QString& text, const QUuid& origin, quint64 seq, QObject* parent)
    : QObject(parent), m_chatText(text), m_origin(origin), m_sequence(seq) {}

QString Message::chatText() const { return m_chatText; }
QUuid Message::origin() const { return m_origin; }
quint64 Message::sequence() const { return m_sequence; }

void Message::setChatText(const QString& text) { m_chatText = text; }

QVariantMap Message::toVariantMap() const {
    return {
        {"chatText", m_chatText},
        {"origin", m_origin.toString()},
        {"sequence", m_sequence}
    };
}

Message Message::fromVariantMap(const QVariantMap& map) {
    return Message(
        map["chatText"].toString(),
        QUuid(map["origin"].toString()),
        map["sequence"].toULongLong()
    );
}
