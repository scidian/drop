//
//      Created by Stephens Nunnally on 1/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "engine/mesh/engine_vertex_debug.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Chipmunk Callbacks
//##        Grabs data from Space
//####################################################################################
// Used for Arbiter iterator to get a list of all arbiters (collision points) attached to a body
static void GetBodyContactPoints(cpBody *, cpArbiter *arb, std::vector<DrPointF> *point_list) {
    cpContactPointSet contact = cpArbiterGetContactPointSet( arb );
    point_list->push_back( DrPointF( contact.points->pointA.x, contact.points->pointA.y ) );
}
// Used for Arbiter iterator to get a list of Normals (angles) of all arbiters (collision points) attached to a body
static void GetBodyContactNormals(cpBody *, cpArbiter *arb, std::vector<cpVect> *normal_list) {
    cpVect normal = cpArbiterGetNormal( arb );
    normal_list->push_back( normal );
}
// Used for constraint iterator to get a list of all Constraints (joints) in cpSpace
static void GetSpaceJointList(cpConstraint *constraint, std::vector<cpConstraint*> *joint_list) {
    joint_list->push_back(constraint);
}


//####################################################################################
//##    Main Debug / Text Rendering Function
//####################################################################################
void DrOpenGL::drawDebug() {

    // ***** Debug Shapes
    if (m_form_engine->debug_shapes) {
        drawDebugShapes();
        drawDebugJoints();
        drawDebugHealth();
        ///paintDebugShapes(painter);
        ///paintDebugJoints(painter);
        ///paintDebugHealth(painter);
    }
    if (m_form_engine->debug_collisions) {
        drawDebugCollisions();
        ///paintDebugCollisions(painter);
    }

    // ***** Debug Text
    glViewport(0, 0, width(), height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width(), height(), 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    fonsDrawText(fs, 500,  20, "Distance:",     nullptr );                          // Game Distance
    fonsDrawText(fs,  20,  20, "FPS:",          nullptr );                          // Frames per second
    fonsDrawText(fs,  20,  40, "Objects:",      nullptr );                          // Object count
    fonsDrawText(fs,  20,  60, "Triangles:",    nullptr );                          // Triangle count
    fonsDrawText(fs,  20,  80, "Zoom:",         nullptr );                          // World scale
    ///fonsDrawText(fs,  20, 120, "Physics:",   nullptr );                          // Physics update

    fonsDrawText(fs, 570,  20, Dr::RoundToDecimalPlace(m_engine->getCurrentWorld()->getMaxDistance(), 2).c_str(), nullptr );            // Game Distance
    fonsDrawText(fs,  90,  20, Dr::RoundToDecimalPlace(m_form_engine->fps_render, 1).c_str(), nullptr );                                // Frames per second
    fonsDrawText(fs,  90,  40, Dr::RoundToDecimalPlace(m_engine->getCurrentWorld()->getThings().size(), 0).c_str(), nullptr );          // Object count
    fonsDrawText(fs,  90,  60, Dr::RoundToDecimalPlace(getTriangleCount(), 0).c_str(), nullptr );                                       // Triangle count
    fonsDrawText(fs,  90,  80, Dr::RoundToDecimalPlace(combinedZoomScale(), 2).c_str(), nullptr );                                      // World scale
    fonsDrawText(fs,  20, 100, g_info.c_str(), nullptr);                                                                                // Global debug string
    ///fonsDrawText(fs,  20, 120, Dr::RoundToDecimalPlace(m_engine->getFormEngine()->fps_physics, 1).c_str(), nullptr );                // Physics update

    ///fonsDrawDebug(fs, 300.0, 350.0);

    ///int max_sample = 0, max_text = 0, max_number_textures = 0, max_layers = 0;
    ///glGetIntegerv ( GL_MAX_SAMPLES, &max_sample );                                      // Finds max multi sampling available on system
    ///glGetIntegerv ( GL_MAX_TEXTURE_SIZE, &max_text );                                   // Finds max texture size available on system
    ///glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_number_textures );        // Finds max number of textures can bind at one time
    ///glGetIntegerv ( GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers );
    ///painter.drawText( QPointF(20, 80), "Max Texture Size: " +  QString::number(g_max_texture_size));
}


//####################################################################################
//##    Assigns Debug color based on Collision Type
//####################################################################################
DrColor DrOpenGL::objectDebugColor(DrEngineObject *object, bool text_color) {
    DrColor color, font_color;

    switch (object->getCollisionType()) {
        case Collision_Type::Damage_None:       color = DrColor(  0, 255,   0);     break;          // Green
        case Collision_Type::Damage_Player:     color = DrColor(255,   0,   0);     break;          // Red
        case Collision_Type::Damage_Enemy:      color = DrColor(  0,   0, 255);     break;          // Blue
        case Collision_Type::Damage_All:        color = DrColor(128,   0, 128);     break;          // Purple
    }
    font_color = color;

    if (cpBodyIsSleeping(object->body)) {
        ///font_color = color;
        color = Dr::yellow;
    } else {
        ///font_color = DrColor(255 - color.red(), 255 - color.green(), 255 - color.blue());
    }

    if (text_color) return font_color;
    else            return color;
}


//####################################################################################
//##    Draws the active Constraints using Debug Shader
//####################################################################################
void DrOpenGL::drawDebugJoints() {
    // Get a list of the constraints in the Space
    std::vector<cpConstraint*> joint_list;
    joint_list.clear();
    cpSpaceEachConstraint(m_engine->getCurrentWorld()->getSpace(), cpSpaceConstraintIteratorFunc(GetSpaceJointList), &joint_list);

    // Joint dot fill color
    DrColor fill_color = Dr::white;
    fill_color.setAlphaF(0.4);

    // ***** Build mesh of joints
    DebugVertex vertexes;
    for (auto joint : joint_list) {
        cpBody *body_a = cpConstraintGetBodyA( joint );
        cpBody *body_b = cpConstraintGetBodyB( joint );

        // Grab DrEngineObjects from cpUserData
        DrEngineObject *object_a = static_cast<DrEngineObject*>(cpBodyGetUserData(body_a));
        DrEngineObject *object_b = static_cast<DrEngineObject*>(cpBodyGetUserData(body_b));

        // Load Object Positions
        DrPointF center_a = object_a->getPosition();
        DrPointF center_b = object_b->getPosition();

        addDebugLine(vertexes, cpv(center_a.x, center_a.y), cpv(center_b.x, center_b.y), 1.5f/combinedZoomScale(), Dr::white, Dr::white);
        addDebugCircle(vertexes, center_a, 3.f, 0.f, fill_color, Dr::white, false);
        addDebugCircle(vertexes, center_b, 3.f, 0.f, fill_color, Dr::white, false);
    }

    // ***** Draw joint triangles
    drawDebugTriangles(m_projection * m_view, vertexes);
}


//####################################################################################
//##    Draws the Collision Points using Debug Shader
//####################################################################################
void DrOpenGL::drawDebugCollisions() {
    // Collision dot fill color
    DrColor fill_color = Dr::red;
    ///fill_color.setAlphaF(0.4);

    // ***** Build mesh of collisions
    DebugVertex vertexes;
    for (auto thing : m_engine->getCurrentWorld()->getThings()) {
        if ( thing->getThingType() != DrThingType::Object)  continue;

        DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
        if ( object->body_type != Body_Type::Dynamic)        continue;

        std::vector<DrPointF> point_list;   point_list.clear();
        std::vector<cpVect>   normal_list;  normal_list.clear();
        cpBodyEachArbiter(object->body, cpBodyArbiterIteratorFunc(GetBodyContactPoints),  &point_list);
        cpBodyEachArbiter(object->body, cpBodyArbiterIteratorFunc(GetBodyContactNormals), &normal_list);

        for (int i = 0; i < static_cast<int>(point_list.size()); i++) {
            DrPointF point_1(point_list[i].x, point_list[i].y + 2.5);
            DrPointF point_2(point_list[i].x, point_list[i].y - 2.5);

            double angle_in_radians = std::atan2(normal_list[i].y, normal_list[i].x);

            point_1 = Dr::RotatePointAroundOrigin(point_1, point_list[i], angle_in_radians, true);
            point_2 = Dr::RotatePointAroundOrigin(point_2, point_list[i], angle_in_radians, true);

            addDebugLine(vertexes, cpv(point_1.x, point_1.y), cpv(point_2.x, point_2.y), 2.5f/combinedZoomScale(), fill_color, Dr::red);

        }   // End For i
    }   // End For thing

    // ***** Draw collision triangles
    drawDebugTriangles(m_projection * m_view, vertexes);
}

















