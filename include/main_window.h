// In main_window.h
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include "chat_session.h"  // Add this include

class QTextEdit;
class MessageInput;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void setupUI();
    void connectSignals();
    void displayMessage(const Message& message);  // Add this declaration

    QTextEdit* chatLog;
    MessageInput* messageInput;
    ChatSession* chatSession;  // Now properly declared
};
#endif // MAIN_WINDOW_H
