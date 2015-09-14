#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("uqmu");
    QCoreApplication::setApplicationName("uqmu");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
