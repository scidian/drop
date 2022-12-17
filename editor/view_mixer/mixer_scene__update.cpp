//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "editor/view_mixer/mixer_item.h"
#include "editor/view_mixer/mixer_scene.h"
#include "editor/view_mixer/mixer_view.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_sound.h"
#include "project/entities_sound/dr_track.h"




//####################################################################################
//##    Update
//##        Called after some change has been made to items on another widget besides
//##        the QGraphicsView, updates the items in the scene
//####################################################################################
void MixerScene::updateChangesInScene(std::list<DrSettings*> changed_items, std::list<ComponentProperty> component_property_pairs) {
    if (changed_items.empty()) return;

    (void) component_property_pairs;

    for (auto settings_item : changed_items) {
        DrType my_type = settings_item->getType();

        if (my_type == DrType::Mix) {

        } else if (my_type == DrType::Sound) {

        } else if (my_type == DrType::Track) {

        }

        //################ !!!!!!!!!!!!!!!!!!!!!!!
        //##    ROOM FOR MORE TYPES
        //################ !!!!!!!!!!!!!!!!!!!!!!!
    }

    // ***** Try to redraw in case items have changed
    this->update();
    for (auto item : this->items()) {           // Forces repaint of all GraphicsItems
        item->update();
    }
}

















