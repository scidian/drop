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
    long                m_entity_key    { c_no_key };                   // Project Key to the World / UI that this item (node) represents in DrProject

    // Local Variables
    int                 m_width         { 256 };                        // Width  of Item
    int                 m_height        { 256 };                        // Height of Item

    double              m_start_x       { 0 };                          // Stores the item position the first time it was loaded
    double              m_start_y       { 0 };                          // Stores the item position the first time it was loaded

    bool                m_item_change_flags_enabled = false;            // Lets us know if we've enabled or disabled ItemChangeFlags

public:
    // Constructor / Destructor
    WorldMapItem(DrProject *project, IEditorRelay *editor_relay, long project_key);
    virtual ~WorldMapItem() override;

    // Base Getter Overrides
    virtual QRectF          boundingRect() const override;
    virtual QPainterPath    shape() const override;

    // Event Overrides
    virtual QVariant        itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual void            paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

    // Item Change Flags
    void                    disableItemChangeFlags();
    void                    enableItemChangeFlags();
    bool                    itemChangeFlagsEnabled()        { return m_item_change_flags_enabled; }

    // Getters / Setters
    DrSettings*         getEntity()                         { return m_entity; }

    double              startX()                            { return m_start_x; }
    double              startY()                            { return m_start_y; }

};

#endif // WORLD_MAP_ITEM_H




















