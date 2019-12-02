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

typedef std::map<Asset_Category, bool>  AssetCategoryMap;
typedef std::map<Components,  bool>     ComponentMap;
typedef std::map<Preferences, QVariant> PreferenceMap;

//####################################################################################
//##    These functions can be used globally without passing around an interface pointer
//##
//##    - Just need to #include "interface_relay.h"
//##    - When program starts, FormMain is set as active 'IEditorRelay' class
//##    - Make calls like: "Dr::SetLabelText"
//##
//####################################################################################

namespace Dr {

    // Global variables
    static AssetCategoryMap asset_categories;                   // Map holding if Asset Tree Category was last Expanded
    static ComponentMap     components;                         // Map holding if Inspector Component was last Expanded
    static PreferenceMap    options;                            // Map holding PROGRAM WIDE options (PreferenceMap type defined in globals.h)
    static FormMain        *g_active_form_main;                 // Stores active FormMain reference
    static IEditorRelay    *g_active_editor;                    // Stores active IEditorRelay reference

    QVariant    GetPreference(Preferences option_to_get)   { return options[option_to_get]; }
    void        SetPreference(Preferences option_to_set, QVariant new_value) { options[option_to_set] = new_value; }

    bool        GetAssetExpanded(Asset_Category asset_type)                         { return asset_categories[asset_type]; }
    void        SetAssetExpanded(Asset_Category asset_type, bool expanded)          { asset_categories[asset_type] = expanded; }

    bool        GetInspectorExpanded(Components component_to_check)                 { return components[component_to_check]; }
    void        SetInspectorExpanded(Components component_to_set, bool expanded)    { components[component_to_set] = expanded; }

    IEditorRelay*   GetActiveEditorRelay()                              { return g_active_editor; }
    void            SetActiveEditorRelay(IEditorRelay *editor_relay)    { g_active_editor =    editor_relay; }

    FormMain*       GetActiveFormMain()                                 { return g_active_form_main; }
    void            SetActiveFormMain(FormMain *form_main)              { g_active_form_main = form_main; }

    void            SetLabelText(Label_Names label, QString text)       { if (g_active_form_main) g_active_form_main->setLabelText(label, text); }

    void    ClearCursor() { while (QApplication::overrideCursor()) qApp->restoreOverrideCursor(); }
    void    HideCursor()               { qApp->setOverrideCursor( Qt::BlankCursor ); }
    void    SetCursor(QCursor &cursor) { qApp->setOverrideCursor(cursor); }


    // ########## Load saved preferences
    void LoadPreferences() {

        // Program Color Scheme
        Dr::SetColorScheme(Color_Scheme::Dark);

        // World Editor Options
        Dr::SetPreference(Preferences::World_Editor_Lock_Backgrounds,       false);
        Dr::SetPreference(Preferences::World_Editor_Show_Collision_Shapes,  false);
        Dr::SetPreference(Preferences::World_Editor_Show_Connections,       false);
        Dr::SetPreference(Preferences::World_Editor_Show_Game_Frame,        false);

        Dr::SetPreference(Preferences::World_Editor_Snap_To_Grid,           true);
        Dr::SetPreference(Preferences::World_Editor_Resize_To_Grid,         true);
        Dr::SetPreference(Preferences::World_Editor_Grid_On_Top,            false);

        Dr::SetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box, true);


        // Stored Color History
        ///QVariant colors;
        ///colors.setValue( QList<QVariant> { qRgba(255, 0, 0, 255), qRgba(0, 0, 255, 255) } );
        ///Dr::SetPreference(Preferences::Color_Popup_History, colors);
        Dr::AddToColorHistory( Qt::red   );
        Dr::AddToColorHistory( Qt::green );
        Dr::AddToColorHistory( Qt::blue  );

        Dr::SetPreference(Preferences::Color_Popup_Tab, 0);                 // int - 0 = Palette, 1 = History


        // List of Asset Categories Being Expanded
        Dr::SetAssetExpanded(Asset_Category::Character,     true);
        Dr::SetAssetExpanded(Asset_Category::Device,        true);
        Dr::SetAssetExpanded(Asset_Category::Effect,        true);
        Dr::SetAssetExpanded(Asset_Category::Object,        true);
        Dr::SetAssetExpanded(Asset_Category::Text,          true);

        Dr::SetAssetExpanded(Asset_Category::Image,         true);
        Dr::SetAssetExpanded(Asset_Category::Basic,         true);
        Dr::SetAssetExpanded(Asset_Category::Outlines,      true);
        Dr::SetAssetExpanded(Asset_Category::Gradient,      true);
        Dr::SetAssetExpanded(Asset_Category::Decoration,    true);
        Dr::SetAssetExpanded(Asset_Category::Ground,        true);
        Dr::SetAssetExpanded(Asset_Category::Polygons,      true);
        Dr::SetAssetExpanded(Asset_Category::Shapes,        true);
        Dr::SetAssetExpanded(Asset_Category::Isometric,     true);


        // List of Inspector Component Categories Being Expanded
        QList<QVariant> expanded_list;
        for (int i = 0; i < static_cast<int>(Components::Total); ++i) {
            Dr::SetInspectorExpanded(static_cast<Components>(i),        false);
        }
        Dr::SetInspectorExpanded(Components::Entity_Settings,           true);
        Dr::SetInspectorExpanded(Components::Hidden_Settings,           true);
        Dr::SetInspectorExpanded(Components::Size_Settings,             true);

        Dr::SetInspectorExpanded(Components::Asset_Animation,           true);
        Dr::SetInspectorExpanded(Components::Asset_Settings_Character,  true);
        Dr::SetInspectorExpanded(Components::Asset_Settings_Font,       true);
        Dr::SetInspectorExpanded(Components::Asset_Settings_Object,     true);

        Dr::SetInspectorExpanded(Components::World_Settings,            true);
        Dr::SetInspectorExpanded(Components::World_Physics,             true);

        Dr::SetInspectorExpanded(Components::Stage_Settings,            true);
        Dr::SetInspectorExpanded(Components::Stage_Grid,                true);

        Dr::SetInspectorExpanded(Components::Thing_Layering,            true);
        Dr::SetInspectorExpanded(Components::Thing_Transform,           true);
        Dr::SetInspectorExpanded(Components::Thing_Movement,            true);

        Dr::SetInspectorExpanded(Components::Thing_Settings_Camera,     true);
        Dr::SetInspectorExpanded(Components::Thing_Settings_Character,  true);
        Dr::SetInspectorExpanded(Components::Thing_Settings_Object,     true);
        Dr::SetInspectorExpanded(Components::Thing_Settings_Text,       true);

        Dr::SetInspectorExpanded(Components::Thing_Settings_Fire,       true);
        Dr::SetInspectorExpanded(Components::Thing_Settings_Fisheye,    true);
        Dr::SetInspectorExpanded(Components::Thing_Settings_Light,      true);
        Dr::SetInspectorExpanded(Components::Thing_Settings_Mirror,     true);
        Dr::SetInspectorExpanded(Components::Thing_Settings_Swirl,      true);
        Dr::SetInspectorExpanded(Components::Thing_Settings_Water,      true);

    }


}   // namespace Dr















