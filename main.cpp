#include "mainwindow.h"
#include "presettings.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PreSettings s;

    return a.exec();
}
