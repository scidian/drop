//
//      Created by Stephens Nunnally on 3/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef GLOBALS_H
#define GLOBALS_H

#include <QCursor>

#include "core/types/dr_variant.h"
#include "editor/enums_editor.h"
#include "project/constants_comps_and_props.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class FormMain;
class IEditorRelay;


//####################################################################################
//##    Program Wide Options / Settings / Preferences)
//############################
enum class Preferences {
    Version_Major,                                  // string (should be convertible to int)
    Version_Minor,                                  // string (should be convertible to int)
    Version_Build,                                  // string (should be convertible to int)

    Limit_Frames_Rendered,                          // bool

    World_Editor_Show_Collision_Shapes,             // bool
    World_Editor_Show_Camera_Boxes,                 // bool

    // !!!!! #TODO: Not implemented
    World_Editor_Show_Connections,                  // bool
    World_Editor_Lock_Backgrounds,                  // bool
    // !!!!! #END TODO

    World_Editor_Snap_To_Grid,                      // bool
    World_Editor_Resize_To_Grid,                    // bool
    World_Editor_Grid_On_Top,                       // bool

    World_Editor_Snap_To_Center_Of_Selection_Box,   // bool

    Color_Popup_History,                            // std::vector<DrVariant> (DrVariant = qRgba = unsigned int)
    Color_Popup_Tab,                                // int - 0 = Palette, 1 = History
};


//####################################################################################
//##    Global function declarations
//############################
namespace Dr {

    // ***** Startup
    bool            CheckDoneLoading();
    void            SetDoneLoading(bool done);


    // ***** Prefereces
    void            LoadPreferences();
    DrVariant       GetPreference(Preferences preference_to_get);
    void            SetPreference(Preferences preference_to_set, DrVariant new_value);

    void            AddToColorHistory(DrColor color);


    // ***** Tree Tracking
    bool            GetAssetExpanded(std::string category_name);
    void            SetAssetExpanded(std::string category_name, bool expanded);

    bool            GetInspectorExpanded(std::string component_name_to_check);
    void            SetInspectorExpanded(std::string component_name_to_set, bool expanded);

    QPixmap         GetAssetCategoryIcon(std::string category_name);
    void            SetAssetCategoryIcon(std::string category_name, QPixmap pixmap);


    // ***** Global Forms
    IEditorRelay*   GetActiveEditorRelay();
    void            SetActiveEditorRelay(IEditorRelay *new_editor_relay);

    FormMain*       GetActiveFormMain();
    void            SetActiveFormMain(FormMain *new_form_main);

    void            SetLabelText(Label_Names label, QString text);


    // ***** Cursors
    void        ClearCursor();
    void        HideCursor();
    void        SetCursor(QCursor &cursor);



}   // namespace Dr

#endif // GLOBALS_H


















