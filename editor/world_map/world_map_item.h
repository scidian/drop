//
//      Created by Stephens Nunnally on 5/1/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WORLD_MAP_ITEM_H
#define WORLD_MAP_ITEM_H

#include <QGraphicsItem>
#include <QGraphicsView>

#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class IEditorRelay;

// Local Constants
const   int     c_default_width =       192;                // Starting width of GraphicsItem
const   int     c_row_height =          32;                 // Height of each row (title, input / output)
const   int     c_slot_size =           10;                 // Size of little circle attached to each slot

const   int     c_corner_radius =       12;                 // Absolute size of round corners of Item
const   int     c_node_buffer =         16;                 // Buffer to add around QGraphicsItem to expand painting area


//####################################################################################
//##    WorldMapItem
//##        A QGraphicsItem for a node (DrProject::DrNode) for use with our custom QGraphicsScene (WorldMapScene)
//############################
class WorldMapItem : public QGraphicsPixmapItem
{

private:
    // External Borrowed Pointers
    DrProject          *m_project;                                      // Pointer to the parent project
    IEditorRelay       *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form

    DrSettings         *m_entity;                                       // Pointer to the base Entity (World, UI) in DrProject for this item (node)
    long                m_entity_key        { c_no_key };               // Project Key to the World / UI that this item (node) represents in DrProject

    // Local Variables
    int                 m_width             { 256 };                    // Width  of Item
    int                 m_height            { 256 };                    // Height of Item
    QPointF             m_start_position    { 0, 0 };                   // Desired starting position of Node

    bool                m_item_change_flags_enabled     { false };      // Lets us know if we've enabled or disabled ItemChangeFlags



public:
    // Constructor / Destructor
    WorldMapItem(DrProject *project, IEditorRelay *editor_relay, long project_key);
    virtual ~WorldMapItem() override;

    // Base Getter Overrides
    virtual QRectF          boundingRect() const override;
    virtual QPainterPath    shape() const override;

    // Event Overrides
    virtual QVariant        itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual void            mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void            paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

    // Item Change Flags
    void                    disableItemChangeFlags();
    void                    enableItemChangeFlags();
    bool                    itemChangeFlagsEnabled()        { return m_item_change_flags_enabled; }

    // Slot Functions
    QRectF                  slotRect(DrSlotType slot_type, int slot_number);

    // Getters / Setters
    DrSettings*         getEntity()                         { return m_entity; }
    QPointF             startPosition()                     { return m_start_position; }
    void                setStartPosition(QPointF position)  { m_start_position = position; }

};

#endif // WORLD_MAP_ITEM_H




















