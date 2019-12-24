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
        DrColor border_color = objectDebugColor(object);
        if (object->isDying() || !object->isAlive()) border_color = Dr::gray;
        if (!object->doesCollide()) border_color = border_color.lighter();
        DrColor fill_color = border_color;
        fill_color.setAlphaF(0.4);

        // Load Object Position
        DrPointF center = object->getPosition();

        // Used to store combined polygon of a multi-shape body
        QPolygonF object_poly;

        for (auto shape : object->shapes) {

            if (object->shape_type[shape] == Shape_Type::Circle) {
                cpVect offset = cpCircleShapeGetOffset(shape);
                float  radius = static_cast<float>(cpCircleShapeGetRadius(shape));
                drawDebugCircle( DrPointF(center.x + offset.x, center.y + offset.y), radius, static_cast<float>(object->getAngle()), fill_color, border_color );

            } else if (object->shape_type[shape] == Shape_Type::Segment) {
                cpVect a = cpSegmentShapeGetA( shape );
                cpVect b = cpSegmentShapeGetB( shape );
                float  radius = static_cast<float>(cpSegmentShapeGetRadius( shape ));
                drawDebugLine( a, b, radius, fill_color, border_color );

            } else if (object->shape_type[shape] == Shape_Type::Polygon || object->shape_type[shape] == Shape_Type::Box) {


            }   // End If

        }   // End For shape



    }   // End For object
}




//void ChipmunkDebugDrawDot(cpFloat size, cpVect pos, cpSpaceDebugColor fillColor)
//{
//	float r = (float)(size*0.5f*ChipmunkDebugDrawPointLineScale);
//	RGBA8 fill = cp_to_rgba(fillColor);
//	Vertex *vertexes = push_vertexes(4, (Index[]){0, 1, 2, 0, 2, 3}, 6);
//	vertexes[0] = (Vertex){{(float)pos.x, (float)pos.y}, {-1, -1}, r, fill, fill};
//	vertexes[1] = (Vertex){{(float)pos.x, (float)pos.y}, {-1,  1}, r, fill, fill};
//	vertexes[2] = (Vertex){{(float)pos.x, (float)pos.y}, { 1,  1}, r, fill, fill};
//	vertexes[3] = (Vertex){{(float)pos.x, (float)pos.y}, { 1, -1}, r, fill, fill};
//}

//void ChipmunkDebugDrawCircle(cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
//{
//	float r = (float)radius + ChipmunkDebugDrawPointLineScale;
//	RGBA8 fill = cp_to_rgba(fillColor), outline = cp_to_rgba(outlineColor);
//	Vertex *vertexes = push_vertexes(4, (Index[]){0, 1, 2, 0, 2, 3}, 6);
//	vertexes[0] = (Vertex){{(float)pos.x, (float)pos.y}, {-1, -1}, r, fill, outline};
//	vertexes[1] = (Vertex){{(float)pos.x, (float)pos.y}, {-1,  1}, r, fill, outline};
//	vertexes[2] = (Vertex){{(float)pos.x, (float)pos.y}, { 1,  1}, r, fill, outline};
//	vertexes[3] = (Vertex){{(float)pos.x, (float)pos.y}, { 1, -1}, r, fill, outline};

//	ChipmunkDebugDrawSegment(pos, cpvadd(pos, cpvmult(cpvforangle(angle), 0.75f*radius)), outlineColor);
//}

//void ChipmunkDebugDrawSegment(cpVect a, cpVect b, cpSpaceDebugColor color)
//{
//	ChipmunkDebugDrawFatSegment(a, b, 0.0f, color, color);
//}

//void ChipmunkDebugDrawFatSegment(cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
//{
//	static const Index indexes[] = {0, 1, 2, 1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7};
//	Vertex *vertexes = push_vertexes(8, indexes, 18);

//	cpVect t = cpvnormalize(cpvsub(b, a));

//	float r = (float)radius + ChipmunkDebugDrawPointLineScale;
//	RGBA8 fill = cp_to_rgba(fillColor), outline = cp_to_rgba(outlineColor);

