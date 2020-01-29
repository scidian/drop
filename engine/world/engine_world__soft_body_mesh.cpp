//
//      Created by Stephens Nunnally on 1/28/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "core/dr_random.h"
#include "engine/engine_texture.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Joins two Bodies with Springs
//####################################################################################
void JoinBodiesMesh(cpSpace *space, cpBody *body1, cpBody *body2, double stiffness, Soft_Body_Shape shape, Soft_Sides side = Soft_Sides::None) {
    (void)shape;
    (void)side;

    cpVect body_a = cpBodyGetPosition(body1);
    cpVect body_b = cpBodyGetPosition(body2);
    double body_distance = cpvdist(body_a, body_b);

    double max_force(0);
    max_force = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 10000.0, 100000.0);

    // WORKS #1, BEST:  Pivot Joint in Middle of Bodies - Used to be very stiff, improved with SetMaxForce
    cpVect middle_of_bodies = cpvsub(cpBodyGetPosition(body2), cpvmult(cpvsub(cpBodyGetPosition(body2), cpBodyGetPosition(body1)), 0.5));
    cpConstraint *pivot = cpPivotJointNew(body1, body2, middle_of_bodies);
    cpConstraintSetMaxForce(pivot, max_force);
    cpSpaceAddConstraint( space, pivot );

    // WORKS #2:        Springy Slide Joint
    double mul = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 60.0);
    cpSpaceAddConstraint( space, cpSlideJointNew(   body1, body2, cpvzero, cpvzero, body_distance*0.5, body_distance*1.5));
    cpSpaceAddConstraint( space, cpDampedSpringNew( body1, body2, cpvzero, cpvzero, body_distance, c_soft_stiff*mul, c_soft_damp*mul));

    // WORKS #3:        Simple Pivot Around One Body
    ///cpSpaceAddConstraint(space, cpPivotJointNew(body1, body2, cpBodyGetPosition(body2)));

    // TEST:            Damped Rotary Spring
    ///cpSpaceAddConstraint( space, cpDampedRotarySpringNew(body1, body2, Dr::DegreesToRadians(0), stiff, damp) );
}

//####################################################################################
//##    Joins circle of soft bodies to circular central body
//####################################################################################
void JoinCenterBodyMesh(cpSpace *space, cpBody *center_body, cpBody *body2, cpVect center_join, double stiffness) {
    // Springy Slide Joint
    cpFloat body_distance = cpvdist(cpBodyGetPosition(center_body) + center_join, cpBodyGetPosition(body2));
    cpFloat full_distance = cpvdist(cpBodyGetPosition(center_body), cpBodyGetPosition(body2));
    double  stiff_mul = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 90.0);
    double  damp_mul =  Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 60.0);

    cpConstraint *slide_joint =   cpSlideJointNew(center_body, body2, center_join, cpvzero, body_distance*0.8, body_distance * 1.2);
    cpConstraint *damped_spring = cpDampedSpringNew(center_body, body2, cpvzero, cpvzero, full_distance, c_soft_stiff*stiff_mul, c_soft_damp*damp_mul);
    cpSpaceAddConstraint( space, slide_joint );
    cpSpaceAddConstraint( space, damped_spring );

    // Rotary Limit - Stops soft balls from rotating, this helps soft ball movement and motor movement (like a wheel)
    cpConstraint *rotary_joint = cpRotaryLimitJointNew(center_body, body2, Dr::DegreesToRadians(-5), Dr::DegreesToRadians(5));
    cpSpaceAddConstraint( space, rotary_joint );
}



//####################################################################################
//##
//##    SOFT CIRCLE DOUBLE JOINTED
//##        RETURNS: Central body of soft body
//##
//####################################################################################
DrEngineObject* DrEngineWorld::addSoftBodyDouble(long texture, DrPointF point, double diameter,
                                                 double stiffness, double friction, double bounce, bool can_rotate) {


    return nullptr;
}




