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


//####################################################################################
//##    SceneGraphicsScene
//##        Holds items of one scene
//############################
class SceneGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // Local member variables
    DrProject          *m_project;                  // Pointer to currently loaded project
    InterfaceRelay     *m_relay;                    // Pointer to InterfaceRelay class of parent form

    // Selection variables
    SelectionGroup     *m_selection_group;          // Holds the group of items currently selected

    QUndoStack         *m_undo;

public:
    // Mutexes
    QMutex       scene_mutex { QMutex::NonRecursive };         // Used to stop from changing scene more than once at a time


public:
    // Constructor
    explicit SceneGraphicsScene(QWidget *parent, DrProject *project, InterfaceRelay *relay);
    virtual ~SceneGraphicsScene() override;

    // Event Overrides, start at Qt Docs for QGraphicsScene Class to find more
    virtual void    keyPressEvent(QKeyEvent *event) override;                              // Inherited from QGraphicsScene
    virtual void    keyReleaseEvent(QKeyEvent *event) override;                            // Inherited from QGraphicsScene

    // Scene Functions
    void            addSquare(qreal new_x, qreal new_y, double new_width, double new_height, double z_order, QString name,
                              QColor color = QColor::fromRgb(QRandomGenerator::global()->generate()).light(100));
    void            setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y);
    void            setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y);
    QRectF          totalSelectedItemsSceneRect();
    void            updateView() { emit updateViews(); }

    // Selection Functions
    void            addItemToSelectionGroup(QGraphicsItem *item);
    void            emptySelectionGroup(bool delete_items_during_empty = false);
    QGraphicsItem*  getItemAtPosition(QPointF position);
    void            resetSelectionGroup();
    void            selectSelectionGroup();

    // Custom Add / Remove from Selection Group without calling update()
    void            addToGroupNoUpdate(QGraphicsItem *item);
    void            removeFromGroupNoUpdate(QGraphicsItem *item);

    // Getters and Setters
    SelectionGroup*       getSelectionGroup();
    QGraphicsItem*        getSelectionGroupAsGraphicsItem();
    QList<QGraphicsItem*> getSelectionGroupItems();
    int                   getSelectionGroupCount();

public slots:
    void            sceneChanged(QList<QRectF> region);                             // Used to resize scene area to fit contents

    // Undo Commands
    void            itemMoved(SelectionGroup *moved_group, const QPointF &old_position);


signals:
    void            updateViews();                                                  // Connected to update() method of attached Views

};



//####################################################################################
//##    SelectionGroup
//##        A sub classed QGraphicsItemGroup that can hold selected items in a scene
//############################
class SelectionGroup : public QGraphicsItemGroup
{
private:
    SceneGraphicsScene  *m_parent_scene;

public:
    // Constructor / destructor
    SelectionGroup(SceneGraphicsScene *parent_scene) : m_parent_scene(parent_scene) {}
    virtual ~SelectionGroup() override;

    // Event Overrides
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // Getters and Setters
    SceneGraphicsScene* getParentScene() { return m_parent_scene; }
};




//####################################################################################
//##    Undo Commands
//##        Commands used with local m_undo QUndoStack
//############################
class MoveCommand : public QUndoCommand
{
public:
    enum { Id = 1234 };

    MoveCommand(SelectionGroup *group, const QPointF &old_pos, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    int  id() const override { return Id; }

private:
    SelectionGroup      *m_group;
    QPointF              m_old_pos;
    QPointF              m_new_pos;
};


QString createCommandString(SelectionGroup *group, const QPointF &point);



#endif // EDITOR_SCENE_SCENE_H









