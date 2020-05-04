//
//      Created by Stephens Nunnally on 5/4/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DR_WIDGET_H
#define DR_WIDGET_H

#include "project/settings/settings.h"

// Forward Declarations
class DrAsset;
class DrProject;
class DrSettings;
class DrUI;
class DrWidget;
class EditorItem;

// Local Structs / Enums
struct Order_Info_Widget {
    long        key;
    DrWidget   *widget;
    int         sub_order;
};


//####################################################################################
//##    DrWidget
//##        Class to hold one Widget within an UI Layer
//############################
class DrWidget : public DrSettings
{
private:
    // External Borrowed Pointers
    DrUI               *m_parent_ui;                        // Holds reference to parent DrUI class

    // Local Variables
    DrWidgetType        m_widget_type;                      // Holds type of current Widget
    long                m_asset_key;                        // Holds the associated asset (Asset, Effect, etc) key, this way we know what image to grab for GraphicsView
    EditorItem         *m_item_in_scene = nullptr;          // Holds a pointer to a QGraphicsItem if this Widget is currently represented in the editor

public:
    // Constructor / Destructor
    explicit DrWidget(DrProject *parent_project, DrUI *parent_ui, long new_widget_key,
                      std::string new_widget_name, DrWidgetType new_widget_type, DrSettings *from_entity, double x, double y, double z);
    virtual ~DrWidget() override;

    // DrSettings Base Class Overrides
    virtual DrType  getType() override          { return DrType::Widget; }

    // Getters / Setters
    DrUI*           getParentWorld()            { return m_parent_ui; }

    long            getAssetKey()                       { return m_asset_key; }
    EditorItem*     getEditorItem()                     { return m_item_in_scene; }
    DrWidgetType    getWidgetType()                     { return m_widget_type; }

    void            setAssetKey(long key)               { m_asset_key = key; }
    void            setEditorItem(EditorItem *item)     { m_item_in_scene = item; }
    void            setWidgetType(DrWidgetType type)    { m_widget_type = type; }

    // Size Limits
    bool            keepSquare()        { return getComponentPropertyValue(Comps::Size_Settings, Props::Size_Keep_Square).toBool(); }
    DrPointF        maxSize()           { return getComponentPropertyValue(Comps::Size_Settings, Props::Size_Max_Size).toPointF();  }
    DrPointF        minSize()           { return getComponentPropertyValue(Comps::Size_Settings, Props::Size_Min_Size).toPointF();  }

    // Z-Ordering
    void            moveBackward();
    void            moveForward();
    double          getZOrderWithSub();
    void            setZOrderWithSub(double z_order, Z_Insert insert, int position = 1);

};

#endif // DR_WIDGET_H













