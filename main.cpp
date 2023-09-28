#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    printf("test\n");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
