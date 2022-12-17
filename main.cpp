//####################################################################################
//##
//##                                        #
//##                                       #│#
//##                                     *#░│▒#*
//##                                   ,*#░░│▒▒#*,
//##                                  *#░░░░│▒▒▒▒#*
//##                                ,*░░░░░░│▒▒▒▒▒▒▒*,
//##                               #░░░░░░░░│▒▒▒▒▒▒▒▒▒#
//##                              #░░░░░░░░░│▒▒▒▒▒▒▒▒▒▒#
//##                              #░░░░░░░░░│▒▒▒▒▒░▒▒▒▒#
//##                              #░░░░░░░░░│▒▒▒▒░▒▒▒▒#
//##                               *#░░░░░░░│▒▒▒▒▒▒▒#*
//##                                 '**#░░░│▒▒▒#**'
//##                                    '******'
//##
//##       ,gggggggggggg,    ,ggggggggggg,     _,gggggg,_      ,ggggggggggg,
//##      dP"""88""""""Y8b, dP"""88""""""Y8, ,d8P""d8P"Y8b,   dP"""88""""""Y8,
//##      Yb,  88       `8b,Yb,  88      `8b,d8'   Y8   "8b,dPYb,  88      `8b
//##       `"  88        `8b `"  88      ,8Pd8'    `Ybaaad88P' `"  88      ,8P
//##           88         Y8     88aaaad8P" 8P       `""""Y8       88aaaad8P"
//##           88         d8     88""""Yb,  8b            d8       88"""""
//##           88        ,8P     88     "8b Y8,          ,8P       88
//##           88       ,8P'     88      `8i`Y8,        ,8P'       88
//##           88______,dP'      88       Yb,`Y8b,,__,,d8P'        88
//##          888888888P"        88        Y8  `"Y8888P"'          88
//##
//##
//##                                 Drop Creator
//##                       A Drag and Drop Video Game Maker
//##
//##                       (C) Scidian Studios, 2018-2020
//##                              Stephens Nunnally
//##
//##                         Started 2018-11-27 11:20:40
//##
//####################################################################################
//
//  Code Flags
//      !!!!! #WARNING:             Very important reminder
//      !!!!! #NOTE:                Important reminder
//      !!!!! #KEYWORD:             Keywords to find helpful code snippets
//      !!!!! #DEBUG:               Code used for debugging only
//      !!!!! #TEMP:                Needs to be fixed at some point
//      !!!!! #TODO:                Needs to be implemented at some point
//      !!!!! #FIXME:               Needs to be fixed at some point
//
//
//
//  Large Dynamic Libraries:
//      Qt -                        LGPL    Eventually for Editor ONLY!!!!!
//
//  Medium Source Libraries
//      Boost -                     Boost   Only using boost::any for now
//      Chipmunk -                  MIT     Physics, very good. Eventually look into Bullet for 3D
//      Font Stash -                zLib    Renders fonts as triangles
//      Glm -                       MIT     Amazing vector / matrix math library
//      SoLoud -                    zLib    Super duper awesome sound library
//      Stb -                       MIT     Single file header libraries, great!
//
//  Small Source Libraries
//      Delaunator -                MIT     Delaunay Triangulation of 2D Points, creates list of triangles connecting set of points
//      Hull Finder -               WTF     Attempts to find concave hull from a collection of Points
//      Poly Partition -            MIT     Polygon Partioning and Triangulation
//      Polyline Simplification -   CC0     Implementation of the Ramer-Douglas-Peucker algorithm for smoothing lines in a set of points
//      Simplex Noise (glsl)        MIT     For Fire Shader
//
//  Assets
//      Font:   Aileron             CC0     Main Editor Font
//
//
//
//
//  Main Components
//  ---------------
//  FormMain                        Modes:  "World Graph":              Node Graph for Worlds / UI
//                                          "World Creator":            2D Physics World Editor
//                                          "UI Creator":               UI World Editor
//                                          "Sound Creator":            Sound Mixer
//
//      FormAbout                   About Box
//      FormAnimation               Animation Frame editor for Asset animations
//      FormFonts                   Create / View Fonts for Use in Project
//      FormSettings                View / edit project settings
//      FormSoundEffect             Create sound effects to add as DrSounds
//
//      FormCollision               Edit Collision Shape
//      FormImageViewer             View / Edit Project Images
//      FormObjectEditor            Edit object's ObjectMaps
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
//  Future:
//      FormNewObject               Choose from different square / circle types with various bevels, coloring, corners, etc
//      FormStartup                 Pick Recent Project, News, Create New Project / Select Template
//      FormImageCreator            Draw shapes, objects for use in Assets, vectors, custom format, export
//      FormParticleCreator         Make custom particles
//
//  Old:
//      FormPlayground              Physics Playground
//
//
#include <QApplication>
#include <QDateTime>
#include <QSurfaceFormat>

