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
                float radius = static_cast<float>(cpPolyShapeGetRadius(shape));
                std::vector<cpVect> vertices;

                // !!!!! Need find center of polygon to draw triangles from two adjacent points plus center

                for (int i = 0; i < cpPolyShapeGetCount(shape); i++) {
                    cpVect vert = cpPolyShapeGetVert(shape, i);
                    vertices.push_back(vert + cpv(center.x, center.y));
                }
                drawDebugPolygon( DrPointF(center.x, center.y), vertices, radius, static_cast<float>(object->getAngle()), fill_color, border_color );

            }   // End If

        }   // End For shape



    }   // End For object
}


//####################################################################################
//##    Vertex for Debug Shapes
//####################################################################################
struct DebugVertex {
public:
    std::vector<float> position;                        // in sets of x, y
    std::vector<float> texture_coordinates;             // in sets of x, y
    std::vector<float> radiuses;                        // in sets of r

    DebugVertex() {
        position.clear();
        texture_coordinates.clear();
        radiuses.clear();
    }

    void addVertex(float x, float y, float uv_x, float uv_y, float r) {
        position.push_back(x);
        position.push_back(y);
        texture_coordinates.push_back(uv_x);
        texture_coordinates.push_back(uv_y);
        radiuses.push_back(r);
    }
};


//####################################################################################
//##    Draws a Debug Shape Using the Debug Shader
//####################################################################################
void DrOpenGL::drawDebugShape(QMatrix4x4 mvp, DrColor fill, DrColor border, float *position, float *uv, float *radius, int triangles) {
    // Enable shader program
    if (!m_debug_shader.bind()) return;

    // ***** Blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);              // Standard non-premultiplied alpha blend

    // ***** Set Shader Variables
    m_debug_shader.setUniformValue( u_debug_matrix,         mvp );
    m_debug_shader.setUniformValue( u_debug_color_fill,     static_cast<float>(fill.redF()),
                                                            static_cast<float>(fill.greenF()),
                                                            static_cast<float>(fill.blueF()),
                                                            static_cast<float>(fill.alphaF()));
    m_debug_shader.setUniformValue( u_debug_color_border,   static_cast<float>(border.redF()),
                                                            static_cast<float>(border.greenF()),
                                                            static_cast<float>(border.blueF()),
                                                            static_cast<float>(border.alphaF()));

    // ***** Draw triangles using shader program
    m_debug_shader.enableAttributeArray( a_debug_position );
    m_debug_shader.enableAttributeArray( a_debug_texture_coord );
    m_debug_shader.enableAttributeArray( a_debug_radius );
    m_debug_shader.setAttributeArray(    a_debug_position,          position,   2 );
    m_debug_shader.setAttributeArray(    a_debug_texture_coord,     uv,         2 );
    m_debug_shader.setAttributeArray(    a_debug_radius,            radius,     1 );
    glDrawArrays( GL_TRIANGLES, 0, triangles * 3 );
    m_debug_shader.disableAttributeArray( a_debug_position );
    m_debug_shader.disableAttributeArray( a_debug_texture_coord );
    m_debug_shader.disableAttributeArray( a_debug_radius );
    addTriangles( triangles );

    m_debug_shader.release();
}



//####################################################################################
//##    Draws a Circle Shape
//####################################################################################
void DrOpenGL::drawDebugCircle(DrPointF pos, float radius, float angle, DrColor fill, DrColor border) {

    // Object location
    float x =   static_cast<float>(pos.x);
    float y =   static_cast<float>(pos.y);

    // Set circle position / angle
    QMatrix4x4 model; model.translate( x,  y, 0); model.rotate(angle, 0.f, 0.f, 1.f); model.translate(-x, -y, 0);

    DebugVertex shape;
    // Triangle 1
    shape.addVertex(x, y, -1, -1, radius);
    shape.addVertex(x, y, -1,  1, radius);
    shape.addVertex(x, y,  1, -1, radius);
    // Triangle 2
    shape.addVertex(x, y, -1,  1, radius);
    shape.addVertex(x, y,  1, -1, radius);
    shape.addVertex(x, y,  1,  1, radius);

    // Draw Circle
    drawDebugShape(m_projection * m_view * model, fill, border, shape.position.data(), shape.texture_coordinates.data(), shape.radiuses.data(), 2 );

    // Draw Orientation Line
    drawDebugLine(cpv(pos.x, pos.y), cpvadd(cpv(pos.x, pos.y), cpvmult(cpvforangle(static_cast<cpFloat>(Dr::DegreesToRadians(angle+90.f))), 0.95*static_cast<double>(radius))),
                  1.5f/combinedZoomScale(), border, border);
}


