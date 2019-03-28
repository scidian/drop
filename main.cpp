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
//##                          (C) Scidian Software, 2018                            ##
//##                              Stephens Nunnally                                 ##
//##                                                                                ##
//##                           Started December 7, 2018                             ##
//##                                                                                ##
//####################################################################################
//
//  FormMain                        (4) Modes: World Map: World / UI Layout
//                                             World Editor
//                                             UI Editor
//                                             Stage Map: Stage Layout??
//      Atlas Viewer                View / Edit Project Atlases
//      Font Builder                Create / View Fonts for Use in Project
//      Settings Manager            View / edit project settings
//
//      Collision Shape Editor      Edit Collision Shape
//      Object Editor               Edit object's ObjectMaps
//  FormStartup                     Pick Recent Project, News, Create New Project / Select Template
//  FormColorPicker                 Implement our own color picker, Color Grabber, Palette Tab
//
//  Future:
//      Image Editor                Draw shapes, objects for use in Assets, vectors, custom format, export
//      Particle Editor             Make custom particles
//      Sound Editor
//      Animation Editor
//
#include <QApplication>
#include <QMetaType>

#include "colors.h"
#include "debug.h"
#include "form_main.h"
#include "globals.h"

int main(int argc, char *argv[])
{
    // ***** Initiliaze application
    QApplication drop(argc, argv);                  // Declare application

    // ***** Load some global data
    Dr::InitializeFlags();                          // Sets debug flags
    Dr::LoadPalettes();                             // Loads color data into global vector
    Dr::LoadPreferences();                          // Loads user preferences

    // ***** Create main form
    FormMain form_main(nullptr);                    // Declare / Load FormMain, pass Globals helper
    Dr::SetActiveFormMain(&form_main);              // Set main form to active FormMain
    Dr::SetActiveEditorRelay(&form_main);           // Set main form to active EditorRelay
    qApp->installEventFilter(&form_main);           // Installs an application wide event filter attached to FormMain (acts as key grabber)
    form_main.show();                               // Show FormMain

    // ***** Process events and mark as loaded
    QApplication::processEvents();                  // Ensure FormMain finishes showing
    Dr::SetDoneLoading(true);                       // Marks FormMain as finished loading

    // ***** Run Program
    drop.exec();

    //Dr::ShowMessageBox("Finished running program");

    return 0;
}








