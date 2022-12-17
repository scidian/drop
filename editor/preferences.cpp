//
//      Created by Stephens Nunnally on 3/3/2019, (c) 2019 Scidian Studios, All Rights Reserved
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

#include "core/sound.h"
#include "editor/form_main/form_main.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"

// Type Definitions
typedef std::map<std::string, bool>         AssetCategoryMap;
typedef std::map<std::string, QPixmap>      CategoryIconMap;
typedef std::map<std::string, bool>         ComponentMap;
typedef std::map<Preferences, DrVariant>    PreferenceMap;

// Namespace Dr
namespace Dr {


//####################################################################################
//##    Local Static Variables
//####################################################################################
static AssetCategoryMap l_asset_categories;                     // Map holding if Asset Tree Category was last Expanded
static CategoryIconMap  l_category_icons;                       // Map holding icons for Asset Tree of external image categories
static ComponentMap     l_components;                           // Map holding if Inspector Component was last Expanded
static PreferenceMap    l_options;                              // Map holding PROGRAM WIDE options (PreferenceMap type defined in globals.h)
static FormMain        *l_active_form_main  { nullptr };        // Stores active FormMain reference
static IEditorRelay    *l_active_editor     { nullptr };        // Stores active IEditorRelay reference

static bool             l_done_loading      { false };          // True after initial startup of FormMain, ensures done loading before any calls to SetLabelText


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
    color_vector.push_back(color.rgba());

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
bool        GetAssetExpanded(std::string category_name) {
    auto it = l_asset_categories.find(category_name);
    if (it != l_asset_categories.end()) return it->second; else return true;
}
void        SetAssetExpanded(std::string category_name, bool expanded)                  { l_asset_categories[category_name] = expanded; }


bool        GetInspectorExpanded(std::string component_name_to_check) {
    auto it = l_components.find(component_name_to_check);
    if (it != l_components.end()) return it->second; else return false;
}
void        SetInspectorExpanded(std::string component_name_to_set, bool expanded)      { l_components[component_name_to_set] = expanded; }


QPixmap     GetAssetCategoryIcon(std::string category_name) {
    auto it = l_category_icons.find(category_name);
    if (it != l_category_icons.end()) return it->second; else return QPixmap();
}
void        SetAssetCategoryIcon(std::string category_name, QPixmap pixmap)             { l_category_icons[category_name] = pixmap; }


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

    // ***** General Options
    Dr::SetPreference(Preferences::Gui_Scale,                               1.0);
    Dr::SetPreference(Preferences::Color_Scheme,                            QVariant::fromValue(static_cast<int>(Color_Scheme::Dark)));
    Dr::SetPreference(Preferences::Color_Popup_Tab, 0);                 // int - 0 = Palette, 1 = History

    // ***** Editor Options
    Dr::SetPreference(Preferences::Editor_Show_Camera_Boxes,                false);
    Dr::SetPreference(Preferences::Editor_Show_Collision_Shapes,            false);

    Dr::SetPreference(Preferences::Editor_Lock_Backgrounds,                 false);     // !!!!! #TODO: THIS OPTION NOT IMPLEMENTED YET
    Dr::SetPreference(Preferences::Editor_Show_Connections,                 false);     // !!!!! #TODO: THIS OPTION NOT IMPLEMENTED YET

    Dr::SetPreference(Preferences::Editor_Snap_To_Grid,                     true);
    Dr::SetPreference(Preferences::Editor_Resize_To_Grid,                   true);
    Dr::SetPreference(Preferences::Editor_Grid_On_Top,                      false);

    Dr::SetPreference(Preferences::Editor_Snap_To_Center_Of_Selection_Box,  false);

    // ***** Mixer Options
    Dr::SetPreference(Preferences::Mixer_Master_Volume,                     100.0);
    Dr::SetPreference(Preferences::Mixer_Auto_Play_Asset_Sounds,            true);


    // ***** Stored Color History
    ///DrVariant colors = std::vector<DrVariant>({ qRgba(255, 0, 0, 255), qRgba(0, 0, 255, 255) });
    ///Dr::SetPreference(Preferences::Color_Popup_History, colors);
    Dr::AddToColorHistory( Dr::red   );
    Dr::AddToColorHistory( Dr::green );
    Dr::AddToColorHistory( Dr::blue  );


    // ********** Set program variables from loaded Preferences
    Dr::SetScale(Dr::GetPreference(Preferences::Gui_Scale).toDouble());
    Dr::SetColorScheme(static_cast<Color_Scheme>(Dr::GetPreference(Preferences::Color_Scheme).toInt()));
    Dr::GetSoLoud()->setGlobalVolume(Dr::GetPreference(Preferences::Mixer_Master_Volume).toDouble() / 100.0);


    //####################################################################################

    // ***** List of Asset Categories Being Expanded, defaults to true if not found in l_asset_categories
    /**
    Dr::SetAssetExpanded(Asset_Category::Characters,    true);
    Dr::SetAssetExpanded(Asset_Category::Objects,       true);
    Dr::SetAssetExpanded(Asset_Category::Devices,       true);
    Dr::SetAssetExpanded(Asset_Category::Effects,       true);
    Dr::SetAssetExpanded(Asset_Category::Items,         true);
    Dr::SetAssetExpanded(Asset_Category::Prefabs,       true);
    Dr::SetAssetExpanded(Asset_Category::Text,          true);
    Dr::SetAssetExpanded(Asset_Category::Images,        true);
    */

    // ***** List of Inspector Component Categories Being Expanded
    Dr::SetInspectorExpanded(Comps::Entity_Settings,            true);
    Dr::SetInspectorExpanded(Comps::Hidden_Settings,            true);
    Dr::SetInspectorExpanded(Comps::Size_Settings,              true);

    Dr::SetInspectorExpanded(Comps::Asset_Animation,            true);
    Dr::SetInspectorExpanded(Comps::Asset_Settings_Character,   true);
    Dr::SetInspectorExpanded(Comps::Asset_Settings_Font,        true);
    Dr::SetInspectorExpanded(Comps::Asset_Settings_Object,      true);

    Dr::SetInspectorExpanded(Comps::Mix_Settings,               true);
    Dr::SetInspectorExpanded(Comps::Sound_Settings,             true);
    Dr::SetInspectorExpanded(Comps::Sound_Effect_Settings,      true);
    Dr::SetInspectorExpanded(Comps::Track_Settings,             true);

    Dr::SetInspectorExpanded(Comps::World_Settings,             true);
    Dr::SetInspectorExpanded(Comps::World_Physics,              true);

    Dr::SetInspectorExpanded(Comps::Stage_Settings,             true);
    Dr::SetInspectorExpanded(Comps::Stage_Grid,                 true);

    Dr::SetInspectorExpanded(Comps::Thing_Layering,             true);
    Dr::SetInspectorExpanded(Comps::Thing_Transform,            true);
    Dr::SetInspectorExpanded(Comps::Thing_Movement,             true);

    // Pure Things
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Character,   true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Object,      true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Text,        true);

    // Device Things
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Camera,      true);

    // Effect Things
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Fire,        true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Fisheye,     true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Light,       true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Mirror,      true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Swirl,       true);
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Water,       true);

    // Item Things
    Dr::SetInspectorExpanded(Comps::Thing_Settings_Tile,        true);
}


}   // namespace Dr

















