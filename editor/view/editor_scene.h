//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QGraphicsScene so we can override events for our Stage
//
//
#ifndef EDITOR_SCENE_H
#define EDITOR_SCENE_H

#include <QElapsedTimer>
#include <QGraphicsScene>
#include <QMutex>
#include <QTreeWidgetItem>
#include <QUndoStack>

#include "editor/interface_editor_relay.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class DrStage;
class EditorItem;
class IEditorRelay;


//####################################################################################
//##    EditorScene
//##        Holds items of one stage
//############################
class EditorScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject      *m_project;                              // Pointer to currently loaded project
    IEditorRelay   *m_editor_relay;                         // Pointer to IEditorRelay class of parent form

    // Local Variables
    QUndoStack     *m_undo = nullptr;                       // Object that holds list of changes to QGraphicsScene for use with undo / redo

    DrStage        *m_current_stage = nullptr;              // Holds a pointer to the current DrStage being shown
    long            m_current_stage_key = c_no_key;         // Holds the Project Key of the currently shown DrStage, starts at c_no_key, i.e. "none"

    // Selection Variables
    QList<QGraphicsItem*>   m_selection_items;              // List of selected items
    double                  m_selection_angle;              // Angle current selection has been rotated to
    QPointF                 m_selection_scale;              // Scaling applied to current selection
    QRectF                  m_selection_box;                // Starting outline of selected items

    // Selection Movement Variables
    bool                    m_calculated_adjust;            // If an item has already recalculated new center, use that
    QPointF                 m_pre_move_center;              // Stores a premove center for group grid snapping
    QPointF                 m_move_adjustment;              // Amount to move items by during translation

    // Timers for holding keys down
    QMap<Qt::Key, QElapsedTimer>    m_key_timers;
    QMap<Qt::Key, bool>             m_key_down;


public:
    // Mutexes
    QMutex       scene_mutex { QMutex::NonRecursive };      // Used to stop from changing scene items more than once at a time


public:
    // Constructor
    explicit EditorScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);
    virtual ~EditorScene() override;

    // Event Overrides, start at Qt Docs for QGraphicsScene Class to find more
    virtual void    keyPressEvent(QKeyEvent *event) override;                              // Inherited from QGraphicsScene
    virtual void    keyReleaseEvent(QKeyEvent *event) override;                            // Inherited from QGraphicsScene

    // Scene Functions
    EditorItem* addItemToSceneFromThing(DrThing *thing);
    void            setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y);
    void            setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y);

    // Other Widget Update Calls
    IEditorRelay*   getEditorRelay() { return m_editor_relay; }
    void            updateChangesInScene(std::list<DrSettings*> changed_items, std::list<ComponentProperty> component_property_pairs);
    void            updateItemInScene(DrSettings* changed_item, std::list<ComponentProperty> component_property_pairs);
    void            updateItemZValues();
    void            updateSelectionFromKeyList(QList<long> key_list);
    void            updateSelectionFromProjectTree(QList<QTreeWidgetItem*> tree_list);
    void            unselectLockedItems();

    // Undo / Redo Functions
    void            undoAction();
    void            redoAction();
    QString         getCurrentUndo() { return m_undo->undoText(); }
    QString         getCurrentRedo() { return m_undo->redoText(); }

    // Stage Handling
    DrStage*        getCurrentStageShown() { return m_current_stage; }
    void            setCurrentStageShown(DrStage *stage) { m_current_stage = stage; }
    long            getCurrentStageKeyShown() { return m_current_stage_key; }
    void            setCurrentStageKeyShown(long stage_key) { m_current_stage_key = stage_key; }
    void            clearStageShown();


    // ********** Selection Functions
    long                    getSelectionCount()         { return m_selection_items.count(); }
    QTransform              getSelectionTransform();

    QList<QGraphicsItem*>   getSelectionItems()         { return m_selection_items; }
    QList<DrThing*>         getSelectionItemsAsThings();
    double                  getSelectionAngle()         { return m_selection_angle; }
    QPointF                 getSelectionScale()         { return m_selection_scale; }
    QRectF                  getSelectionBox()           { return m_selection_box; }

    void                    setSelectionItems(QList<QGraphicsItem*> list)   { m_selection_items = list; }
    void                    setSelectionAngle(double angle)                 { m_selection_angle = angle; }
    void                    setSelectionScale(QPointF scale)                { m_selection_scale = scale; }
    void                    setSelectionBox(QRectF box)                     { m_selection_box = box; }

    QGraphicsItemGroup*     createEmptyItemGroup(double angle = 0, QPointF scale = QPointF(1, 1));
    bool                    checkAllSelectedItemsHaveSameAngle();
    void                    resetSelectionGroup();
    bool                    shouldEnableResizeToGrid();
    QRectF                  totalSelectionSceneRect();
    void                    updateSelectionBox();


    // ********** Seelction Movement Functions
    QPointF                 getPreMoveSelectionCenter()                     { return m_pre_move_center; }
    QPointF                 getMoveAdjustment()                             { return m_move_adjustment; }
    bool                    getHasCalculatedAdjustment()                    { return m_calculated_adjust; }

    void                    setPreMoveSelectionCenter(QPointF center)       { m_pre_move_center = center; }
    void                    setMoveAdjustment(QPointF adjust)               { m_move_adjustment = adjust; }
    void                    setHasCalculatedAdjustment(bool has)            { m_calculated_adjust = has; }


public slots:
    void            sceneChanged(QList<QRectF>);                            // Used to resize scene area to fit contents
    void            sceneRectChanged(QRectF new_rect);
    void            selectionChanged();

    // Undo Commands
    void            newStageSelected(DrProject *project, EditorScene *scene, long old_stage, long new_stage);
    void            selectionGroupMoved(EditorScene *scene, const QPointF &old_position);
    void            selectionGroupNewGroup(EditorScene *scene, QList<DrThing*> old_list, QList<DrThing*> new_list);

signals:
    void            setViewRect(QRectF new_rect);                           // Connected to setViewRect() function of attached Views
    void            updateGrid();                                           // Connected to updateGrid() function of attached Views
    void            updateViews();                                          // Connected to update() function of attached Views

};







#endif // EDITOR_SCENE_H