#include "core/colors/colors.h"
#include "core/dr_debug.h"
#include "core/sound.h"
#include "editor/form_main/form_main.h"
#include "editor/forms/form_expire.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "engine/debug_flags.h"


//####################################################################################
//##
//##    Main
//##
//####################################################################################
int main(int argc, char *argv[]) {

    // ***** Initiliaze application
    QApplication drop(argc, argv);                                      // Declare application

    // ***** Initializer Soloud singleton
    Dr::InitializeSound();

    // Check if we need to open a file (NOT FOR MACOS)
    QString open_file ="";
    if (drop.arguments().size() > 0) open_file = drop.arguments().at(0);


    // ***** Load Global Data
    Dr::LoadDebugFlags();                                               // Sets debug flags
    Dr::LoadCustomFonts();                                              // Loads font from resource file
    Dr::LoadPalettes();                                                 // Loads color data into global vector
    Dr::LoadPreferences();                                              // Loads user preferences

    // ***** Version
    Dr::SetPreference(Preferences::Version_Major, "1");
    Dr::SetPreference(Preferences::Version_Minor, "0");
    Dr::SetPreference(Preferences::Version_Build, "0005");

    // ***** Temporary-ish options
    Dr::SetPreference(Preferences::Limit_Frames_Rendered, false);
    Dr::SetDebugFlag( Debug_Flags::Show_Secret_Menu );
    ///Dr::SetDebugFlag(Debug_Flags::Render_Soft_Body_Shapes)  ;        // Turns on rendering of Soft Body physics children objects
    ///Dr::SetDebugFlag(Debug_Flags::Load_Test_Project);                // Turns on Test Project loading into editor upon startup


    // ***** Set OpenGL surface format of QOpenGLWidgets
    QSurfaceFormat format;
    format.setDepthBufferSize(24);                                      // Enable Depth Buffer
    format.setSwapInterval(1);                                          // Enable V-Sync (set to 1 to enable, doesn't seem to work on macOS?)
    ///format.setSwapInterval(0);                                       // Disable V-Sync (set to 0 to disable, doesn't seem to work on iOS?)
    ///format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);            // Use Off Screen Double Buffer
    ///format.setSamples(4);                                            // Multi-sampling, not needed if rendering to offscreen fbo with its own multisampling
    ///format.setStencilBufferSize(8);
    ///format.setVersion(3,3);
    ///format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(2,0);
    QSurfaceFormat::setDefaultFormat(format);


    // ***** Declare / Load QMainWindows
    FormMain    *form_main      { nullptr };                            // FormMain, main editor Form
    FormExpire  *form_expire    { nullptr };                            // FormExpire used for demo versions that are expired

    // ***** Check date for expired versions
    QDate now =         QDate::currentDate();
    QDate expire =      QDate(2021, 10, 02);
    long diff_days =    0; //expire.daysTo(now);
    if (diff_days > 0) {
        form_expire = new FormExpire();
        form_expire->show();

    // ***** Create main form
    } else {
        ///form_main = new FormMain(nullptr, open_file);                // FormMain, main editor Form, trys to open command line argument as a file
        form_main = new FormMain();                                     // Create FormMain
        Dr::SetActiveFormMain(form_main);                               // Set main form to active FormMain
        Dr::SetActiveEditorRelay(form_main);                            // Set main form to active EditorRelay
        qApp->installEventFilter(form_main);                            // Installs application wide event filter attached to FormMain (acts as key grabber)
        form_main->show();                                              // Show FormMain

        // ***** Process events and mark as loaded
        qApp->processEvents();                                          // Ensure FormMain finishes showing
        Dr::SetDoneLoading(true);                                       // Marks FormMain as finished loading
    }

    // ***** Run Program
    drop.exec();

    // ***** Clean up singletons
    if (Dr::IsSoundInitialized()) Dr::CleanUpSound();

    ///Dr::ShowMessageBox("Finished running program");
    return 0;
}



//####################################################################################
//##
//##    Version History
//##
//####################################################################################
//##
//##
//##    v 1.0.0005 - Migrating away from Qt for engine and model, save files broken
//##    v 1.0.0004 - Adding built in Images
//##    v 1.0.0003 - Post Space Shooter Droplet
//##    v 1.0.0002 - Version for Space Shooter Droplet
//##    v 1.0.0001 - Initial release with Platformer Droplet
//##
//##