//    vertexes[0] = (Vertex){{(float)a.x, (float)a.y}, {(float)(-t.x + t.y), (float)(-t.x - t.y)}, r, fill, outline};
//	vertexes[1] = (Vertex){{(float)a.x, (float)a.y}, {(float)(-t.x - t.y), (float)(+t.x - t.y)}, r, fill, outline};
//	vertexes[2] = (Vertex){{(float)a.x, (float)a.y}, {(float)(-0.0 + t.y), (float)(-t.x + 0.0)}, r, fill, outline};
//	vertexes[3] = (Vertex){{(float)a.x, (float)a.y}, {(float)(-0.0 - t.y), (float)(+t.x + 0.0)}, r, fill, outline};
//	vertexes[4] = (Vertex){{(float)b.x, (float)b.y}, {(float)(+0.0 + t.y), (float)(-t.x - 0.0)}, r, fill, outline};
//	vertexes[5] = (Vertex){{(float)b.x, (float)b.y}, {(float)(+0.0 - t.y), (float)(+t.x - 0.0)}, r, fill, outline};
//	vertexes[6] = (Vertex){{(float)b.x, (float)b.y}, {(float)(+t.x + t.y), (float)(-t.x + t.y)}, r, fill, outline};
//	vertexes[7] = (Vertex){{(float)b.x, (float)b.y}, {(float)(+t.x - t.y), (float)(+t.x + t.y)}, r, fill, outline};
//}

//#define MAX_POLY_VERTEXES 64
//// Fill needs (count - 2) triangles.
//// Outline needs 4*count triangles.
//#define MAX_POLY_INDEXES (3*(5*MAX_POLY_VERTEXES - 2))

//void ChipmunkDebugDrawPolygon(int count, const cpVect *verts, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
//{
//	RGBA8 fill = cp_to_rgba(fillColor), outline = cp_to_rgba(outlineColor);

//	Index indexes[MAX_POLY_INDEXES];

//	// Polygon fill triangles.
//	for(int i = 0; i < count - 2; i++){
//		indexes[3*i + 0] = 0;
//		indexes[3*i + 1] = 4*(i + 1);
//		indexes[3*i + 2] = 4*(i + 2);
//	}

//	// Polygon outline triangles.
//	Index *cursor = indexes + 3*(count - 2);
//	for(int i0 = 0; i0 < count; i0++){
//		int i1 = (i0 + 1)%count;
//		cursor[12*i0 +  0] = 4*i0 + 0;
//		cursor[12*i0 +  1] = 4*i0 + 1;
//		cursor[12*i0 +  2] = 4*i0 + 2;
//		cursor[12*i0 +  3] = 4*i0 + 0;
//		cursor[12*i0 +  4] = 4*i0 + 2;
//		cursor[12*i0 +  5] = 4*i0 + 3;
//		cursor[12*i0 +  6] = 4*i0 + 0;
//		cursor[12*i0 +  7] = 4*i0 + 3;
//		cursor[12*i0 +  8] = 4*i1 + 0;
//		cursor[12*i0 +  9] = 4*i0 + 3;
//		cursor[12*i0 + 10] = 4*i1 + 0;
//		cursor[12*i0 + 11] = 4*i1 + 1;
//	}

//	float inset = (float)-cpfmax(0, 2*ChipmunkDebugDrawPointLineScale - radius);
//	float outset = (float)radius + ChipmunkDebugDrawPointLineScale;
//	float r = outset - inset;

//	Vertex *vertexes = push_vertexes(4*count, indexes, 3*(5*count - 2));
//	for(int i=0; i<count; i++){
//		cpVect v0 = verts[i];
//		cpVect v_prev = verts[(i+(count - 1))%count];
//		cpVect v_next = verts[(i+(count + 1))%count];

//		cpVect n1 = cpvnormalize(cpvrperp(cpvsub(v0, v_prev)));
//		cpVect n2 = cpvnormalize(cpvrperp(cpvsub(v_next, v0)));
//		cpVect of = cpvmult(cpvadd(n1, n2), 1.0/(cpvdot(n1, n2) + 1.0f));
//		cpVect v = cpvadd(v0, cpvmult(of, inset));

//		vertexes[4*i + 0] = (Vertex){{(float)v.x, (float)v.y}, {0.0f, 0.0f}, 0.0f, fill, outline};
//		vertexes[4*i + 1] = (Vertex){{(float)v.x, (float)v.y}, {(float)n1.x, (float)n1.y}, r, fill, outline};
//		vertexes[4*i + 2] = (Vertex){{(float)v.x, (float)v.y}, {(float)of.x, (float)of.y}, r, fill, outline};
//		vertexes[4*i + 3] = (Vertex){{(float)v.x, (float)v.y}, {(float)n2.x, (float)n2.y}, r, fill, outline};
//	}
//}












