//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef PHYSICS_SPRITE_H
#define PHYSICS_SPRITE_H

#include <QOpenGLTexture>
#include <QPixmap>

#include "chipmunk/chipmunk.h"

enum class BodyType {
    Dynamic,
    Kinematic,
    Static,
};

enum class ShapeType {
    Circle,                                     // cpCircleShapeNew
    Box,                                        // cpBoxShapeNew
    Segment,    // "Line"                       // cpSegmentShapeNew
    Polygon                                     // cpPolyShapeNew
};

struct SceneObject {
    QOpenGLTexture  *texture;

    cpBody     *body;
    cpShape    *shape;

    BodyType    body_type;
    ShapeType   shape_type;

    double      wheel_speed;

    bool        is_wheel = false;
    bool        in_scene = true;

    double      angle;
    QPointF     position;
    QPointF     velocity;

};


#endif // PHYSICS_SPRITE_H
