//
//      Created by Stephens Nunnally on 5/4/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRUI_H
#define DRUI_H

#include "core/types/dr_pointf.h"
#include "project/settings/settings.h"

// Forward Declarations
class DrAsset;
class DrProject;
class DrWidget;

// Type Definitions
typedef std::map<long, DrWidget*> WidgetMap;


//####################################################################################
//##    DrUI
//##        Class to hold one GUI Layer
//############################
class DrUI : public DrSettings
{
private:
    // Local variables
    WidgetMap       m_widgets;                                  // Map of pointers to DrThing classes (holds the things for current stage)

    bool            m_is_start_ui;                              // True if this is the first UI in the Project

    bool            m_expanded = true;                          // Wether or not this is expanded in the Project Tree
    DrPointF        m_center_view_point;                        // Holds center point the View was on last time this UI was shown
    double          m_zoom_level = 0.5;                         // Holds zoom level   the View was on last time this UI was shown


public:
    // Constructor / Destructor
    DrUI(DrProject *parent_project, long new_ui_key, std::string new_ui_name, bool is_start_ui = false);
    virtual ~DrUI() override;

    // DrSettings Overrides
    virtual DrType  getType() override                      { return DrType::UI; }

    // Getters / Setters
    WidgetMap&      getWidgetMap()                          { return m_widgets; }
    DrWidget*       getWidget(long key)                     { return m_widgets[key]; }

    bool            isStartUI()                             { return m_is_start_ui; }
    void            setIsStartUI(bool is_start_ui)          { m_is_start_ui = is_start_ui; }

    bool            getExpanded()                           { return m_expanded; }
    DrPointF        getViewCenterPoint()                    { return m_center_view_point; }
    double          getViewZoomLevel()                      { return m_zoom_level; }
    void            setExpanded(bool expanded)              { m_expanded = expanded; }
    void            setViewCenterPoint(DrPointF new_point)  { m_center_view_point = new_point; }
    void            setViewZoomLevel(double zoom)           { m_zoom_level = zoom; }


    // Misc UI Functions
    DrWidget*       addWidget(DrWidgetType new_type, long from_asset_key, double x, double y, double z, long key = c_no_key);
    void            deleteWidget(DrWidget *&thing) ;

    std::vector<long>       widgetKeysSortedByZOrder(Sort_Order sort_order);
    std::vector<DrWidget*>  widgetsSortedByZOrder(Sort_Order sort_order, bool all_widgets = true, std::list<DrWidget*> just_these_widgets = { });
    void                    initializeUISettings(std::string new_name);
    void                    removeGraphicsItemReferences();


};

#endif // DRUI_H



















