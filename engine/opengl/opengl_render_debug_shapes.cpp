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
//##    Vertex for Debug Shapes
//####################################################################################
struct DebugVertex {
    std::vector<float> position;                        // in sets of x, y
    std::vector<float> texture_coordinates;             // in sets of x, y
    std::vector<float> radiuses;                        // in sets of r
    std::vector<float> color_fill;                      // in sets of r, g, b, a
    std::vector<float> color_border;                    // in sets of r, g, b, a

    DebugVertex() {
        position.clear();
        texture_coordinates.clear();
        radiuses.clear();
        color_fill.clear();
        color_border.clear();
    }

    int triangleCount() { return static_cast<int>(radiuses.size() / 3); }
    int vertexCount() { return static_cast<int>(radiuses.size()); }

    void addVertex(double x, double y, float uv_x, float uv_y, float r, DrColor fill, DrColor border) {
        addVertex(static_cast<float>(x), static_cast<float>(y), uv_x, uv_y, r, fill, border);
    }

    void addVertex(float x, float y, float uv_x, float uv_y, float r, DrColor fill, DrColor border) {
        position.push_back(x);
        position.push_back(y);
        texture_coordinates.push_back(uv_x);
        texture_coordinates.push_back(uv_y);
        radiuses.push_back(r);
        color_fill.push_back(static_cast<float>(fill.redF()));
        color_fill.push_back(static_cast<float>(fill.greenF()));
        color_fill.push_back(static_cast<float>(fill.blueF()));
        color_fill.push_back(static_cast<float>(fill.alphaF()));
        color_border.push_back(static_cast<float>(border.redF()));
        color_border.push_back(static_cast<float>(border.greenF()));
        color_border.push_back(static_cast<float>(border.blueF()));
        color_border.push_back(static_cast<float>(border.alphaF()));
    }
};


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
                drawDebugCircle( DrPointF(center.x + offset.x, center.y + offset.y), radius, static_cast<float>(object->getAngle()), fill_color, fill_color );

            } else if (object->shape_type[shape] == Shape_Type::Segment) {
                cpVect a = cpSegmentShapeGetA( shape );
                cpVect b = cpSegmentShapeGetB( shape );
                float  radius = static_cast<float>(cpSegmentShapeGetRadius( shape ));
                drawDebugLine( a, b, radius, fill_color, border_color );

            } else if (object->shape_type[shape] == Shape_Type::Polygon || object->shape_type[shape] == Shape_Type::Box) {
                float radius = static_cast<float>(cpPolyShapeGetRadius(shape));
                std::vector<cpVect> vertices;

                DrPointF centroid(0, 0);
                int point_count = cpPolyShapeGetCount(shape);
                for (int i = 0; i < point_count; i++) {
                    cpVect vert = cpPolyShapeGetVert(shape, i) + cpv(center.x, center.y);
                    centroid.x += vert.x;
                    centroid.y += vert.y;
                    vertices.push_back(vert);
                }
                centroid.x /= point_count;
                centroid.y /= point_count;

                drawDebugPolygon( DrPointF(center.x, center.y), vertices, centroid, radius, static_cast<float>(object->getAngle()), fill_color, border_color );

            }   // End If

        }   // End For shape



    }   // End For object
}



//####################################################################################
//##    Draws a Debug Shape Using the Debug Shader
//####################################################################################
void DrOpenGL::drawDebugTriangles(QMatrix4x4 mvp, DebugVertex &vertexes) {
    // Enable shader program
    if (!m_debug_shader.bind()) return;

    // ***** Blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);              // Standard non-premultiplied alpha blend

    // ***** Set Shader Variables
    m_debug_shader.setUniformValue( u_debug_matrix, mvp );

    // ***** Draw triangles using shader program
    m_debug_shader.enableAttributeArray( a_debug_position );
    m_debug_shader.enableAttributeArray( a_debug_texture_coord );
    m_debug_shader.enableAttributeArray( a_debug_radius );
    m_debug_shader.enableAttributeArray( a_debug_color_fill );
    m_debug_shader.enableAttributeArray( a_debug_color_border );
    m_debug_shader.setAttributeArray(    a_debug_position,      vertexes.position.data(),               2 );
    m_debug_shader.setAttributeArray(    a_debug_texture_coord, vertexes.texture_coordinates.data(),    2 );
    m_debug_shader.setAttributeArray(    a_debug_radius,        vertexes.radiuses.data(),               1 );
    m_debug_shader.setAttributeArray(    a_debug_color_fill,    vertexes.color_fill.data(),             4 );
    m_debug_shader.setAttributeArray(    a_debug_color_border,  vertexes.color_border.data(),           4 );
    glDrawArrays( GL_TRIANGLES, 0, vertexes.vertexCount() );
    m_debug_shader.disableAttributeArray( a_debug_position );
    m_debug_shader.disableAttributeArray( a_debug_texture_coord );
    m_debug_shader.disableAttributeArray( a_debug_radius );
    m_debug_shader.disableAttributeArray( a_debug_color_fill );
    m_debug_shader.disableAttributeArray( a_debug_color_border );
    addTriangles( vertexes.triangleCount() );

    m_debug_shader.release();
}



