//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>

#include "form_playground.h"
#include "playground.h"
#include "playground_toy.h"


//####################################################################################
//##    Update Space steps the physics calculations
//##        updateSpaceHelper meant to be called immediately afterwards
//####################################################################################

// #NOTE: time_passed is in milliseconds
void DrPlayground::updateSpace(double time_passed) {
    double step_time = time_passed / 1000.0 * m_time_warp;
    ///double step_time = m_time_step * m_time_warp;
    cpSpaceStep(m_space, step_time);
}


// ********** Iterate through objects, delete if necessary
void DrPlayground::updateSpaceHelper() {
    for ( auto it = objects.begin(); it != objects.end(); ) {
        // ***** Initial loop variables
        DrToy *object = *it;
        object->m_has_been_processed = true;

        // ***** Skip object if static; or if not yet in Space / no longer in Space
        if (!object->m_should_process) {
            it++;
            continue;
        }

        // ***** Get some info about the current object from the space and save it to the current DrEngineObject
        cpVect  new_position = cpBodyGetPosition( object->body );
        object->m_position = QPointF( new_position.x, new_position.y );
        object->m_angle = qRadiansToDegrees( cpBodyGetAngle( object->body ));


        // ***** Process removal
        if (object->m_remove) {
            removeObject(object);
            delete object;
            it = objects.erase(it);
        } else it++;

    }   // End For
}












