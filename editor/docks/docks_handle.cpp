//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>

#include "editor/docks/docks.h"


namespace Dr {

//####################################################################################
//##    Keeps docks from changing width while QMainWindow is changed around
//####################################################################################
void LockDockWidth(QDockWidget *&dock, int width) {
    dock->setFixedWidth( width );
}

void UnlockDockWidth(QMainWindow *window, QDockWidget *&dock) {
    int pre_width = dock->width();
    dock->setMaximumSize(622, QWIDGETSIZE_MAX);

    QString dock_name = dock->objectName();
    if (dock_name == "dockInspector")
        dock->setMinimumSize(260, 80);
    else
        dock->setMinimumSize(124, 80);

    QApplication::processEvents();
    window->resizeDocks( { dock }, { pre_width }, Qt::Horizontal);
}




}   // end namespace Dr
