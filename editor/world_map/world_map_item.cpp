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
#include "engine/debug_flags.h"
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
    std::string            description = m_entity->getComponentPropertyValue(Comps::Hidden_Settings, Props::Hidden_Advisor_Description).toString();
    if (description == "") description = Dr::StringFromType(m_entity->getType());
    setData(User_Roles::Name,       QString::fromStdString(m_entity->getName()));
    setData(User_Roles::Type,       QString::fromStdString(description) );
    setData(User_Roles::Key,        QVariant::fromValue(m_entity_key));

    // ***** Turn on anti aliasing
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor))
        setTransformationMode(Qt::SmoothTransformation);

    // ***** Set Item Properties
    this->setPos(0, 0);
    this->setPixmap(QPixmap(":/assets/asset_types/blob.png"));

    // ***** Apply Initial Settings
    setAcceptHoverEvents(true);                                                 // Item tracks mouse movement
    setShapeMode(QGraphicsPixmapItem::ShapeMode::MaskShape);                    // Allows for selecting while ignoring transparent pixels
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
    enableItemChangeFlags();
}

WorldMapItem::~WorldMapItem() { }

void WorldMapItem::disableItemChangeFlags() {
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, false);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, false);
    m_item_change_flags_enabled = false;
}
void WorldMapItem::enableItemChangeFlags() {
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, true);
    m_item_change_flags_enabled = true;
}


//####################################################################################
//##    Item Property Overrides
//####################################################################################
// Outline of entire item
QRectF WorldMapItem::boundingRect() const {
    QRectF my_rect = QRectF(0, 0, 256, 256);
    return my_rect;
}

// Seems to define mouseOver events, and intersection events for Rubber Band Box
QPainterPath WorldMapItem::shape() const {
    ///QPainterPath path;
    ///path.addRect(0, 0, m_width, m_height);
    ///return path;
    return QGraphicsPixmapItem::shape();
}














