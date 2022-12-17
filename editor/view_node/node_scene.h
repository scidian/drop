//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef NODE_MAP_SCENE_H
#define NODE_MAP_SCENE_H

#include <QElapsedTimer>
#include <QGraphicsScene>
#include <QTreeWidgetItem>

#include "editor/interface_editor_relay.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class DrSettings;
class IEditorRelay;
class NodeItem;


//####################################################################################
//##    NodeScene
//##        Holds nodes (NodeItem) representing DrComponents
//############################
class NodeScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to currently loaded Project
    IEditorRelay           *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form

    // Local Variables
    long                    m_node_map_entity           { c_no_key };       // Entity to build Nodes from DrComponents, if c_no_key NodeScene is built from DrProject (Worlds, etc)
    bool                    m_need_rebuild_scene        { false };          // Flag to tell View to wait to update Options until scene has been rebuilt

    // Selection Variables
    QList<QGraphicsItem*>   m_selection_items;                              // List of selected items

    // Timers for holding keys down
    QMap<Qt::Key, QElapsedTimer>    m_key_timers;
    QMap<Qt::Key, bool>             m_key_down;

public:
    // Constructor / Destructor
    explicit NodeScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay, long node_map_entity_key = c_no_key);
    virtual ~NodeScene() override;

    // Event Overrides
    virtual void    keyPressEvent(QKeyEvent *event) override;
    virtual void    keyReleaseEvent(QKeyEvent *event) override;

    // Construction
    void                    buildScene();                                   // Clears / builds node scene
    void                    clearSceneOverride();                           // Should call this instead of "clear()", prevents dangling pointers and resets local variables

    // Scene Functions
    NodeItem*               addItemToSceneFromComponent(DrComponent *component);
    void                    setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y);
    void                    setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y);
    QList<NodeItem*>        nodeItems();
    static NodeItem*        nodeItemWithKey(QList<NodeItem*> &world_items, long entity_key);

    // Selection Functions
    QRectF                  totalSelectionSceneRect();

    // Other Widget Update Calls
    void                    updateChangesInScene(std::list<DrSettings*> changed_items, std::list<ComponentProperty> component_property_pairs);
    void                    updateSelectionFromKeyList(QList<long> key_list);
    void                    updateSelectionFromProjectTree(QList<QTreeWidgetItem*> tree_list);
    void                    unselectLockedItems();

    // Getters / Setters
    IEditorRelay*           getEditorRelay()                        { return m_editor_relay; }
    DrProject*              getProject()                            { return m_project; }

    long                    getNodeMapEntityKey()                   { return m_node_map_entity; }
    bool                    needRebuild()                           { return m_need_rebuild_scene; }
    void                    setNeedRebuild(bool need)               { m_need_rebuild_scene = need; }


public slots:
    void            sceneChanged(QList<QRectF>);                            // Used to resize scene area to fit contents
    void            selectionChanged();

signals:
    void            aboutToClear();

};

#endif // NODE_MAP_SCENE_H











