//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QKeyEvent>

#include "editor/view_mixer/mixer_item.h"
#include "editor/view_mixer/mixer_scene.h"
#include "editor/view_mixer/mixer_view.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_sound.h"
#include "project/entities_sound/dr_track.h"


//####################################################################################
//##    Key Release
//####################################################################################
void MixerScene::keyReleaseEvent(QKeyEvent *event) {
    // Reset key timer
    Qt::Key key_released = static_cast<Qt::Key>(event->key());
    m_key_down[key_released] = false;

    QGraphicsScene::keyReleaseEvent(event);
}


//####################################################################################
//##    Key Press
//####################################################################################
void MixerScene::keyPressEvent(QKeyEvent *event) {

    // ***** If no selected items, pass on key press event to base class and exit
    if (selectedItems().size() < 1) {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    // ***** Start tracking time key has been pressed
    Qt::Key key_pressed = static_cast<Qt::Key>(event->key());
    if (m_key_down[key_pressed] == false) {
        m_key_timers[key_pressed] = QElapsedTimer();
        m_key_timers[key_pressed].restart();
        m_key_down[key_pressed] = true;
    }

}

