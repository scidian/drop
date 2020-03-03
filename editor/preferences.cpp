//
//      Created by Stephens Nunnally on 3/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Globals
//
//####################################################################################
//##    These functions can be used globally without passing around an interface pointer
//##
//##    - Just need to #include "interface_relay.h"
//##    - When program starts, FormMain is set as active 'IEditorRelay' class
//##    - Make calls like: "Dr::SetLabelText"
//##
//####################################################################################
#include <QApplication>

#include "editor/form_main/form_main.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"

typedef std::map<Asset_Category, bool>      AssetCategoryMap;
typedef std::map<std::string, bool>         ComponentMap;
typedef std::map<Preferences, DrVariant>    PreferenceMap;

namespace Dr {


//####################################################################################
//##    Local Static Variables
//####################################################################################
static AssetCategoryMap l_asset_categories;                 // Map holding if Asset Tree Category was last Expanded
static ComponentMap     l_components;                       // Map holding if Inspector Component was last Expanded
static PreferenceMap    l_options;                          // Map holding PROGRAM WIDE options (PreferenceMap type defined in globals.h)
static FormMain        *l_active_form_main;                 // Stores active FormMain reference
static IEditorRelay    *l_active_editor;                    // Stores active IEditorRelay reference

static bool             l_done_loading = false;             // True after initial startup of FormMain, ensures done loading before any calls to SetLabelText


//####################################################################################
//##    Functions to initialize, set and check flags
//####################################################################################
bool        CheckDoneLoading()              { return l_done_loading; }
void        SetDoneLoading(bool done)       { l_done_loading = done; }


//####################################################################################
//##    Preferences
//####################################################################################
DrVariant   GetPreference(Preferences option_to_get)                        { return l_options[option_to_get]; }
void        SetPreference(Preferences option_to_set, DrVariant new_value)   { l_options[option_to_set] = new_value; }

// Color History
void AddToColorHistory(DrColor color) {
    // Load old color history list (up to 36 colors)
    std::vector<DrVariant> variant_vector = Dr::GetPreference(Preferences::Color_Popup_History).toVector();

    // Creat new list and add color as first item
    std::vector<DrVariant> color_vector;
    color_vector.push_back( color.rgba() );

    // Add all the old colors skipping the lastest color
    for (auto item : variant_vector) {
        if (color.rgb() != item.toColor().rgb()) {
            color_vector.push_back( item.toColor().rgba() );
        }
    }

    // Make sure list is <= 36 items
    while (color_vector.size() > 36) {
        color_vector.pop_back();
    }

    // Store new color list in preferences
    Dr::SetPreference(Preferences::Color_Popup_History, color_vector);
}


//####################################################################################
//##    Tree Tracking
//####################################################################################
bool        GetAssetExpanded(Asset_Category asset_type)                         { return l_asset_categories[asset_type]; }
void        SetAssetExpanded(Asset_Category asset_type, bool expanded)          { l_asset_categories[asset_type] = expanded; }

bool        GetInspectorExpanded(std::string component_key_to_check) {
                auto it = l_components.find(component_key_to_check);
                if (it != l_components.end()) return it->second; else return false; }
void        SetInspectorExpanded(std::string component_key_to_set, bool expanded) { l_components[component_key_to_set] = expanded; }


//####################################################################################
//##    Global Forms
//####################################################################################
IEditorRelay*   GetActiveEditorRelay()                              { return l_active_editor; }
void            SetActiveEditorRelay(IEditorRelay *editor_relay)    { l_active_editor =    editor_relay; }

FormMain*       GetActiveFormMain()                                 { return l_active_form_main; }
void            SetActiveFormMain(FormMain *form_main)              { l_active_form_main = form_main; }

void            SetLabelText(Label_Names label, QString text)       { if (l_active_form_main) l_active_form_main->setLabelText(label, text); }


//####################################################################################
//##    Cursors
//####################################################################################
void        ClearCursor() { while (QApplication::overrideCursor()) qApp->restoreOverrideCursor(); }
void        HideCursor()               { qApp->setOverrideCursor( Qt::BlankCursor ); }
void        SetCursor(QCursor &cursor) { qApp->setOverrideCursor(cursor); }


//####################################################################################
//##
//##    Load saved preferences
//##
//####################################################################################
void LoadPreferences() {

    // Program Color Scheme
    Dr::SetColorScheme(Color_Scheme::Dark);

    // World Editor Options
    Dr::SetPreference(Preferences::World_Editor_Show_Camera_Boxes,      false);
    Dr::SetPreference(Preferences::World_Editor_Show_Collision_Shapes,  false);

    // !!!!! NOT IMPLEMENTED YET: World Editor Options
    Dr::SetPreference(Preferences::World_Editor_Lock_Backgrounds,       false);
    Dr::SetPreference(Preferences::World_Editor_Show_Connections,       false);
    // !!!!!

    Dr::SetPreference(Preferences::World_Editor_Snap_To_Grid,           true);
    Dr::SetPreference(Preferences::World_Editor_Resize_To_Grid,         true);
    Dr::SetPreference(Preferences::World_Editor_Grid_On_Top,            false);

    Dr::SetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box, false);


    // Stored Color History
    ///DrVariant colors = std::vector<DrVariant>({ qRgba(255, 0, 0, 255), qRgba(0, 0, 255, 255) });
    ///Dr::SetPreference(Preferences::Color_Popup_History, colors);
    Dr::AddToColorHistory( Dr::red   );
    Dr::AddToColorHistory( Dr::green );
    Dr::AddToColorHistory( Dr::blue  );

    Dr::SetPreference(Preferences::Color_Popup_Tab, 0);                 // int - 0 = Palette, 1 = History


    // List of Asset Categories Being Expanded
    Dr::SetAssetExpanded(Asset_Category::Character,     true);
    Dr::SetAssetExpanded(Asset_Category::Object,        true);
    Dr::SetAssetExpanded(Asset_Category::Device,        true);
    Dr::SetAssetExpanded(Asset_Category::Effect,        true);
    Dr::SetAssetExpanded(Asset_Category::Item,          true);
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
    Dr::SetInspectorExpanded(Comps::Entity_Settings,           true);
    Dr::SetInspectorExpanded(Comps::Hidden_Settings,           true);
    Dr::SetInspectorExpanded(Comps::Size_Settings,             true);

    Dr::SetInspectorExpanded(Comps::Asset_Animation,           true);
    Dr::SetInspectorExpanded(Comps::Asset_Settings_Character,  true);
    Dr::SetInspectorExpanded(Comps::Asset_Settings_Font,       true);
    Dr::SetInspectorExpanded(Comps::Asset_Settings_Object,     true);

    Dr::SetInspectorExpanded(Comps::World_Settings,            true);
    Dr::SetInspectorExpanded(Comps::World_Physics,             true);

    Dr::SetInspectorExpanded(Comps::Stage_Settings,            true);
    Dr::SetInspectorExpanded(Comps::Stage_Grid,                true);

    Dr::SetInspectorExpanded(Comps::Thing_Layering,            true);
    Dr::SetInspectorExpanded(Comps::Thing_Transform,           true);
    Dr::SetInspectorExpanded(Comps::Thing_Movement,            true);

    Dr::SetInspectorExpanded(Comps::Thing_Settings_Camera,     true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Character,  true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Object,     true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Text,       true);

    Dr::SetInspectorExpanded(Comps::Thing_Settings_Fire,       true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Fisheye,    true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Light,      true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Mirror,     true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Swirl,      true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Water,      true);

}


}   // namespace Dr

















