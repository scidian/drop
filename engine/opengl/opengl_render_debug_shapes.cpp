//
//      Created by Stephens Nunnally on 12/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Draws the Collision Shapes using Shaders
//####################################################################################
void DrOpenGL::drawDebugShapes2() {

    for (auto thing : m_engine->getCurrentWorld()->getThings()) {
        if (thing->getThingType() != DrThingType::Object) continue;
        DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);

        // Figure out what color to make the debug shapes
        DrColor color = objectDebugColor(object);
        if (object->isDying() || !object->isAlive()) color = Qt::gray;
        if (!object->doesCollide()) color = color.lighter();

        // Load Object Position
        DrPointF center = object->getPosition();

        // Used to store combined polygon of a multi-shape body
        QPolygonF object_poly;

        for (auto shape : object->shapes) {

            if (object->shape_type[shape] == Shape_Type::Circle) {
                cpVect offset = cpCircleShapeGetOffset(shape);
                double radius = cpCircleShapeGetRadius(shape);

                drawShapeCircle( DrPointF(center.x + offset.x, center.y + offset.y), static_cast<float>(radius),
                                 static_cast<float>(object->getAngle()), 0.35f, DrColor(color.red(), color.green(), color.blue()) );


            } else if (object->shape_type[shape] == Shape_Type::Segment) {

            } else if (object->shape_type[shape] == Shape_Type::Polygon || object->shape_type[shape] == Shape_Type::Box) {


            }   // End If

        }   // End For shape



    }   // End For object
}














