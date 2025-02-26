#include "main_window.h"
#include "chat_session.h"
#include "message_input.h"
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent) 
    : QMainWindow(parent),
      chatLog(new QTextEdit(this)),
      messageInput(new MessageInput(this)),
      chatSession(new ChatSession(this)) {
    setupUI();
    connectSignals();
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    
    chatLog->setReadOnly(true);
    messageInput->setFocus();
    
    layout->addWidget(chatLog);
    layout->addWidget(messageInput);
    
    setCentralWidget(centralWidget);
    resize(600, 400);
}

// In main_window.cpp
void MainWindow::connectSignals() {
    // Proper connection syntax
    connect(messageInput, &MessageInput::messageSent,
            chatSession, &ChatSession::submitMessage);
    
    // Use qOverload for messageReceived signal
    connect(chatSession, qOverload<const Message&>(&ChatSession::messageReceived),
            this, &MainWindow::displayMessage);
}

void MainWindow::displayMessage(const Message& message) {
    QString formatted = QString("[%1 #%2] %3")
        .arg(message.origin().toString().left(8))  // Shorten UUID for display
        .arg(message.sequence())
        .arg(message.chatText());
    
    chatLog->append(formatted);
}
