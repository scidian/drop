//
//      Created by Stephens Nunnally on 5/26/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "editor/view_mixer/mixer_item.h"
#include "editor/view_mixer/mixer_scene.h"
#include "editor/view_mixer/mixer_view.h"
#include "engine/debug_flags.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_sound.h"
#include "project/entities_sound/dr_track.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
MixerItem::MixerItem(DrProject *project, IEditorRelay *editor_relay, DrTrack *track) {
    // ***** Check for DrTrack
    if (track == nullptr) {
        Dr::PrintDebug("Error in MixerItem() constructor! Argument track is nullptr!");
        return;
    }

    // ***** Store relevant Project data for use later
    m_editor_relay  = editor_relay;
    m_project       = project;
    m_track         = track;

    // ***** Store some initial user data
    std::string            description = m_track->getComponentPropertyValue(Comps::Hidden_Settings, Props::Hidden_Advisor_Description).toString();
    if (description == "") description = Dr::StringFromType(m_track->getType());
    setData(User_Roles::Name,           QString::fromStdString(m_track->getName()));
    setData(User_Roles::Description,    QString::fromStdString(description));
    setData(User_Roles::Type,           QVariant::fromValue(static_cast<int>(m_track->getType())));
    setData(User_Roles::Key,            QVariant::fromValue(m_track->getKey()));

    // ***** Turn on anti aliasing
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor))
        setTransformationMode(Qt::SmoothTransformation);

    // ***** Apply Initial Settings
    setAcceptHoverEvents(true);                                                 // Item tracks mouse movement
    setShapeMode(QGraphicsPixmapItem::ShapeMode::MaskShape);                    // Allows for selecting while ignoring transparent pixels
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
    enableItemChangeFlags();
}

MixerItem::~MixerItem() { }

void MixerItem::disableItemChangeFlags() {
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, false);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, false);
    m_item_change_flags_enabled = false;
}
void MixerItem::enableItemChangeFlags() {
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, true);
    m_item_change_flags_enabled = true;
}


//####################################################################################
//##    Item Property Overrides
//####################################################################################
// Outline of entire item including paint buffer
QRectF MixerItem::boundingRect() const {
    QRectF my_rect = QRectF(0, 0, m_width, m_height);
    return my_rect;
}

// Defines mouseOver events, and intersection events for Rubber Band Box
QPainterPath MixerItem::shape() const {
    // Add rounded rect w/ paint buffer
    QPainterPath path;
    path.addRect(boundingRect());

    return path;
    ///return QGraphicsPixmapItem::shape();         // Returns QPixmap as path
}


//####################################################################################
//##    Event Overrides
//####################################################################################
// Force mouse press
void MixerItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
}






















