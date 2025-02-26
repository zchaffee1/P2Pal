#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QVariantMap>
#include <QUuid>

class Message : public QObject {
  //    Q_OBJECT
  //    Q_PROPERTY(QString chatText READ chatText WRITE setChatText)
  //    Q_PROPERTY(QUuid origin READ origin)
  //    Q_PROPERTY(quint64 sequence READ sequence)

  public:
    Message();
    // Removed the ambiguous three-argument constructor

    Message(const Message& other);   
    Message& operator=(const Message& other);

    explicit Message(QObject* parent = nullptr);
    explicit Message(const QString& text, 
        const QUuid& origin, 
        quint64 seq,
        QObject* parent = nullptr);
    ~Message() = default;

    QString chatText() const;
    QUuid origin() const;
    quint64 sequence() const;

    void setChatText(const QString& text);

    QVariantMap toVariantMap() const;
    static Message fromVariantMap(const QVariantMap& map);

  private:
    QString m_chatText;
    QUuid m_origin;
    quint64 m_sequence;
};

#endif
