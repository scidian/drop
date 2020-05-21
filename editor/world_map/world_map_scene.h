//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WORLD_MAP_SCENE_H
#define WORLD_MAP_SCENE_H

#include <QGraphicsScene>

#include "editor/interface_editor_relay.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class DrSettings;
class IEditorRelay;
class WorldMapItem;


//####################################################################################
//##    WorldMapScene
//##        Holds nodes (WorldMapItem) representing DrComponents
//############################
class WorldMapScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to currently loaded Project
    IEditorRelay           *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form

    // Local Variables
    bool                    m_need_rebuild_scene        { false };          // Flag to tell View to wait to update Options until scene has been rebuilt

public:
    // Constructor / Destructor
    explicit WorldMapScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);
    virtual ~WorldMapScene() override;

    // Construction
    void                    buildScene();                                   // Clears / builds node scene
    void                    clearSceneOverride();                           // Should call this instead of "clear()", prevents dangling pointers and resets local variables

    // Scene Functions
    WorldMapItem*           addItemToSceneFromComponent(DrComponent *component);
    void                    setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y);
    void                    setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y);
    QList<WorldMapItem*>    worldMapItems();
    static WorldMapItem*    worldMapItemWithKey(QList<WorldMapItem*> &world_items, long entity_key);

    // Other Widget Update Calls
    IEditorRelay*           getEditorRelay() { return m_editor_relay; }
    void                    updateChangesInScene(std::list<DrSettings*> changed_items, std::list<ComponentProperty> component_property_pairs);

    // Getters / Setters
    bool                    needRebuild()                           { return m_need_rebuild_scene; }
    void                    setNeedRebuild(bool need)               { m_need_rebuild_scene = need; }


public slots:
    void            sceneChanged(QList<QRectF>);                            // Used to resize scene area to fit contents

signals:
    void            aboutToClear();

};

#endif // WORLD_MAP_SCENE_H








