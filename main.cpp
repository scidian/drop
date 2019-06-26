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
//##                         (C) Scidian Software, 2018-19                          ##
//##                              Stephens Nunnally                                 ##
//##                                                                                ##
//##                           Started December 7, 2018                             ##
//##                                                                                ##
//####################################################################################
//
//  Code Flags
//      !!!!! #NOTE:                Important reminder
//      !!!!! #DEBUG:               Code used for debugging only
//      !!!!! #TEMP:                Needs to be fixed at some point
//      !!!!! #TODO:                Needs to be implemented at some point
//
//
//  Main Components
//  ---------------
//  FormMain                        Modes:  World Map: World / UI Layout
//                                          Physics World Editor
//                                          UI Editor
//                                          Stage Map: Stage Layout??
//                                        FUTURE: App Editor
//                                        FUTURE: Puzzle World Editor
//
//      FormCollision               Edit Collision Shape
//      FormFonts                   Create / View Fonts for Use in Project
//      FormImageViewer             View / Edit Project Images
//      FormObjectEditor            Edit object's ObjectMaps
//      FormPlayground              Physics Playground
//      FormSettings                View / edit project settings
//
//  FormEngine                      Runs the game engine
//
//
//  Helper Components
//  -----------------
//      FormColorMagnifier          Custom color picking maginfier
//      FormPopup                   Can handle misc popup windows / alerts
//      FormPopupColors             Custom color popup, palette tab and color history
//
//
//
//
//
//  Future:
//      FormStartup                 Pick Recent Project, News, Create New Project / Select Template
//      FormImageEditor             Draw shapes, objects for use in Assets, vectors, custom format, export
//      FormParticleEditor          Make custom particles
//      FormSoundEditor
//      FormAnimationEditor
//
//
#include <QApplication>
#include <QMetaType>
#include <QSurfaceFormat>

#include "colors/colors.h"
#include "debug.h"
#include "forms/form_main.h"
#include "globals.h"
#include "helper.h"

int main(int argc, char *argv[]) {

    // ***** Initiliaze application
    QApplication drop(argc, argv);                              // Declare application

    // ***** Set OpenGL surface format of QOpenGLWidgets
    QSurfaceFormat format;
    format.setDepthBufferSize(24);                              // Enable Depth Buffer
#if defined (Q_OS_MACOS)
    format.setSwapInterval(0);                                  // Disable V-Sync (set to 0 to disable, doesn't seem to work on iOS)
#else
    format.setSwapInterval(1);                                  // Enable V-Sync (set to 1 to enable, doesn't seem to work on macOS)
#endif
    ///format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);    // Use Off Screen Double Buffer
    ///format.setSamples(4);                                    // Multi-sampling, not needed if rendering to offscreen fbo with its own multisampling
    ///format.setStencilBufferSize(8);
    ///format.setVersion(3,3);
    ///format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(2,0);
    QSurfaceFormat::setDefaultFormat(format);

    // ***** Load some global data
    Dr::InitializeFlags();                                      // Sets debug flags
    Dr::LoadCustomFonts();                                      // Loads font from resource file
    Dr::LoadPalettes();                                         // Loads color data into global vector
    Dr::LoadPreferences();                                      // Loads user preferences

    // ***** Create main form
    FormMain form_main;                                         // Declare / Load FormMain, pass Globals helper

    Dr::SetActiveFormMain(&form_main);                          // Set main form to active FormMain
    Dr::SetActiveEditorRelay(&form_main);                       // Set main form to active EditorRelay
    qApp->installEventFilter(&form_main);                       // Installs an application wide event filter attached to FormMain (acts as key grabber)
    form_main.show();                                           // Show FormMain

    // ***** Process events and mark as loaded
    qApp->processEvents();                                      // Ensure FormMain finishes showing
    Dr::SetDoneLoading(true);                                   // Marks FormMain as finished loading

    // ***** Run Program
    drop.exec();

    ///Dr::ShowMessageBox("Finished running program");
    return 0;
}















