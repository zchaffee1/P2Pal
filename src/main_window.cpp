#include "main_window.h"
#include "chat_session.h"
#include "message_input.h"
#include <QVBoxLayout>
#include <QWidget>

// Modified constructor to accept port
MainWindow::MainWindow(quint16 port, QWidget* parent)
    : QMainWindow(parent),
      chatLog(new QTextEdit(this)),
      messageInput(new MessageInput(this)),
      chatSession(new ChatSession(port, this)) {  // Pass port to ChatSession
    setupUI();
    connectSignals();
    chatSession->addPeer(QHostAddress::LocalHost, 55000);
}

// Sets up the UI layout with the chat log and message input
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

// Connects signals to the respective slots for sending/receiving messages
void MainWindow::connectSignals() {
    // Proper connection syntax
    connect(messageInput, &MessageInput::messageSent,
            chatSession, &ChatSession::submitMessage);
    
    // Use qOverload for messageReceived signal
    connect(chatSession, qOverload<const Message&>(&ChatSession::messageReceived),
            this, &MainWindow::displayMessage);
}

// Displays a received message in the chat log, formatted with UUID and sequence
void MainWindow::displayMessage(const Message& message) {
    QString formatted = QString("[%1 #%2] %3")
        .arg(message.origin().toString().left(8))  // Shorten UUID for display
        .arg(message.sequence())
        .arg(message.chatText());
    
    chatLog->append(formatted);
}
