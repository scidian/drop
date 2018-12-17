//
//
//      Drop - Drag and Drop Game Maker, by Scidian Software
//                                          Stephens Nunnally, Started December 2018
//
//

#include <QApplication>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(nullptr);
    w.show();

    return a.exec();
}
