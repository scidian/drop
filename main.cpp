//####################################################################################
//##                                                                                ##
//##       ,gggggggggggg,    ,ggggggggggg,     _,gggggg,_      ,ggggggggggg,        ##
//##      dP"""88""""""Y8b, dP"""88""""""Y8, ,d8P""d8P"Y8b,   dP"""88""""""Y8,      ##
//##      Yb,  88       `8b,Yb,  88      `8b,d8'   Y8   "8b,dPYb,  88      `8b      ##
//##       `"  88        `8b `"  88      ,8Pd8'    `Ybaaad88P' `"  88      ,8P      ##
//##           88         Y8     88aaaad8P" 8P       `""""Y8       88aaaad8P"       ##
//##           88         d8     88""""Yb,  8b            d8       88"""""          ##
//##           88        ,8P     88     "8b Y8,          ,8P       88               ##
//##           88       ,8P'     88      `8i`Y8,        ,8P'       88               ##
//##           88______,dP'      88       Yb,`Y8b,,__,,d8P'        88               ##
//##          888888888P"        88        Y8  `"Y8888P"'          88               ##
//##                                                                                ##
//##                                                                                ##
//##                                 Drop Creator                                   ##
//##                        A Drag and Drop Video Game Maker                        ##
//##                                                                                ##
//##                             by Scidian Software                                ##
//##                              Stephens Nunnally                                 ##
//##                                                                                ##
//##                            Started December 2018                               ##
//##                                                                                ##
//####################################################################################

#include <QApplication>

#include "form_main.h"
#include "globals.h"

int main(int argc, char *argv[])
{
    QApplication app_drop(argc, argv);              // Declare application

    Globals *my_globals = new Globals();            // Declare / Load Globals helper
    FormMain form_main(nullptr, my_globals);        // Declare / Load FormMain, pass Globals helper
    form_main.show();                               // Show FormMain

    app_drop.exec();                                // Run program

    delete my_globals;
    //ShowMessageBox("Finished running program");
    return 0;
}








