//
//      Created by Stephens Nunnally on 12/26/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrGraphicsItem - Class to hold one Graphics Item within a QGraphicsScene
//
//

#ifndef DR_GRAPHICS_ITEM_H
#define DR_GRAPHICS_ITEM_H

#include <QGraphicsItem>
#include <QGraphicsView>

// Forward declarations
class DrProject;
class DrSettings;
class DrThing;
class IEditorRelay;


//####################################################################################
//##    DrGraphicsItem
//##        A QGraphicsItem for use with our custom QGraphicsScene
//############################
class DrGraphicsItem : public QGraphicsPixmapItem
{

private:
    // External Borrowed Pointers
    DrProject      *m_project;                              // Pointer to the parent project
    IEditorRelay   *m_editor_relay;                         // Pointer to IEditorRelay class of parent form

    DrThing        *m_thing = nullptr;                      // Pointer to the Thing for this item
    long            m_thing_key;                            // Stores the Thing Project Key this item represents

    DrSettings     *m_asset = nullptr;                      // Pointer to the base Entity (Asset, Effect, etc) for this item
    long            m_asset_key;                            // Stores the     base Entity Project Key this item is drawn from
    double          m_asset_width;                          // Width of       base Entity this item is drawn from
    double          m_asset_height;                         // Height of      base Entity this item is drawn from

    // Local Variables
    QPixmap         m_pixmap;                               // Pixmap used to represent this Thing, filters are applied to this

    double          m_start_x;                              // Stores the item position the first time it was loaded
    double          m_start_y;                              // Stores the item position the first time it was loaded

    bool            m_temp_only = false;                    // If this is set to true when item is created, changes to this item are ignored
                                                            // and not processed into the undo stack, nor do changes have an effect on the associated
                                                            // Thing in the project data model
    bool            m_item_change_flags_enabled = false;    // Lets us know if we've enabled or disabled ItemChangeFlags


public:
    // Constructor
    DrGraphicsItem(DrProject *project, IEditorRelay *editor_relay, DrThing *thing, bool is_temp_only = false);

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

    // Item Change Flags
    void                    disableItemChangeFlags();
    void                    enableItemChangeFlags();
    bool                    itemChangeFlagsEnabled()                { return m_item_change_flags_enabled; }

    // Pixmap Filters
    void                    applyFilters();


    // Getters and Setters
    IEditorRelay*           getEditorRelay()    { return m_editor_relay; }

    DrThing*                getThing()          { return m_thing; }
    long                    getThingKey()       { return m_thing_key; }
    DrSettings*             getAsset()          { return m_asset; }
    long                    getAssetKey()       { return m_asset_key; }

    double                  getAssetWidth()               { return m_asset_width; }
    double                  getAssetHeight()              { return m_asset_height; }
    void                    setAssetWidth(double width)   { m_asset_width  = width; }
    void                    setAssetHeight(double height) { m_asset_height = height; }
    void                    setBasePixmap(QPixmap pix)    { m_pixmap = pix; }

    QColor                  getColorAtPoint(QPointF at_local_point);
    QColor                  getColorAtPoint(QPointF at_view_point, QGraphicsView *mouse_over_view);

    double                  startX() { return m_start_x; }
    double                  startY() { return m_start_y; }

    bool                    isTempOnly() { return m_temp_only; }

};

#endif // DR_GRAPHICS_ITEM_H








