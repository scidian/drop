//
//      Created by Stephens Nunnally on 1/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_random.h"
#include "engine/debug_flags.h"
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "engine/mesh/engine_vertex_debug.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/entities/dr_variable.h"
#include "project/entities/dr_world.h"
#include "project/dr_project.h"


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
// Used for body iterator to get a list of all Bodies in cpSpace
static void GetSpaceBodyList(cpBody *body, std::vector<cpBody*> *body_list) {
    body_list->push_back(body);
}
// Used for constraint iterator to get a list of all Constraints (joints) in cpSpace
static void GetSpaceJointList(cpConstraint *constraint, std::vector<cpConstraint*> *joint_list) {
    joint_list->push_back(constraint);
}

//####################################################################################
//##    Main Debug / Text Rendering Function
//####################################################################################
void DrOpenGL::drawDebug() {

    // ***** Display number of cpBodies in world
    if (m_engine->getCurrentWorld()->getSpace()) {
        ///std::vector<cpBody*> body_list;
        ///body_list.clear();
        ///cpSpaceEachBody(m_engine->getCurrentWorld()->getSpace(), cpSpaceBodyIteratorFunc(GetSpaceBodyList), &body_list);
        ///g_info = "Body Count: " + std::to_string(body_list.size());
    }

    // ***** Debug Shapes
    if (m_form_engine->debug_shapes) {
        drawDebugShapes();
        drawDebugJoints();
        drawDebugHealth();
        ///paintDebugJoints(painter);
    }
    if (m_form_engine->debug_collisions) {
        drawDebugCollisions();
    }

    // ***** Debug Text
    #if !defined (Q_OS_IOS)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width() * devicePixelRatio(), height() * devicePixelRatio(), 0, -1, 1);
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

        // Get Distance
        long   world_key = getEngine()->getCurrentWorld()->getWorldKey();
        ///double distance_current = m_engine->getCurrentWorld()->getMaxDistance();
        double distance_current = getEngine()->getProject()->findWorldFromKey(world_key)->variable(Variables::Distance)->getCurrent().toDouble();
        double distance_best =    getEngine()->getProject()->findWorldFromKey(world_key)->variable(Variables::Distance)->getBest().toDouble();
        std::string distance = Dr::RoundToDecimalPlace(distance_current, 2) + ", Best: " + Dr::RoundToDecimalPlace(distance_best, 2);

        fonsDrawText(fs, 570,  20, distance.c_str(), nullptr );                                                                             // Game Distance
        fonsDrawText(fs,  90,  20, Dr::RoundToDecimalPlace(m_form_engine->fps_render, 1).c_str(), nullptr );                                // Frames per second
        fonsDrawText(fs,  90,  40, Dr::RoundToDecimalPlace(m_engine->getCurrentWorld()->getThings().size(), 0).c_str(), nullptr );          // Object count
        fonsDrawText(fs,  90,  60, Dr::RoundToDecimalPlace(getTriangleCount(), 0).c_str(), nullptr );                                       // Triangle count
        fonsDrawText(fs,  90,  80, Dr::RoundToDecimalPlace(combinedZoomScale(), 2).c_str(), nullptr );                                      // World scale
        fonsDrawText(fs,  20, 100, g_info.c_str(), nullptr);                                                                                // Global debug string
        ///fonsDrawText(fs,  20, 120, Dr::RoundToDecimalPlace(m_engine->getFormEngine()->fps_physics, 1).c_str(), nullptr );                // Physics update

        ///fonsDrawDebug(fs, 300.0, 350.0);                                                     // Draws Font Stash debug texture
    #endif


    ///int max_sample = 0, max_text = 0, max_number_textures = 0, max_layers = 0, depthBits = 0;
    ///glGetIntegerv ( GL_MAX_SAMPLES, &max_sample );                                       // Finds max multi sampling available on system
    ///glGetIntegerv ( GL_MAX_TEXTURE_SIZE, &max_text );                                    // Finds max texture size available on system
    ///glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_number_textures );         // Finds max number of textures can bind at one time
    ///glGetIntegerv ( GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers );
    ///glGetIntegerv( GL_DEPTH_BITS, &depthBits);                                           // Finds bit depth of z buffer
    ///fonsDrawText(fs, 20, 140, std::string("Depth Bits: " +  std::to_string(depthBits)).c_str(), nullptr );
}


//####################################################################################
//##    Assigns Debug color based on Collision Type
//####################################################################################
DrColor DrOpenGL::objectDebugColor(Collision_Type collision_type, bool sleeping) {
    DrColor color;

    switch (collision_type) {
        case Collision_Type::Damage_None:       color = DrColor(  0, 255,   0);     break;          // Green
        case Collision_Type::Damage_Player:     color = DrColor(255,   0,   0);     break;          // Red
        case Collision_Type::Damage_Enemy:      color = DrColor(  0,   0, 255);     break;          // Blue
        case Collision_Type::Damage_All:        color = DrColor(128,   0, 128);     break;          // Purple
    }

    if (sleeping) {
        color = Dr::yellow;
    }

    return color;
}


