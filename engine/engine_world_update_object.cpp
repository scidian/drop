//
//      Created by Stephens Nunnally on 6/14/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>

#include "engine.h"
#include "engine_thing_object.h"
#include "engine_world.h"


//######################################################################################################
//##    Updates Kinematic Angular Velocity
//######################################################################################################
extern void KinematicUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt) {
    // Grab object from User Data
    DrEngineObject *object = static_cast<DrEngineObject*>(cpBodyGetUserData(body));

    // Figure out new velocity based on current object angle
    if (object->getUseAngleVelocity()) {
        double  angle = qRadiansToDegrees( cpBodyGetAngle(body) );
        QPointF original = QPointF( object->getOriginalVelocityX(), object->getOriginalVelocityY() );
        QPointF rotated = QTransform().rotate(angle).map(original);
        cpBodySetVelocity( body, cpv(rotated.x(), rotated.y()) );
    }

    // Call real update function
    cpBodyUpdateVelocity(body, gravity, damping, dt);
}