//####################################################################################
//##    Draws a Segment (Line) Shape
//####################################################################################
void DrOpenGL::drawDebugLine(cpVect a, cpVect b, float radius, DrColor fill, DrColor border) {

//    cpVect t = cpvnormalize(cpvsub(b, a));

//    std::vector<float> position;                     // in sets of x, y
//    position.resize( 16 );
//    position[ 0] = static_cast<float>(a.x);      position[ 1] = static_cast<float>(a.y);
//    position[ 2] = static_cast<float>(a.x);      position[ 3] = static_cast<float>(a.y);
//    position[ 4] = static_cast<float>(a.x);      position[ 5] = static_cast<float>(a.y);
//    position[ 6] = static_cast<float>(a.x);      position[ 7] = static_cast<float>(a.y);
//    position[ 8] = static_cast<float>(b.x);      position[ 9] = static_cast<float>(b.y);
//    position[10] = static_cast<float>(b.x);      position[11] = static_cast<float>(b.y);
//    position[12] = static_cast<float>(b.x);      position[13] = static_cast<float>(b.y);
//    position[14] = static_cast<float>(b.x);      position[15] = static_cast<float>(b.y);

//    std::vector<float> texture_coordinates;          // in sets of x, y
//    texture_coordinates.clear();
//    texture_coordinates.resize( 16 );
//    texture_coordinates[ 0] = static_cast<float>(-t.x + t.y);   texture_coordinates[ 1] = static_cast<float>(-t.x - t.y);
//    texture_coordinates[ 2] = static_cast<float>(-t.x - t.y);   texture_coordinates[ 3] = static_cast<float>(+t.x - t.y);
//    texture_coordinates[ 4] = static_cast<float>(-0.0 + t.y);   texture_coordinates[ 5] = static_cast<float>(-t.x + 0.0);
//    texture_coordinates[ 6] = static_cast<float>(-0.0 - t.y);   texture_coordinates[ 7] = static_cast<float>(+t.x + 0.0);

//    texture_coordinates[ 8] = static_cast<float>(+0.0 + t.y);   texture_coordinates[ 9] = static_cast<float>(-t.x - 0.0);
//    texture_coordinates[10] = static_cast<float>(+0.0 - t.y);   texture_coordinates[11] = static_cast<float>(+t.x - 0.0);
//    texture_coordinates[12] = static_cast<float>(+t.x + t.y);   texture_coordinates[13] = static_cast<float>(-t.x + t.y);
//    texture_coordinates[14] = static_cast<float>(+t.x - t.y);   texture_coordinates[15] = static_cast<float>(+t.x + t.y);

//    std::vector<float> radiuses;                       // in sets of r
//    radiuses.resize( 8 );
//    std::fill(radiuses.begin(), radiuses.end(), radius);

//    drawDebugShape(m_projection * m_view, fill, border, position.data(), texture_coordinates.data(), radiuses.data(), 6 );

}






