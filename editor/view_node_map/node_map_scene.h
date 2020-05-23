//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef NODE_MAP_SCENE_H
#define NODE_MAP_SCENE_H

#include <QGraphicsScene>
#include <QTreeWidgetItem>

#include "editor/interface_editor_relay.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class DrSettings;
class IEditorRelay;
class NodeMapItem;


//####################################################################################
//##    NodeMapScene
//##        Holds nodes (NodeMapItem) representing DrComponents
//############################
class NodeMapScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to currently loaded Project
    IEditorRelay           *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form

    // Local Variables
    long                    m_node_map_entity           { c_no_key };       // Entity to build Nodes from DrComponents, if c_no_key NodeMapScene is built from DrProject (Worlds, etc)
    bool                    m_need_rebuild_scene        { false };          // Flag to tell View to wait to update Options until scene has been rebuilt

    // Selection Variables
    QList<QGraphicsItem*>   m_selection_items;                              // List of selected items


public:
    // Constructor / Destructor
    explicit NodeMapScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay, long node_map_entity_key = c_no_key);
    virtual ~NodeMapScene() override;

    // Event Overrides
    virtual void    keyPressEvent(QKeyEvent *event) override;
    virtual void    keyReleaseEvent(QKeyEvent *event) override;

    // Construction
    void                    buildScene();                                   // Clears / builds node scene
    void                    clearSceneOverride();                           // Should call this instead of "clear()", prevents dangling pointers and resets local variables

    // Scene Functions
    NodeMapItem*            addItemToSceneFromComponent(DrComponent *component);
    void                    setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y);
    void                    setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y);
    QList<NodeMapItem*>     nodeMapItems();
    static NodeMapItem*     nodeMapItemWithKey(QList<NodeMapItem*> &world_items, long entity_key);

    // Other Widget Update Calls
    IEditorRelay*           getEditorRelay() { return m_editor_relay; }
    void                    updateChangesInScene(std::list<DrSettings*> changed_items, std::list<ComponentProperty> component_property_pairs);
    void                    updateSelectionFromKeyList(QList<long> key_list);
    void                    updateSelectionFromProjectTree(QList<QTreeWidgetItem*> tree_list);
    void                    unselectLockedItems();

    // Getters / Setters
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











