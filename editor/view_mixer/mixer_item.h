//
//      Created by Stephens Nunnally on 5/26/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef MIXERITEM_H
#define MIXERITEM_H

#include <QGraphicsItem>
#include <QGraphicsView>

#include "project/enums_entity_types.h"

// Forward Declarations
class DrTrack;


//####################################################################################
//##    MixerItem
//##        A QGraphicsItem for a track (DrTrack) for use with our custom QGraphicsScene (MixerScene)
//############################
class MixerItem : public QGraphicsPixmapItem
{

private:
    // External Borrowed Pointers
    DrProject          *m_project;                                      // Pointer to the parent project
    IEditorRelay       *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form

    DrTrack            *m_track;                                        // Pointer to the base DrTrack in DrMix for this item
    long                m_track_key         { c_no_key };               // Project Key to the Track that this item represents in DrMix

    // Size Info
    int                 m_width             { 256 };                    // Width  of Item
    int                 m_height            { 256 };                    // Height of Item
    QPointF             m_start_position    { 0, 0 };                   // Desired starting position of Node

    // Flags
    bool                m_item_change_flags_enabled     { false };      // Lets us know if we've enabled or disabled ItemChangeFlags


public:
    // Constructor / Destructor
    MixerItem(DrProject *project, IEditorRelay *editor_relay, DrTrack *track);
    virtual ~MixerItem() override;

    // Base Getter Overrides
    virtual QRectF          boundingRect() const override;
    virtual QPainterPath    shape() const override;

    // Event Overrides
    virtual void            mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void            paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

    // Item Change Flags
    void                    disableItemChangeFlags();
    void                    enableItemChangeFlags();
    bool                    itemChangeFlagsEnabled()        { return m_item_change_flags_enabled; }

    // Getters / Setters
    IEditorRelay*           getEditorRelay()                    { return m_editor_relay; }
    DrTrack*                getTrack()                          { return m_track; }
    QPointF                 startPosition()                     { return m_start_position; }
    void                    setStartPosition(QPointF position)  { m_start_position = position; }


};


#endif // MIXERITEM_H