//####################################################################################
//##    Creates Square Soft Body - ORIGINAL
//##      This implementation creates a grid of soft bodies, works but tangles a lot
//##      and also doesnt collapse well when using lower stiffness values
//####################################################################################
DrEngineObject* DrEngineWorld::addSoftBodyMesh(long texture, DrPointF point, DrPointF scale,
                                               double stiffness, double friction, double bounce, bool can_rotate) {
    long   min_balls =      6;
    double render_scale =   1.01;

    // SOFTNESS
    stiffness = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 0.2, 1.0);       // Percentage of 0.0 == gooey, 1.0 == stiff
    double inner_size =     0.9;

    // Figure out actual diameter of texture / scaling
    long   center_texture =     texture;
    double center_width =       static_cast<double>(getTexture(center_texture)->width()) *  scale.x;
    double center_height =      static_cast<double>(getTexture(center_texture)->height()) * scale.y;
    double height_width_ratio = center_height / center_width;

    // Inner Block Sizing
    DrPointF radius_multiplier(inner_size, inner_size);
    if (center_width < center_height) {
        double empty_space = center_width - (center_width * inner_size);
        double target_height = center_height - empty_space;
        radius_multiplier.y = target_height / center_height;
    } else {
        double empty_space = center_height - (center_height * inner_size);
        double target_width = center_width - empty_space;
        radius_multiplier.x = target_width / center_width;
    }

    // Main Central Ball
    std::vector<long> center_list;
    DrEngineObject *central = addBall(this, center_list, center_texture, Soft_Body_Shape::Square, point.x, point.y,
                                      scale, radius_multiplier, friction, bounce, true, can_rotate);
                    central->body_style = Body_Style::Mesh_Blob;
                    central->height_width_ratio = height_width_ratio;
                    central->soft_size = DrPointF(center_width, center_height);

    // Calculate size and location of soft balls
    double  empty_diameter =  getTexture(c_key_image_empty)->width();
    double  target_diameter;
    long    x_balls = min_balls;
    long    y_balls = min_balls;
    if (center_width < center_height) {
        target_diameter = (center_width / min_balls);
        y_balls = static_cast<int>(center_height / target_diameter);
    } else {
        target_diameter = (center_height / min_balls);
        x_balls = static_cast<int>(center_width  / target_diameter);
    }
    x_balls = Dr::Min(x_balls, static_cast<long>(15));
    y_balls = Dr::Min(y_balls, static_cast<long>(15));
    double empty_scale = target_diameter / empty_diameter;
    double outside_x_spacing = (center_width ) / static_cast<double>(x_balls - 1);
    double outside_y_spacing = (center_height) / static_cast<double>(y_balls - 1);
    double inside_x_spacing =  (center_width -  target_diameter) / static_cast<double>(x_balls - 1);
    double inside_y_spacing =  (center_height - target_diameter) / static_cast<double>(y_balls - 1);
    double inside_radius = target_diameter/2.0;
    central->soft_scale.x = ((center_width/2.0) /  ((center_width  - target_diameter)/2.0)) * render_scale;
    central->soft_scale.y = ((center_height/2.0) / ((center_height - target_diameter)/2.0)) * render_scale;


    // Add Soft Ball Grid
    std::vector<long> &ball_list = central->soft_balls;
    long count = 0;
    for (long y = 0; y < y_balls; y++) {
        for (long x = 0; x < x_balls; x++) {

            DrPointF ball_at, outside_at;
            ball_at.x =     -(center_width/ 2.0) + inside_radius + (static_cast<double>(x) * inside_x_spacing);
            ball_at.y =     -(center_height/2.0) + inside_radius + (static_cast<double>(y) * inside_y_spacing);
            outside_at.x =  -(center_width/ 2.0) + (static_cast<double>(x) * outside_x_spacing);
            outside_at.y =  -(center_height/2.0) + (static_cast<double>(y) * outside_y_spacing);

            // Store Starting Position, and also Converted Texture Coordinates
            DrPointF uv_coord = DrPointF((outside_at.x + center_width/2.0)/center_width, (outside_at.y + center_height/2.0)/center_height);
            central->soft_uv.push_back(uv_coord);
            central->soft_start.push_back(ball_at);

            // Store starting angle of first child
            if (count == 0) {
                central->soft_start_angle = Dr::CalcRotationAngleInDegrees(DrPointF(0, 0), ball_at);
            }

            // Add soft ball to world
            DrEngineObject *soft_ball = addBall(this, ball_list, c_key_image_empty, Soft_Body_Shape::Circle, ball_at.x + point.x, ball_at.y + point.y,
                                                DrPointF(empty_scale, empty_scale), DrPointF(1.0, 1.0), friction, bounce, true, true);
                            soft_ball->setPhysicsParent(central);

            if (x == 0 || y == 0 || x == x_balls-1 || y == y_balls-1) {
                DrEngineObject *this_ball = findObjectByKey(ball_list[ball_list.size()-1]);
                cpFloat body_distance = cpvdist(cpBodyGetPosition(central->body), cpBodyGetPosition(this_ball->body));
                double mul = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 1.0, 10.0);
                cpSpaceAddConstraint(m_space, cpDampedSpringNew(central->body, this_ball->body, cpvzero, cpvzero, body_distance, c_soft_stiff*mul, c_soft_damp*mul));
                cpSpaceAddConstraint(m_space, cpRotaryLimitJointNew(central->body, this_ball->body, Dr::DegreesToRadians(-10), Dr::DegreesToRadians(10)));
            }

            if (x > 0) {
                DrEngineObject *ball_1 = findObjectByKey(ball_list[ball_list.size()-1]);
                DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-2]);
                JoinBodiesMesh(m_space, ball_1->body, ball_2->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::None);
            }
            if (y > 0) {
                DrEngineObject *ball_1 = findObjectByKey(ball_list[ball_list.size()-1]);
                DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-(x_balls+1)]);
                JoinBodiesMesh(m_space, ball_1->body, ball_2->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::None);
            }

            if (x == 0         && y == 0)         soft_ball->soft_corner = true;
            if (x == 0         && y == y_balls-1) soft_ball->soft_corner = true;
            if (x == x_balls-1 && y == 0)         soft_ball->soft_corner = true;
            if (x == x_balls-1 && y == y_balls-1) soft_ball->soft_corner = true;
            count++;
        }
    }

    // Set collision groups so that soft bodies do not collide with each other, but do other things
    applyCategoryMask(central, central->getKey());

    // Return central soft body DrEngineObject*
    return central;
}