//####################################################################################
//##    Draws the active Constraints using Debug Shader
//####################################################################################
void DrOpenGL::drawDebugJoints() {
    // Get a list of the constraints in the Space
    std::vector<cpConstraint*> joint_list;
    joint_list.clear();
    cpSpaceEachConstraint(m_engine->getCurrentWorld()->getSpace(), cpSpaceConstraintIteratorFunc(GetSpaceJointList), &joint_list);

    // ***** Build mesh of joints
    DebugVertex vertexes;
    for (auto joint : joint_list) {
        cpVect point_a, point_b, pivot_point;
        bool   has_pivot = false;

        // Find Joint Coordinates        
        cpBody *body_a = cpConstraintGetBodyA(joint);
        cpBody *body_b = cpConstraintGetBodyB(joint);

        // Dont draw joints for soft bodies unless debug flag is on
        if (body_a != m_engine->mouse_body && body_b != m_engine->mouse_body) {
            DrEngineThing *object_a = static_cast<DrEngineThing*>(cpBodyGetUserData(body_a));
            DrEngineThing *object_b = static_cast<DrEngineThing*>(cpBodyGetUserData(body_b));
            if (object_a != nullptr && object_b != nullptr) {
                if (object_a->physics() != nullptr && object_b->physics() != nullptr) {
                    if (Dr::CheckDebugFlag(Debug_Flags::Render_Soft_Body_Shapes) == false) {
                        if (object_a->physics()->isPhysicsChild() || object_b->physics()->isPhysicsChild()) continue;
                    }
                }
            }
        }

        // Get Joint Info
        cpVect  body_pos_a = cpBodyGetPosition(body_a);
        cpVect  body_pos_b = cpBodyGetPosition(body_b);
        if (cpConstraintIsSlideJoint(joint)) {
            cpVect    slide_a = cpSlideJointGetAnchorA(joint);
            DrPointF  rotate_a = Dr::RotatePointAroundOrigin(DrPointF(slide_a.x, slide_a.y), DrPointF(0, 0), cpBodyGetAngle(body_a), true);
            point_a = body_pos_a + cpv(rotate_a.x, rotate_a.y);

            cpVect    slide_b = cpSlideJointGetAnchorB(joint);
            DrPointF  rotate_b = Dr::RotatePointAroundOrigin(DrPointF(slide_b.x, slide_b.y), DrPointF(0, 0), cpBodyGetAngle(body_b), true);
            point_b = body_pos_b + cpv(rotate_b.x, rotate_b.y);
        } else if (cpConstraintIsPivotJoint(joint)) {
            point_a = body_pos_a;
            point_b = body_pos_b;

            cpVect    pivot = cpPivotJointGetAnchorA(joint);
            DrPointF  rotate_p = Dr::RotatePointAroundOrigin(DrPointF(pivot.x, pivot.y),     DrPointF(0, 0), cpBodyGetAngle(body_a), true);
            pivot_point = body_pos_a + cpv(rotate_p.x, rotate_p.y);
            has_pivot = true;
        } else if (cpConstraintIsDampedSpring(joint)) {
            continue;
        } else if (cpConstraintIsRotaryLimitJoint(joint)) {
            continue;
        } else {
            point_a = body_pos_a;
            point_b = body_pos_b;
        }

        // Joint dot colors
        DrColor color_a = Dr::white;
        DrColor color_b = Dr::brown;
        DrColor color_p = Dr::orange;
        color_a.setAlphaF(0.5);
        color_b.setAlphaF(0.5);
        color_p.setAlphaF(0.75);

        addDebugLine(vertexes, point_a, point_b, 1.5f/combinedZoomScale(), Dr::white, Dr::white);
        addDebugCircle(vertexes, DrPointF(point_a.x, point_a.y), 5.0f, 0.f, color_a, Dr::white, false);
        addDebugCircle(vertexes, DrPointF(point_b.x, point_b.y), 5.0f, 0.f, color_b, Dr::brown, false);
        if (has_pivot) addDebugCircle(vertexes, DrPointF(pivot_point.x, pivot_point.y), 3.0f, 0.f, color_p, Dr::orange, false);
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
        if ( thing->physics() == nullptr) continue;

        ThingCompPhysics *physics = thing->physics();
        if ( physics->body_type != Body_Type::Dynamic) continue;

        std::vector<DrPointF> point_list;   point_list.clear();
        std::vector<cpVect>   normal_list;  normal_list.clear();
        cpBodyEachArbiter(physics->body, cpBodyArbiterIteratorFunc(GetBodyContactPoints),  &point_list);
        cpBodyEachArbiter(physics->body, cpBodyArbiterIteratorFunc(GetBodyContactNormals), &normal_list);

        for (int i = 0; i < static_cast<int>(point_list.size()); i++) {
            DrPointF point_1(point_list[i].x + 2.5, point_list[i].y);/// + 2.5);
            DrPointF point_2(point_list[i].x - 2.5, point_list[i].y);/// - 2.5);

            double angle_in_radians = std::atan2(normal_list[i].y, normal_list[i].x);

            point_1 = Dr::RotatePointAroundOrigin(point_1, point_list[i], angle_in_radians, true);
            point_2 = Dr::RotatePointAroundOrigin(point_2, point_list[i], angle_in_radians, true);

            addDebugLine(vertexes, cpv(point_1.x, point_1.y), cpv(point_2.x, point_2.y), 2.5f/combinedZoomScale(), fill_color, Dr::red);

        }   // End For i
    }   // End For thing

    // ***** Draw collision triangles
    drawDebugTriangles(m_projection * m_view, vertexes);
}

















