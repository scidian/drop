//
//      Created by Stephens Nunnally on 12/23/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "engine/debug_flags.h"
#include "engine/engine.h"
#include "engine/mesh/engine_vertex_debug.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/components/thing_comp_soft_body.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Draws the Collision Shapes using Shaders
//####################################################################################
void DrOpenGL::drawDebugShapes() {

    // ********** Go through Things and add triangles for Shapes
    DebugVertex vertexes;
    for (auto thing : m_engine->getCurrentWorld()->getThings()) {
        ThingCompPhysics *physics = thing->physics();
        if (physics == nullptr) continue;
        if (physics->getCollidesWith() == Collision_Groups::None) continue;

        // ***** Figure out what color to make the debug shapes
        DrColor color = objectDebugColor(physics->getCollisionType(), cpBodyIsSleeping(physics->body));
        if (physics->isDying() || !physics->isAlive()) color = Dr::gray;
        if (!physics->doesCollide()) color = color.lighter();
        DrColor border_color =  color;
        DrColor fill_color =    color;
                fill_color.setAlphaF(0.4);

        // ***** Load Object Position
        DrPointF center = thing->getPosition();

        // ***** Soft Body Shapes
        bool add_shapes = false;
        if (Dr::CheckDebugFlag(Debug_Flags::Render_Soft_Body_Shapes) == false) {
            if (physics->isPhysicsChild()) continue;
            if (physics->body_style != Body_Style::Rigid_Body) {
                if (thing->compSoftBody() == nullptr) continue;
                std::vector<cpVect> vertices;
                DrPointF centroid(0, 0);
                size_t   point_count = thing->compSoftBody()->soft_outline_indexes.size();
                if (point_count == 0) continue;
                for (size_t i = 0; i < point_count; ++i) {
                    DrEngineThing *soft_ball = thing->compSoftBody()->soft_balls[thing->compSoftBody()->soft_outline_indexes[i]];
                    if (soft_ball == nullptr) continue;

                    cpVect vert = cpv(soft_ball->compSoftBody()->soft_position.x, soft_ball->compSoftBody()->soft_position.y) + cpv(center.x, center.y);
                    centroid.x += vert.x;
                    centroid.y += vert.y;
                    DrPointF mapped = Dr::RotatePointAroundOrigin(DrPointF(vert.x, vert.y), center, thing->getAngle(), false);
                    vertices.push_back(cpv(mapped.x, mapped.y));
                }
                centroid.x /= static_cast<double>(point_count);
                centroid.y /= static_cast<double>(point_count);
                centroid = Dr::RotatePointAroundOrigin(DrPointF(centroid.x, centroid.y), center, thing->getAngle(), false);

                addDebugPolygon( vertexes, vertices, centroid, 0.0, fill_color, border_color, true );
            } else {
                add_shapes = true;
            }
        } else {
            add_shapes = true;
        }

        // ***** Add Objects' Shapes
        if (add_shapes) {
            for (auto shape : physics->shapes) {
                if (physics->shape_type[shape] == Shape_Type::Circle) {
                    cpVect offset = cpCircleShapeGetOffset(shape);
                    float  radius = static_cast<float>(cpCircleShapeGetRadius(shape));
                    addDebugCircle( vertexes, DrPointF(center.x + offset.x, center.y + offset.y), radius, static_cast<float>(thing->getAngle()), fill_color, fill_color );

                } else if (physics->shape_type[shape] == Shape_Type::Segment) {
                    cpVect a = cpSegmentShapeGetA( shape );
                    cpVect b = cpSegmentShapeGetB( shape );
                    float  radius = static_cast<float>(cpSegmentShapeGetRadius( shape ));
                    addDebugLine( vertexes, a, b, radius, fill_color, border_color );

                } else if ((physics->shape_type[shape] == Shape_Type::Polygon || physics->shape_type[shape] == Shape_Type::Box)) {
                    float radius = static_cast<float>(cpPolyShapeGetRadius(shape));
                    std::vector<cpVect> vertices;

                    DrPointF centroid(0, 0);
                    int point_count = cpPolyShapeGetCount(shape);
                    for (int i = 0; i < point_count; i++) {
                        cpVect vert = cpPolyShapeGetVert(shape, i) + cpv(center.x, center.y);
                        centroid.x += vert.x;
                        centroid.y += vert.y;
                        DrPointF mapped = Dr::RotatePointAroundOrigin(DrPointF(vert.x, vert.y), center, thing->getAngle(), false);
                        vertices.push_back(cpv(mapped.x, mapped.y));
                    }
                    centroid.x /= point_count;
                    centroid.y /= point_count;
                    centroid = Dr::RotatePointAroundOrigin(DrPointF(centroid.x, centroid.y), center, thing->getAngle(), false);

                    addDebugPolygon( vertexes, vertices, centroid, radius, fill_color, border_color, physics->polygons.empty() );

                }   // End If
            }   // End For shape

            // ***** Add concave polygon outlines (if object has polygons (non-box polygons), their outlines weren't drawn in previous step)
            if (physics->polygons.empty() == false) {
                for (auto poly : physics->polygons) {
                    // Map points
                    std::vector<cpVect> vertices;
                    for (int i = 0; i < static_cast<int>(poly.size()); i++) {
                        cpVect vert = poly[i] + cpv(center.x, center.y);
                        DrPointF mapped = Dr::RotatePointAroundOrigin(DrPointF(vert.x, vert.y), center, thing->getAngle(), false);
                        vertices.push_back(cpv(mapped.x, mapped.y));
                    }
                    // Add lines
                    int count = static_cast<int>(vertices.size());
                    for (int i = 0; i < count - 1; i++) {
                        addDebugLine(vertexes, vertices[i], vertices[i+1], 1.5f/combinedZoomScale(), border_color, border_color);
                    }
                    addDebugLine(vertexes, vertices[count-1], vertices[0], 1.5f/combinedZoomScale(), border_color, border_color);
                }
            }
        }   // End if add_shapes
    }   // End For object

    // ***** Draw Debug Triangles
    drawDebugTriangles(m_projection * m_view, vertexes);
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
//##    Adds a Circle Shape to "vertexes"
//####################################################################################
void DrOpenGL::addDebugCircle(DebugVertex &vertexes, DrPointF pos, float radius, float angle, DrColor fill, DrColor border, bool draw_angle) {
    // Object location
    float x =   static_cast<float>(pos.x);
    float y =   static_cast<float>(pos.y);

    // Triangle 1
    vertexes.addVertex(x, y, -1, -1, radius, fill, border);
    vertexes.addVertex(x, y, -1,  1, radius, fill, border);
    vertexes.addVertex(x, y,  1, -1, radius, fill, border);
    // Triangle 2
    vertexes.addVertex(x, y, -1,  1, radius, fill, border);
    vertexes.addVertex(x, y,  1, -1, radius, fill, border);
    vertexes.addVertex(x, y,  1,  1, radius, fill, border);

    // Draw Orientation Line
    if (draw_angle) {
        addDebugLine(vertexes, cpv(pos.x, pos.y),
                     cpvadd(cpv(pos.x, pos.y), cpvmult(cpvforangle(static_cast<cpFloat>(Dr::DegreesToRadians(angle+90.f))), 0.98*static_cast<double>(radius))),
                     1.5f/combinedZoomScale(), border, border);
    }
}


//####################################################################################
//##    Draws a Segment (Line) Shape
//####################################################################################
void DrOpenGL::addDebugLine(DebugVertex &vertexes, cpVect a, cpVect b, float radius, DrColor fill, DrColor border) {
    cpVect t = cpvnormalize(cpvsub(b, a));

    vertexes.addVertex(a.x, a.y, static_cast<float>(-t.x + t.y), static_cast<float>(-t.x - t.y), radius, fill, border);
    vertexes.addVertex(a.x, a.y, static_cast<float>(-t.x - t.y), static_cast<float>(+t.x - t.y), radius, fill, border);
    vertexes.addVertex(a.x, a.y, static_cast<float>(-0.0 + t.y), static_cast<float>(-t.x + 0.0), radius, fill, border);

    vertexes.addVertex(a.x, a.y, static_cast<float>(-t.x - t.y), static_cast<float>(+t.x - t.y), radius, fill, border);
    vertexes.addVertex(a.x, a.y, static_cast<float>(-0.0 + t.y), static_cast<float>(-t.x + 0.0), radius, fill, border);
    vertexes.addVertex(a.x, a.y, static_cast<float>(-0.0 - t.y), static_cast<float>(+t.x + 0.0), radius, fill, border);

    vertexes.addVertex(a.x, a.y, static_cast<float>(-0.0 + t.y), static_cast<float>(-t.x + 0.0), radius, fill, border);
    vertexes.addVertex(a.x, a.y, static_cast<float>(-0.0 - t.y), static_cast<float>(+t.x + 0.0), radius, fill, border);
    vertexes.addVertex(b.x, b.y, static_cast<float>(+0.0 + t.y), static_cast<float>(-t.x - 0.0), radius, fill, border);

    vertexes.addVertex(a.x, a.y, static_cast<float>(-0.0 - t.y), static_cast<float>(+t.x + 0.0), radius, fill, border);
    vertexes.addVertex(b.x, b.y, static_cast<float>(+0.0 + t.y), static_cast<float>(-t.x - 0.0), radius, fill, border);
    vertexes.addVertex(b.x, b.y, static_cast<float>(+0.0 - t.y), static_cast<float>(+t.x - 0.0), radius, fill, border);

    vertexes.addVertex(b.x, b.y, static_cast<float>(+0.0 + t.y), static_cast<float>(-t.x - 0.0), radius, fill, border);
    vertexes.addVertex(b.x, b.y, static_cast<float>(+0.0 - t.y), static_cast<float>(+t.x - 0.0), radius, fill, border);
    vertexes.addVertex(b.x, b.y, static_cast<float>(+t.x + t.y), static_cast<float>(-t.x + t.y), radius, fill, border);

    vertexes.addVertex(b.x, b.y, static_cast<float>(+0.0 - t.y), static_cast<float>(+t.x - 0.0), radius, fill, border);
    vertexes.addVertex(b.x, b.y, static_cast<float>(+t.x + t.y), static_cast<float>(-t.x + t.y), radius, fill, border);
    vertexes.addVertex(b.x, b.y, static_cast<float>(+t.x - t.y), static_cast<float>(+t.x + t.y), radius, fill, border);
}


//####################################################################################
//##    Draws a Polygon Shape
//####################################################################################
void DrOpenGL::addDebugPolygon(DebugVertex &vertexes, const std::vector<cpVect> &verts, const DrPointF &centroid,
                               float radius, DrColor fill, DrColor border, bool add_outlines) {
    int count = static_cast<int>(static_cast<int>(verts.size()));

    for (int i = 0; i < count - 1; i++) {
        vertexes.addVertex(verts[i].x,     verts[i].y,         0.f, 0.f, radius, fill, border);
        vertexes.addVertex(verts[i+1].x,   verts[i+1].y,       0.f, 0.f, radius, fill, border);
        vertexes.addVertex(centroid.x,     centroid.y,         0.f, 0.f, 0,      fill, border);
        if (add_outlines) addDebugLine(vertexes, verts[i], verts[i+1], 1.5f/combinedZoomScale(), border, border);
    }

    vertexes.addVertex(verts[count-1].x,   verts[count-1].y,   0.f, 0.f, radius, fill, border);
    vertexes.addVertex(verts[0].x,         verts[0].y,         0.f, 0.f, radius, fill, border);
    vertexes.addVertex(centroid.x,         centroid.y,         0.f, 0.f, 0,      fill, border);
    if (add_outlines) addDebugLine(vertexes, verts[count-1], verts[0], 1.5f/combinedZoomScale(), border, border);
}




















