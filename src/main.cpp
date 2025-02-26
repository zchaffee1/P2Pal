#include "main_window.h"
#include "chat_session.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Set up command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("P2P Chat Application");
    parser.addHelpOption();
    
    // Port option
    QCommandLineOption portOption(
        QStringList() << "p" << "port",
        QCoreApplication::translate("main", "Port number to listen on"),
        QCoreApplication::translate("main", "port"),
        "54999" // default value
    );
    parser.addOption(portOption);
    parser.process(app);

    // Get port number
    bool ok;
    quint16 port = parser.value(portOption).toUShort(&ok);
    if (!ok || port < 1024) {
        qCritical() << "Invalid port number. Please use a port number between 1024 and 65535";
        return 1;
    }

    // Initialize main window with chat session
    MainWindow mainWin(port);
    mainWin.show();

    qInfo() << "Chat application started on port" << port;
    
    return app.exec();
}
