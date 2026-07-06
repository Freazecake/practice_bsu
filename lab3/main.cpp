#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("ProductCardApp");
    app.setOrganizationName("QtLab3");

    MainWindow window;
    window.show();

    return app.exec();
}
