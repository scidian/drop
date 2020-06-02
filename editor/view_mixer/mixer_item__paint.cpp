//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "editor/view_mixer/mixer_item.h"
#include "editor/view_mixer/mixer_scene.h"
#include "editor/view_mixer/mixer_view.h"
#include "engine/debug_flags.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_sound.h"
#include "project/entities_sound/dr_track.h"


//####################################################################################
//##    Custom Paint Handling
//####################################################################################
void MixerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (m_track == nullptr) return;

    // ***** Turn on anti aliasing if necessary
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor)) {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    }

    QGraphicsPixmapItem::paint(painter, option, widget);
}










