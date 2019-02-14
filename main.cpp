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
//##                            Started December 7, 18                              ##
//##                                                                                ##
//####################################################################################
//
//  FormStartup                     Pick Recent Project, News, Create New Project
//
//  FormMain                        (4) Modes: World Editor
//                                             UI Editor
//                                             World Map: World / UI Layout
//                                             Stage Map: Stage Layout
//      Collision Shape Editor      Edit Collision Shape
//      Atlas Editor                View / Edit Project Atlases
//      Font Editor                 Create / View Fonts for Use in Project
//      Image Editor                Draw shapes, objects for use in Assets, vectors, custom format, export
//      Object Editor               Edit object's ObjectMaps
//      Particle Editor             Make custom particles
//      Sound Editor
//      Animation Editor
//

#include <QApplication>
#include <QMetaType>

#include "colors.h"
#include "debug.h"
#include "form_main.h"

// To declare a custom QVariant Type:
typedef std::map<long, QString> OptionList;

Q_DECLARE_METATYPE(OptionList)

///Q_DECLARE_METATYPE(QList<int>)
///qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
///settings.setValue("foo", QVariant::fromValue(myList));
///QList<int> myList2 = settings.value("foo").value<QList<int> >();

int main(int argc, char *argv[])
{
    // ***** This registers custom typedefs, classes, structs, etc. for use with Qt::QueuedConnection signals and QVariants
    qRegisterMetaType<HeaderBodyList>("HeaderBodyList");
    qRegisterMetaType<OptionList>("OptionList");

    // ***** Initiliaze application
    QApplication app_drop(argc, argv);              // Declare application

    // Load some global data
    Dr::InitializeFlags();
    Dr::LoadPalettes();

    FormMain form_main(nullptr);                    // Declare / Load FormMain, pass Globals helper
    Dr::SetActiveRelay(&form_main);                 // Set main form to active relay
    form_main.show();                               // Show FormMain

    app_drop.exec();                                // Run program


    //Dr::ShowMessageBox("Finished running program");

    return 0;
}

















