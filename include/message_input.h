#include <QTextEdit>

class MessageInput : public QTextEdit {
    Q_OBJECT
public:
    explicit MessageInput(QWidget* parent = nullptr);

signals:
    void messageSent(const QString& message);

protected:
    void keyPressEvent(QKeyEvent* event) override;
};
