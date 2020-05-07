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

    // Adjust item to proper transform
    QPointF center = boundingRect().center();
    double transform_scale_x = 1.0;
    double transform_scale_y = 1.0;
    QTransform t = QTransform()
            .translate(center.x(), center.y())
            .rotate(0)
            .scale(transform_scale_x, transform_scale_y)
            .translate(-center.x(), -center.y());
    setTransform(t);

    // ***** Load starting position
    DrPointF start_pos = DrPointF(0, 0) ;///m_thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Position).toPointF();
    m_start_x = start_pos.x;
    m_start_y = start_pos.y;

    // ***** Set Item Properties
    this->setPixmap(QPixmap(":/assets/asset_types/blob.png"));
    this->m_width =  pixmap().width();
    this->m_height = pixmap().height();

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
    QRectF my_rect = QRectF(0, 0, m_width, m_height);
    return my_rect;
}

// Seems to define mouseOver events, and intersection events for Rubber Band Box
QPainterPath WorldMapItem::shape() const {
    ///QPainterPath path;
    ///path.addRect(0, 0, m_width, m_height);
    ///return path;
    return QGraphicsPixmapItem::shape();
}














