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


class SceneGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // Local member variables
    DrProject          *m_project;                  // Pointer to currently loaded project
    InterfaceRelay     *m_relay;                    // Pointer to InterfaceRelay class of parent form


    // Selection variables
    int                 m_selection_count = 0;      // Tracks changes in number of selected items

    QGraphicsItemGroup *m_selection_group;          // Holds the group of items currently selected

    QRectF              m_selection_rect;           // Initial rectangle of selection, changed whenever selection is changed
    QPolygonF           m_selection_polygon;        // Holds transformed polygon with any changes to rotate, scale, etc since first selection
    double              m_selection_rotate;         // Any rotation applied to current selection
    QPointF             m_selection_scale;          // Any scaling  applied to current selection


public:
    // Constructor
    explicit SceneGraphicsScene(QWidget *parent, DrProject *project, InterfaceRelay *relay) :
                                QGraphicsScene(parent = nullptr), m_project(project), m_relay(relay) {
        connect(this, SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));
        connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
        m_selection_group = createItemGroup(selectedItems());
        m_selection_group->setFlags(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable |
                                    QGraphicsItem::GraphicsItemFlag::ItemIsMovable |
                                    QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges |
                                    QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges);
    }

    // Event Overrides, start at Qt Docs for QGraphicsScene Class to find more
    virtual void    keyPressEvent(QKeyEvent *event) override;                              // Inherited from QGraphicsScene
    virtual void    keyReleaseEvent(QKeyEvent *event) override;                            // Inherited from QGraphicsScene

    // Functions
    void            addSquare(qreal new_x, qreal new_y, double new_width, double new_height, double z_order,
                              QColor color = QColor::fromRgb(QRandomGenerator::global()->generate()).light(100));
    QRectF          totalSelectedItemsSceneRect();

    void            setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y);
    void            setPositionByOrigin(QGraphicsItem *item, Origin by_origin, double new_x, double new_y);

public slots:
    void            sceneChanged(QList<QRectF> region);
    void            selectionChanged();

};

#endif // EDITOR_SCENE_SCENE_H








