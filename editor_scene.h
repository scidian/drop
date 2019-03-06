//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QGraphicsScene so we can override events for our Stage
//
//
#ifndef EDITOR_SCENE_H
#define EDITOR_SCENE_H

#include <QGraphicsScene>
#include <QTreeWidgetItem>
#include <QUndoStack>

#include "interface_editor_relay.h"

class DrItem;
class DrProject;
class DrStage;
class IEditorRelay;


//####################################################################################
//##    Possible handle rects, order is vector / array index critical
//####################################################################################
enum class Position_Flags {
    Top          = 0,
    Bottom       = 1,
    Right        = 2,
    Left         = 3,
    Top_Left     = 4,
    Bottom_Left  = 5,
    Top_Right    = 6,
    Bottom_Right = 7,
    Total,
    No_Position,
    Move_Item,
    Rotate,
    Center
};


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
    IEditorRelay           *m_editor_relay;                 // Pointer to IEditorRelay class of parent form

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
    explicit DrScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);
    virtual ~DrScene() override;

    // Event Overrides, start at Qt Docs for QGraphicsScene Class to find more
    virtual void    keyPressEvent(QKeyEvent *event) override;                              // Inherited from QGraphicsScene
    virtual void    keyReleaseEvent(QKeyEvent *event) override;                            // Inherited from QGraphicsScene

    // Scene Functions
    DrItem*         addItemToSceneFromObject(DrObject *object);
    void            setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y);
    void            setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y);

    // Other Widget Update Calls
    IEditorRelay*   getRelay() { return m_editor_relay; }
    void            clearViewSceneRect(QRectF new_rect) { emit clearViewRect(new_rect); }
    void            updateAlignmentGrid() { emit updateGrid(); }
    void            updateChangesInScene(QList<DrSettings*> changed_items, QList<long> property_keys);
    void            updateItemInScene(DrSettings* changed_item, QList<long> property_keys);
    void            updateSelectionFromProjectTree(QList<QTreeWidgetItem*> tree_list);
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
    void                    resetSelectionGroup();
    bool                    shouldEnableResizeToGrid();
    QRectF                  totalSelectionSceneRect();
    void                    updateSelectionBox();


public slots:
    void            sceneChanged(QList<QRectF>);                            // Used to resize scene area to fit contents
    void            sceneRectChanged(QRectF new_rect);
    void            selectionChanged();

    // Undo Commands
    void            newStageSelected(DrProject *project, DrScene *scene, long old_stage, long new_stage);
    void            selectionGroupMoved(DrScene *scene, const QPointF &old_position);
    void            selectionGroupNewGroup(DrScene *scene, QList<DrObject*> old_list, QList<DrObject*> new_list);

signals:
    void            clearViewRect(QRectF new_rect);                         // Connected to clearViewSceneRect() function of attached Views
    void            updateGrid();                                           // Connected to updateGrid() function of attached Views
    void            updateViews();                                          // Connected to update() function of attached Views

};







#endif // EDITOR_SCENE_H









