//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QGraphicsScene so we can override events for our Scene
//
//
#ifndef EDITOR_SCENE_SCENE_H
#define EDITOR_SCENE_SCENE_H

#include <QtWidgets>

#include <editor_scene_item.h>

class DrProject;
class InterfaceRelay;
class SelectionGroup;

class SceneGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // Local member variables
    DrProject          *m_project;                  // Pointer to currently loaded project
    InterfaceRelay     *m_relay;                    // Pointer to InterfaceRelay class of parent form


    // Selection variables
    SelectionGroup     *m_selection_group;          // Holds the group of items currently selected

public:
    // Constructor
    explicit SceneGraphicsScene(QWidget *parent, DrProject *project, InterfaceRelay *relay);
    virtual ~SceneGraphicsScene() override;

    // Event Overrides, start at Qt Docs for QGraphicsScene Class to find more
    virtual void    keyPressEvent(QKeyEvent *event) override;                              // Inherited from QGraphicsScene
    virtual void    keyReleaseEvent(QKeyEvent *event) override;                            // Inherited from QGraphicsScene

    // Scene Functions
    void            addSquare(qreal new_x, qreal new_y, double new_width, double new_height, double z_order,
                              QColor color = QColor::fromRgb(QRandomGenerator::global()->generate()).light(100));
    void            setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y);
    void            setPositionByOrigin(QGraphicsItem *item, Origin by_origin, double new_x, double new_y);
    QRectF          totalSelectedItemsSceneRect();

    // Selection Functions
    void            addItemToSelectionGroup(QGraphicsItem *item);
    void            emptySelectionGroup(bool delete_items_during_empty = false);
    QGraphicsItem*  getItemAtPosition(QPoint position);
    void            resetSelectionGroup();
    void            selectSelectionGroup();

    // Getters and Setters
    SelectionGroup*       getSelectionGroup();
    QGraphicsItem*        getSelectionGroupAsGraphicsItem();
    QList<QGraphicsItem*> getSelectionGroupItems();

public slots:
    void            sceneChanged(QList<QRectF> region);

};


//############################
//##    SelectionGroup
//##        A sub classed QGraphicsItemGroup that can hold selected items in a scene
//############################
class SelectionGroup : public QGraphicsItemGroup
{
private:

public:
    // Constructor / destructor
    SelectionGroup() {}
    virtual ~SelectionGroup() override;

    // Event Overrides
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

};





#endif // EDITOR_SCENE_SCENE_H









