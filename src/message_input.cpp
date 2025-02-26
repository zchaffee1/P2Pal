#include "message_input.h"
#include <QKeyEvent>

MessageInput::MessageInput(QWidget* parent) : QTextEdit(parent) {}

void MessageInput::keyPressEvent(QKeyEvent* event) {
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) &&
        !(event->modifiers() & Qt::ShiftModifier)) {
        emit messageSent(toPlainText().trimmed());
        clear();
        event->accept();
    } else {
        QTextEdit::keyPressEvent(event);
    }
}
