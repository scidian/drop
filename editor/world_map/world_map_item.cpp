//
//      Created by Stephens Nunnally on 5/1/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsPixmapItem>
#include <QWidget>

#include "core/colors/colors.h"
#include "core/dr_debug.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/world_map/world_map_item.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
WorldMapItem::WorldMapItem(DrProject *project, IEditorRelay *editor_relay, long project_key) {

    // ***** Store relevant Project data for use later
    m_editor_relay  = editor_relay;
    m_project       = project;
    m_entity_key    = project_key;
    m_entity        = project->findSettingsFromKey(project_key);

    // ***** Store some initial user data
    setData(User_Roles::Name, QString::fromStdString(m_entity->getName()) );
    std::string description = m_entity->getComponentPropertyValue(Comps::Hidden_Settings, Props::Hidden_Advisor_Description).toString();
    if (description == "") description = Dr::StringFromType(m_entity->getType());
    setData(User_Roles::Type, QString::fromStdString(description) );
    setData(User_Roles::Key, QVariant::fromValue(m_entity_key));



}

WorldMapItem::~WorldMapItem() { }














