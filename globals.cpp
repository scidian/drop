//
//      Created by Stephens Nunnally on 3/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Globals
//
//
#include <QApplication>

#include "forms/form_main.h"
#include "globals.h"
#include "interface_editor_relay.h"

typedef std::map<Preferences, QVariant> PreferenceMap;


//####################################################################################
//##        These functions can be used globally without passing around an interface pointer
//##
//##        - Just need to #include "interface_relay.h"
//##        - When program starts, FormMain is set as active 'IEditorRelay' class
//##        - Make calls like: "Dr::SetLabelText"
//##
//####################################################################################

namespace Dr {

    // Global variables
    static PreferenceMap options;                                       // Map holding PROGRAM WIDE options (PreferenceMap type defined in globals.h)
    static FormMain     *g_active_form_main;                            // Stores active FormMain reference
    static IEditorRelay *g_active_editor;                               // Stores active IEditorRelay reference


    QVariant    GetPreference(Preferences option_to_get)   { return options[option_to_get]; }
    void        SetPreference(Preferences option_to_set, QVariant new_value) { options[option_to_set] = new_value; }

    void        SetActiveEditorRelay(IEditorRelay *editor_relay)   { g_active_editor =    editor_relay; }
    void        SetActiveFormMain(FormMain *form_main)             { g_active_form_main = form_main; }

    void        SetLabelText(Label_Names label, QString text)      { if (g_active_form_main) g_active_form_main->setLabelText(label, text); }

    void        ClearCursor() { while (QApplication::overrideCursor()) qApp->restoreOverrideCursor(); }
    void        HideCursor()               { qApp->setOverrideCursor( Qt::BlankCursor ); }
    void        SetCursor(QCursor &cursor) { qApp->setOverrideCursor(cursor); }


    // ########## Load saved preferences
    void LoadPreferences() {

        // Program Color Scheme
        Dr::SetColorScheme(Color_Scheme::Dark);

        // World Editor Options
        Dr::SetPreference(Preferences::World_Editor_Lock_Backgrounds,       false);
        Dr::SetPreference(Preferences::World_Editor_Show_Collision_Shapes,  false);
        Dr::SetPreference(Preferences::World_Editor_Show_Connections,       false);
        Dr::SetPreference(Preferences::World_Editor_Show_Game_Frame,        false);

        Dr::SetPreference(Preferences::World_Editor_Snap_To_Grid,   true);
        Dr::SetPreference(Preferences::World_Editor_Resize_To_Grid, true);
        Dr::SetPreference(Preferences::World_Editor_Grid_On_Top,    false);

        Dr::SetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box, true);


        // Stored Color History
        ///QVariant colors;
        ///colors.setValue( QList<QVariant> { qRgba(255, 0, 0, 255), qRgba(0, 0, 255, 255) } );
        ///Dr::SetPreference(Preferences::Color_Popup_History, colors);
        Dr::AddToColorHistory( Qt::red   );
        Dr::AddToColorHistory( Qt::green );
        Dr::AddToColorHistory( Qt::blue  );

        Dr::SetPreference(Preferences::Color_Popup_Tab, 0);                 // int - 0 = Palette, 1 = History

    }


}   // namespace Dr

















