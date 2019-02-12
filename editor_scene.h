//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QGraphicsScene so we can override events for our Stage
//
//
#ifndef EDITOR_SCENE_H
#define EDITOR_SCENE_H

#include <QtWidgets>

#include <editor_item.h>

class DrProject;
class DrStage;
class InterfaceRelay;


//####################################################################################
//##    DrScene
//##        Holds items of one stage
//############################
class DrScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // Local member variables
    DrProject              *m_project;                      // Pointer to currently loaded project
    InterfaceRelay         *m_relay;                        // Pointer to InterfaceRelay class of parent form

    DrStage                *m_current_stage = nullptr;      // Holds a pointer to the current DrStage being shown
    long                    m_current_stage_key = -1;       // Holds the project key of the currently shown DrStage, starts at -1, i.e. "none"

    QUndoStack             *m_undo;


    // Selection variables
    QList<QGraphicsItem*>   m_selection_items;              // List of selected items
    double                  m_selection_angle;              // Angle current selection has been rotated to
    QPointF                 m_selection_scale;              // Scaling applied to current selection
    QRectF                  m_selection_box;                // Starting outline of selected items


public:
    // Mutexes
    QMutex       scene_mutex { QMutex::NonRecursive };         // Used to stop from changing scene items more than once at a time


public:
    // Constructor
    explicit DrScene(QWidget *parent, DrProject *project, InterfaceRelay *relay);
    virtual ~DrScene() override;

    // Event Overrides, start at Qt Docs for QGraphicsScene Class to find more
    virtual void    keyPressEvent(QKeyEvent *event) override;                              // Inherited from QGraphicsScene
    virtual void    keyReleaseEvent(QKeyEvent *event) override;                            // Inherited from QGraphicsScene

    // Scene Functions
    void            setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y);
    void            setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y);

    // Other Widget Update Calls
    InterfaceRelay* getRelay() { return m_relay; }
    void            updateStageTreeSelection() { m_relay->updateStageTreeSelectionBasedOnSelectionGroup(); }
    void            updateView() { emit updateViews(); }

    // Undo / Redo Functions
    void            undoAction();
    void            redoAction();
    QString         getCurrentUndo() { return m_undo->undoText(); }
    QString         getCurrentRedo() { return m_undo->redoText(); }

    // Getters and Setters
    DrStage*        getCurrentStageShown() { return m_current_stage; }
    void            setCurrentStageShown(DrStage *stage) { m_current_stage = stage; }
    long            getCurrentStageKeyShown() { return m_current_stage_key; }
    void            setCurrentStageKeyShown(long stage_key) { m_current_stage_key = stage_key; }


    // ********** Selection Functions
    long                    getSelectionCount()         { return m_selection_items.count(); }
    QTransform              getSelectionTransform();

    QList<QGraphicsItem*>   getSelectionItems()         { return m_selection_items; }
    double                  getSelectionAngle()         { return m_selection_angle; }
    QPointF                 getSelectionScale()         { return m_selection_scale; }
    QRectF                  getSelectionBox()           { return m_selection_box; }

    void                    setSelectionItems(QList<QGraphicsItem*> list)    { m_selection_items = list; }
    void                    setSelectionAngle(double angle)                  { m_selection_angle = angle; }
    void                    setSelectionScale(QPointF scale)                 { m_selection_scale = scale; }
    void                    setSelectionBox(QRectF box)                      { m_selection_box = box; }

    QGraphicsItemGroup*     createEmptyItemGroup(double angle = 0, QPointF scale = QPointF(1, 1));
    QRectF                  totalSelectionSceneRect();
    void                    updateSelectionBox();

    QList<DrObject*>        convertListItemsToObjects(QList<QGraphicsItem*> graphics_items);
    QList<DrObject*>        getSelectionGroupObjects();


public slots:
    void            sceneChanged(QList<QRectF> region);                             // Used to resize scene area to fit contents
    void            selectionChanged();

    // Undo Commands
    void            newStageSelected(DrProject *project, DrScene *scene, long old_stage, long new_stage);
    void            selectionGroupMoved(DrScene *scene, const QPointF &old_position);
    void            selectionGroupNewGroup(DrScene *scene, QList<DrObject*> old_list, QList<DrObject*> new_list);

signals:
    void            updateViews();                                                  // Connected to update() method of attached Views

};







#endif // EDITOR_SCENE_H









