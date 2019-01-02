#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets/QApplication>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setAutoFillBackground(true);
    QPalette p = w.palette();
    p.setColor(QPalette::Window,"white");
    w.setPalette(p);

    w.show();


    return a.exec();
}
