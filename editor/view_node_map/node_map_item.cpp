//
//      Created by Stephens Nunnally on 5/1/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QWidget>

#include "core/colors/colors.h"
#include "core/dr_debug.h"
#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/view_node_map/node_map_item.h"
#include "editor/view_node_map/node_map_scene.h"
#include "editor/view_node_map/node_map_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
NodeMapItem::NodeMapItem(DrProject *project, IEditorRelay *editor_relay, DrComponent *component) {
    // ***** Check for DrComponent
    if (component == nullptr) {
        Dr::PrintDebug("Error in NodeMapItem() contructor! Argument component is nullptr!");
        return;
    }

    // ***** Store relevant Project data for use later
    m_editor_relay  = editor_relay;
    m_project       = project;
    m_entity        = component->getParentSettings();
    m_entity_key    = m_entity->getKey();
    m_component     = component;
    m_component_key = component->getComponentKey();

    // ***** Store some initial user data
    std::string            description = m_entity->getComponentPropertyValue(Comps::Hidden_Settings, Props::Hidden_Advisor_Description).toString();
    if (description == "") description = Dr::StringFromType(m_entity->getType());
    setData(User_Roles::Name,       QString::fromStdString(m_entity->getName()));
    setData(User_Roles::Type,       QString::fromStdString(description));
    setData(User_Roles::Key,        QVariant::fromValue(m_entity_key));

    // ***** Turn on anti aliasing
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor))
        setTransformationMode(Qt::SmoothTransformation);

    // ***** Adjust item to proper transform
    QPointF center = boundingRect().center();
    double transform_scale_x = 1.0;
    double transform_scale_y = 1.0;
    QTransform t = QTransform()
            .translate(center.x(), center.y())
            .rotate(0)
            .scale(transform_scale_x, transform_scale_y)
            .translate(-center.x(), -center.y());
    setTransform(t);


    // ***** Calculate Size
    m_component->updateNodeSize();
    this->m_width =  m_component->getNodeSize().x;
    this->m_height = m_component->getNodeSize().y;


    // ***** Store Slot Circle Rects
    updateSlotRects();


    // ***** Starting position
    this->setStartPosition( Dr::ToQPointF(m_component->getNodePosition()) );


    // ***** Apply Drop Shadow
    QGraphicsDropShadowEffect *drop_shadow = new QGraphicsDropShadowEffect();
    drop_shadow->setColor( Qt::black );
    drop_shadow->setBlurRadius( c_corner_radius );
    drop_shadow->setOffset( -3, 3 );
    this->setGraphicsEffect(drop_shadow);

    // ***** Apply Initial Settings
    setAcceptHoverEvents(true);                                                 // Item tracks mouse movement
    setShapeMode(QGraphicsPixmapItem::ShapeMode::MaskShape);                    // Allows for selecting while ignoring transparent pixels
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
    enableItemChangeFlags();
}

NodeMapItem::~NodeMapItem() { }

void NodeMapItem::disableItemChangeFlags() {
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, false);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, false);
    m_item_change_flags_enabled = false;
}
void NodeMapItem::enableItemChangeFlags() {
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, true);
    m_item_change_flags_enabled = true;
}

void NodeMapItem::updateSlotRects() {
    int signal_count = m_component->getSignalMap().size();
    int output_count = m_component->getOutputMap().size();
    m_signal_rects.clear();
    m_output_rects.clear();
    for (int slot = 0; slot < signal_count; ++slot) { m_signal_rects.push_back(this->slotRect(DrSlotType::Signal, slot)); }
    for (int slot = 0; slot < output_count; ++slot) { m_output_rects.push_back(this->slotRect(DrSlotType::Output, slot)); }
}


//####################################################################################
//##    Item Property Overrides
//####################################################################################
// Outline of entire item including paint buffer
QRectF NodeMapItem::boundingRect() const {
    QRectF my_rect = QRectF(0, 0, m_width + c_node_buffer*2, m_height + c_node_buffer*2);
    return my_rect;
}

// Defines mouseOver events, and intersection events for Rubber Band Box
QPainterPath NodeMapItem::shape() const {
    // Add rounded rect w/ paint buffer
    QPainterPath path;
    path.addRoundedRect(boundingRect().adjusted(c_node_buffer, c_node_buffer, -c_node_buffer, -c_node_buffer), c_corner_radius, c_corner_radius);

    // Go through Node and add Slot Circles
    for (auto &signal_rect : m_signal_rects)  {
        QPainterPath slot_circle;
                     slot_circle.addEllipse( signal_rect );
        path = path.united(slot_circle);
    }
    for (auto &output_rect : m_output_rects) {
        QPainterPath slot_circle;
                     slot_circle.addEllipse( output_rect );
        path = path.united(slot_circle);
    }
    return path;
    ///return QGraphicsPixmapItem::shape();         // Returns QPixmap as path
}



//####################################################################################
//##    Event Overrides
//####################################################################################
// Force mouse press
void NodeMapItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
}