//####################################################################################
//##    Draws a Circle Shape
//####################################################################################
void DrOpenGL::drawDebugCircle(DrPointF pos, float radius, float angle, DrColor fill, DrColor border) {

    // Object location
    float x =   static_cast<float>(pos.x);
    float y =   static_cast<float>(pos.y);

    DebugVertex shape;
    // Triangle 1
    shape.addVertex(x, y, -1, -1, radius, fill, border);
    shape.addVertex(x, y, -1,  1, radius, fill, border);
    shape.addVertex(x, y,  1, -1, radius, fill, border);
    // Triangle 2
    shape.addVertex(x, y, -1,  1, radius, fill, border);
    shape.addVertex(x, y,  1, -1, radius, fill, border);
    shape.addVertex(x, y,  1,  1, radius, fill, border);

    // Draw Circle
    drawDebugTriangles(m_projection * m_view, shape );

    // Draw Orientation Line
    drawDebugLine(cpv(pos.x, pos.y), cpvadd(cpv(pos.x, pos.y), cpvmult(cpvforangle(static_cast<cpFloat>(Dr::DegreesToRadians(angle+90.f))), 0.95*static_cast<double>(radius))),
                  1.5f/combinedZoomScale(), border, border);
}


//####################################################################################
//##    Draws a Segment (Line) Shape
//####################################################################################
void DrOpenGL::drawDebugLine(cpVect a, cpVect b, float radius, DrColor fill, DrColor border) {

    cpVect t = cpvnormalize(cpvsub(b, a));

    DebugVertex shape;
    shape.addVertex(a.x, a.y, static_cast<float>(-t.x + t.y), static_cast<float>(-t.x - t.y), radius, fill, border);
    shape.addVertex(a.x, a.y, static_cast<float>(-t.x - t.y), static_cast<float>(+t.x - t.y), radius, fill, border);
    shape.addVertex(a.x, a.y, static_cast<float>(-0.0 + t.y), static_cast<float>(-t.x + 0.0), radius, fill, border);

    shape.addVertex(a.x, a.y, static_cast<float>(-t.x - t.y), static_cast<float>(+t.x - t.y), radius, fill, border);
    shape.addVertex(a.x, a.y, static_cast<float>(-0.0 + t.y), static_cast<float>(-t.x + 0.0), radius, fill, border);
    shape.addVertex(a.x, a.y, static_cast<float>(-0.0 - t.y), static_cast<float>(+t.x + 0.0), radius, fill, border);

    shape.addVertex(a.x, a.y, static_cast<float>(-0.0 + t.y), static_cast<float>(-t.x + 0.0), radius, fill, border);
    shape.addVertex(a.x, a.y, static_cast<float>(-0.0 - t.y), static_cast<float>(+t.x + 0.0), radius, fill, border);
    shape.addVertex(b.x, b.y, static_cast<float>(+0.0 + t.y), static_cast<float>(-t.x - 0.0), radius, fill, border);

    shape.addVertex(a.x, a.y, static_cast<float>(-0.0 - t.y), static_cast<float>(+t.x + 0.0), radius, fill, border);
    shape.addVertex(b.x, b.y, static_cast<float>(+0.0 + t.y), static_cast<float>(-t.x - 0.0), radius, fill, border);
    shape.addVertex(b.x, b.y, static_cast<float>(+0.0 - t.y), static_cast<float>(+t.x - 0.0), radius, fill, border);

    shape.addVertex(b.x, b.y, static_cast<float>(+0.0 + t.y), static_cast<float>(-t.x - 0.0), radius, fill, border);
    shape.addVertex(b.x, b.y, static_cast<float>(+0.0 - t.y), static_cast<float>(+t.x - 0.0), radius, fill, border);
    shape.addVertex(b.x, b.y, static_cast<float>(+t.x + t.y), static_cast<float>(-t.x + t.y), radius, fill, border);

    shape.addVertex(b.x, b.y, static_cast<float>(+0.0 - t.y), static_cast<float>(+t.x - 0.0), radius, fill, border);
    shape.addVertex(b.x, b.y, static_cast<float>(+t.x + t.y), static_cast<float>(-t.x + t.y), radius, fill, border);
    shape.addVertex(b.x, b.y, static_cast<float>(+t.x - t.y), static_cast<float>(+t.x + t.y), radius, fill, border);

    drawDebugTriangles(m_projection * m_view, shape );
}


//####################################################################################
//##    Draws a Polygon Shape
//####################################################################################
void DrOpenGL::drawDebugPolygon(const DrPointF &position, const std::vector<cpVect> &verts, const DrPointF &centroid, float radius, float angle, DrColor fill, DrColor border) {
    int count = static_cast<int>(static_cast<int>(verts.size()));

    DebugVertex shape;
    for (int i = 0; i < count - 1; i++) {
        shape.addVertex(verts[i].x,     verts[i].y,         0.f, 0.f, radius, fill, border);
        shape.addVertex(verts[i+1].x,   verts[i+1].y,       0.f, 0.f, radius, fill, border);
        shape.addVertex(centroid.x,     centroid.y,         0.f, 0.f, 0,      fill, border);
    }

    shape.addVertex(verts[count-1].x,   verts[count-1].y,   0.f, 0.f, radius, fill, border);
    shape.addVertex(verts[0].x,         verts[0].y,         0.f, 0.f, radius, fill, border);
    shape.addVertex(centroid.x,         centroid.y,         0.f, 0.f, 0,      fill, border);

    // Set Rotation of Shape
    float x =   static_cast<float>(position.x);
    float y =   static_cast<float>(position.y);
    QMatrix4x4 model; model.translate( x,  y, 0); model.rotate(angle, 0.f, 0.f, 1.f); model.translate( -x, -y, 0);

    drawDebugTriangles(m_projection * m_view * model, shape);
}




