void DrOpenGL::drawDebugPolygon(DrPointF pos, const std::vector<cpVect> &verts, float radius, float angle, DrColor fill, DrColor border) {
    int   count =  static_cast<int>(verts.size());

//    float inset =  static_cast<float>(-cpfmax(0.0, static_cast<double>(2.f - radius)));
//    float outset = radius;
//    float r = outset - inset;

    DebugVertex shape;

    int triangles = 0;
    for (int i = 0; i < count - 2; i++) {
//        cpVect v0 = verts[i];
//        cpVect v_prev = verts[(i+(count - 1))%count];
//        cpVect v_next = verts[(i+(count + 1))%count];
//        cpVect n1 = cpvnormalize(cpvrperp(cpvsub(v0, v_prev)));
//        cpVect n2 = cpvnormalize(cpvrperp(cpvsub(v_next, v0)));
//        cpVect of = cpvmult(cpvadd(n1, n2), 1.0/(cpvdot(n1, n2) + 1.0));
//        cpVect v  = cpvadd(v0, cpvmult(of, static_cast<double>(inset)));

        shape.addVertex(verts[i].x,   verts[i].y,   0.f, 0.f, radius);
        shape.addVertex(verts[i+1].x, verts[i+1].y, 0.f, 0.f, radius);
        shape.addVertex(verts[i+2].x, verts[i+2].y, 0.f, 0.f, radius);

//		vertexes[4*i + 0] = (Vertex){{(float)v.x, (float)v.y}, {0.0f, 0.0f}, 0.0f, fill, outline};
//		vertexes[4*i + 1] = (Vertex){{(float)v.x, (float)v.y}, {(float)n1.x, (float)n1.y}, r, fill, outline};
//		vertexes[4*i + 2] = (Vertex){{(float)v.x, (float)v.y}, {(float)of.x, (float)of.y}, r, fill, outline};
//		vertexes[4*i + 3] = (Vertex){{(float)v.x, (float)v.y}, {(float)n2.x, (float)n2.y}, r, fill, outline};

        triangles++;
    }

    // Set Rotation of Shape
    float x =   static_cast<float>(pos.x);
    float y =   static_cast<float>(pos.y);
    QMatrix4x4 model; model.translate( x,  y, 0); model.rotate(angle, 0.f, 0.f, 1.f); model.translate( -x, -y, 0);

    drawDebugShape(m_projection * m_view * model, fill, border, shape.position.data(), shape.texture_coordinates.data(), shape.radiuses.data(), triangles);
}




//void ChipmunkDebugDrawFatSegment(cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
//{
//	static const Index indexes[] = {0, 1, 2, 1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7};
//	Vertex *vertexes = push_vertexes(8, indexes, 18);

//	cpVect t = cpvnormalize(cpvsub(b, a));

//	float r = (float)radius + ChipmunkDebugDrawPointLineScale;
//	RGBA8 fill = cp_to_rgba(fillColor), outline = cp_to_rgba(outlineColor);

//  vertexes[0] = (Vertex){{(float)a.x, (float)a.y}, {(float)(-t.x + t.y), (float)(-t.x - t.y)}, r, fill, outline};
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
//	for (int i = 0; i< c ount; i++){
//		cpVect v0 = verts[i];
//		cpVect v_prev = verts[(i+(count - 1))%count];
//		cpVect v_next = verts[(i+(count + 1))%count];

//		cpVect n1 = cpvnormalize(cpvrperp(cpvsub(v0, v_prev)));
//		cpVect n2 = cpvnormalize(cpvrperp(cpvsub(v_next, v0)));
//		cpVect of = cpvmult(cpvadd(n1, n2), 1.0/(cpvdot(n1, n2) + 1.0f));
//		cpVect v  = cpvadd(v0, cpvmult(of, inset));

//		vertexes[4*i + 0] = (Vertex){{(float)v.x, (float)v.y}, {0.0f, 0.0f}, 0.0f, fill, outline};
//		vertexes[4*i + 1] = (Vertex){{(float)v.x, (float)v.y}, {(float)n1.x, (float)n1.y}, r, fill, outline};
//		vertexes[4*i + 2] = (Vertex){{(float)v.x, (float)v.y}, {(float)of.x, (float)of.y}, r, fill, outline};
//		vertexes[4*i + 3] = (Vertex){{(float)v.x, (float)v.y}, {(float)n2.x, (float)n2.y}, r, fill, outline};
//	}
//}












