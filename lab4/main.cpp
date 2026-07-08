#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("VectorEditor");
    app.setOrganizationName("QtLab4");

    MainWindow window;
    window.show();

    return app.exec();
}
