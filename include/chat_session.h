// In chat_session.h
#ifndef CHAT_SESSION_H
#define CHAT_SESSION_H

#include <QObject>
#include <QUuid>
#include "message.h"

class ChatSession : public QObject {
    Q_OBJECT
public:
    explicit ChatSession(QObject* parent = nullptr);
    QUuid originId() const { return m_originId; }

public slots:
    void submitMessage(const QString& text);

signals:
    void messageReceived(const Message& message);

private:
    QUuid m_originId;
    quint64 m_sequenceNumber = 1;
};
#endif // CHAT_SESSION_H
