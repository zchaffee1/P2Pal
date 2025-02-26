#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include "message.h"

class MessageInput;
class ChatSession;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    // Modified constructor to accept port
    explicit MainWindow(quint16 port, QWidget* parent = nullptr);
    
private slots:
    void displayMessage(const Message& message);

private:
    void setupUI();
    void connectSignals();

    QTextEdit* chatLog;
    MessageInput* messageInput;
    ChatSession* chatSession;
};

#endif
