//
//      Created by Stephens Nunnally on 5/1/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WORLD_MAP_ITEM_H
#define WORLD_MAP_ITEM_H

#include <QGraphicsItem>
#include <QGraphicsView>

#include "project/enums_entity_types.h"

// Forward Declarations
class DrProject;
class IEditorRelay;


//####################################################################################
//##    WorldMapItem
//##        A QGraphicsItem for a node (DrProject::DrNode) for use with our custom QGraphicsScene (WorldMapScene)
//############################
class WorldMapItem : public QGraphicsPixmapItem
{

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                      // Pointer to the parent project
    IEditorRelay           *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form

    // Local Variables
    DrSettings             *m_entity;                                       // Pointer to the base Entity (World, UI) in DrProject for this item (node)
    long                    m_entity_key   { c_no_key };                    // Project Key to the World / UI that this item (node) represents in DrProject


public:
    // Constructor / Destructor
    WorldMapItem(DrProject *project, IEditorRelay *editor_relay, long project_key);
    virtual ~WorldMapItem() override;


};

#endif // WORLD_MAP_ITEM_H








