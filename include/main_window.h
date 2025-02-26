// In main_window.h
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include "chat_session.h"   

class QTextEdit;
class MessageInput;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void setupUI();
    void connectSignals();
    void displayMessage(const Message& message);

    QTextEdit* chatLog;
    MessageInput* messageInput;
    ChatSession* chatSession;
};
#endif // MAIN_WINDOW_H
