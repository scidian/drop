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
//##        Holds nodes (WorldMapItem) representing layers (DrProject::DrNode - Worlds, UI, etc) of DrProject
//############################
class WorldMapScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                          // Pointer to currently loaded Project
    IEditorRelay           *m_editor_relay;                     // Pointer to IEditorRelay class of parent form


public:
    // Constructor / Destructor
    explicit WorldMapScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);
    virtual ~WorldMapScene() override;

    // Construction
    void                    buildScene();                                   // Clears / builds node scene

    // Scene Functions
    WorldMapItem*           addItemToSceneFromEntity(DrSettings *entity);
    void                    setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y);
    void                    setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y);

    // Other Widget Update Calls
    IEditorRelay*           getEditorRelay() { return m_editor_relay; }
    void                    updateChangesInScene(std::list<DrSettings*> changed_items, std::list<ComponentProperty> component_property_pairs);


public slots:
    void            sceneChanged(QList<QRectF>);                            // Used to resize scene area to fit contents

};

#endif // WORLD_MAP_SCENE_H








