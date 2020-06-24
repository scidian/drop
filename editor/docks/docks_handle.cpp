//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QDebug>

#include "editor/docks/docks.h"


namespace Dr {

//####################################################################################
//##    Keeps docks from changing width while QMainWindow is changed around
//####################################################################################
void LockDockSize(QDockWidget *&dock) {
    dock->setFixedWidth(  dock->width() );
    ///dock->setFixedHeight( dock->height() );
}

void UnlockDockSize(QMainWindow *window, QDockWidget *&dock) {
    int pre_width =  dock->width();
    ///int pre_height = dock->height();
    dock->setMaximumSize(625, QWIDGETSIZE_MAX);

    QString dock_name = dock->objectName();
    if (dock_name == "dockInspector") {
        dock->setMinimumSize(260, 80);
    } else {
        dock->setMinimumSize(124, 80);
    }

    QApplication::processEvents();
    window->resizeDocks( { dock }, { pre_width },  Qt::Horizontal);
    ///window->resizeDocks( { dock }, { pre_height }, Qt::Vertical);
}




}   // End namespace Dr
