//
//      Created by Stephens Nunnally on 12/26/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrItem - Class to hold one Graphics Item within a QGraphicsScene
//
//

#ifndef DRITEM_H
#define DRITEM_H

#include <QGraphicsItem>
#include <QGraphicsView>

// Forward declarations
class DrProject;
class DrAsset;
class DrThing;
class IEditorRelay;

//####################################################################################
//##    DrItem
//##        A QGraphicsItem for use with our custom QGraphicsScene
//############################
class DrItem : public QGraphicsPixmapItem
{

private:
    // External Borrowed Pointers
    DrProject      *m_project;                              // Pointer to the parent project
    IEditorRelay   *m_editor_relay;                         // Pointer to IEditorRelay class of parent form

    DrThing        *m_thing = nullptr;                      // Pointer to the Thing for this item
    long            m_thing_key;                            // Stores the Thing Project Key this item represents

    DrAsset        *m_asset = nullptr;                      // Pointer to the asset for this item
    long            m_asset_key;                            // Stores the asset Project Key this item is drawn from
    double          m_asset_width;                          // Width of asset this item is drawn from
    double          m_asset_height;                         // Height of asset this item is drawn from

    // Local Variables
    QPixmap         m_pixmap;                               // Pixmap used to represent this Thing, filters are applied to this

    double          m_start_x;                              // Stores the item position the first time it was loaded
    double          m_start_y;                              // Stores the item position the first time it was loaded

    bool            m_temp_only = false;                    // If this is set to true when item is created, changes to this item are ignored
                                                            // and not processed into the undo stack, nor do changes have an effect on the associated
                                                            // Thing in the project data model

public:
    // Constructor
    DrItem(DrProject *project, IEditorRelay *editor_relay, DrThing *thing, bool is_temp_only = false);

    // Base Getter Overrides
    virtual QRectF          boundingRect() const override;
    virtual QPainterPath    shape() const override;

    // Event Overrides
    virtual QVariant        itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual void            paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
    virtual void            mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void            mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void            mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void            hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    // Function Calls
    void                    disableItemChangeFlags();
    void                    enableItemChangeFlags();

    // Pixmap Filters
    void                    applyFilters();



    // Getters and Setters
    IEditorRelay*           getRelay()       { return m_editor_relay; }

    DrThing*                getThing()       { return m_thing; }
    long                    getThingKey()    { return m_thing_key; }
    DrAsset*                getAsset()       { return m_asset; }
    long                    getAssetKey()    { return m_asset_key; }

    double                  getAssetWidth()               { return m_asset_width; }
    double                  getAssetHeight()              { return m_asset_height; }
    void                    setAssetWidth(double width)   { m_asset_width  = width; }
    void                    setAssetHeight(double height) { m_asset_height = height; }
    void                    setBasePixmap(QPixmap pix)    { m_pixmap = pix; }

    QColor                  getColorAtPoint(QPointF at_local_point);
    QColor                  getColorAtPoint(QPointF at_view_point, QGraphicsView *mouse_over_view);

    double                  startX() { return m_start_x; }
    double                  startY() { return m_start_y; }

};

#endif // DRITEM_H







