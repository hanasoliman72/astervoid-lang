#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application info
    app.setApplicationName("AsterVoid IDE");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("AsterVoid");

    MainWindow window;
    window.show();

    return app.exec();
}
