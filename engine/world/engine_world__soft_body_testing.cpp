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
void DoubleJoinBodies(cpSpace *space, cpBody *body1, cpBody *body2, double stiffness, Soft_Body_Shape shape, Soft_Sides side = Soft_Sides::None) {
    (void)shape;
    (void)side;

    cpVect body_a = cpBodyGetPosition(body1);
    cpVect body_b = cpBodyGetPosition(body2);
    double body_distance = cpvdist(body_a, body_b);

    double max_force(0);
    ///max_force = 10000.0 + std::pow(Dr::Clamp(stiffness, 0.0, 1.0) * 15.85, 5.0);
    max_force = 10000.0 + std::pow(Dr::Clamp(stiffness, 0.0, 1.0) * 13.79, 5.0);


    // WORKS #1, BEST:  Pivot Joint in Middle of Bodies - Used to be very stiff, improved with SetMaxForce
    cpVect middle_of_bodies = cpvsub(cpBodyGetPosition(body2), cpvmult(cpvsub(cpBodyGetPosition(body2), cpBodyGetPosition(body1)), 0.5));
    cpConstraint *pivot = cpPivotJointNew(body1, body2, middle_of_bodies);
    cpConstraintSetMaxForce(pivot, max_force);
    cpSpaceAddConstraint( space, pivot );

    // WORKS #2:        Springy Slide Joint
    ///double mul = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 5.0, 50.0);
    ///cpSpaceAddConstraint( space, cpSlideJointNew(   body1, body2, cpvzero, cpvzero, body_distance - (body_distance*0.10), body_distance) );
    ///cpSpaceAddConstraint( space, cpDampedSpringNew( body1, body2, cpvzero, cpvzero, body_distance, c_stiff*mul, c_damp*mul) );

    // WORKS #3:        Simple Pivot Around One Body
    ///cpSpaceAddConstraint(space, cpPivotJointNew(body1, body2, cpBodyGetPosition(body2)));

    // TEST:            Damped Rotary Spring
    ///cpSpaceAddConstraint( space, cpDampedRotarySpringNew(body1, body2, Dr::DegreesToRadians(0), stiff, damp) );
}

//####################################################################################
//##    Joins circle of soft bodies to circular central body
//####################################################################################
void DoubleJoinCenterBody(cpSpace *space, cpBody *center_body, cpBody *body2, cpVect center_join, double stiffness) {
    // Springy Slide Joint
    cpFloat body_distance = cpvdist(cpBodyGetPosition(center_body) + center_join, cpBodyGetPosition(body2));
    cpFloat full_distance = cpvdist(cpBodyGetPosition(center_body), cpBodyGetPosition(body2));
    double  stiff_mul = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 90.0);
    double  damp_mul =  Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 60.0);

    cpConstraint *slide_joint =   cpSlideJointNew(center_body, body2, center_join, cpvzero, body_distance * 0.0, body_distance * 1.25);
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
DrEngineObject* DrEngineWorld::addSoftBodyDouble(long texture, DrPointF point, double diameter, double stiffness, double friction, double bounce, bool can_rotate) {


    return nullptr;
}




//####################################################################################
//##    Creates Square Soft Body - ORIGINAL
//##      This implementation creates a grid of soft bodies, works but tangles a lot
//##      and also doesnt collapse well when using lower stiffness values
//####################################################################################
void DrEngineWorld::addSoftBodySquare(DrPointF point) {
    long   row_size =       12;
    double soft_ball_size = 20.0;
    double ball_spacing =   20.5;

    double friction =       0.8;
    double bounce =         0.1;
    double stiffness =      0.2;

    // Main Central Ball
    std::vector<long> center_list;
    long center_texture = Asset_Textures::Block;
    DrEngineObject *central = addBall(this, center_list, center_texture, Soft_Body_Shape::Circle, point.x, point.y,
                                      DrPointF(4.0, 4.0), DrPointF(1.0, 1.0), friction, bounce, false, true);
                    ///central->body_style = Body_Style::Square_Blob;
                    central->soft_size = DrPointF(100, 100);
                    central->height_width_ratio = 1;
    cpBody *center_ball = central->body;

    // Soft Balls
    std::vector<long> &ball_list = central->soft_balls;

    double empty_diameter = getTexture(c_key_image_empty)->width();
    double empty_scale =    soft_ball_size / empty_diameter;
    double row_width = (ball_spacing*static_cast<double>(row_size-1)) + (empty_diameter * empty_scale);

    // Grid
    for (long y = 0; y < row_size; y++) {
        for (long x = 0; x < row_size; x++) {
            double px = ball_spacing * static_cast<double>(x) + point.x - (row_width/2.0) + (empty_diameter * empty_scale / 2.0);
            double py = ball_spacing * static_cast<double>(y) + point.y - (row_width/2.0) + (empty_diameter * empty_scale / 2.0);
            DrEngineObject *soft_ball = addBall(this, ball_list, c_key_image_empty, Soft_Body_Shape::Circle, px, py,
                                                DrPointF(empty_scale, empty_scale), DrPointF(1.0, 1.0), friction, bounce, true, false);
                            soft_ball->setPhysicsParent(central);

            DrEngineObject *this_ball = findObjectByKey(ball_list[ball_list.size()-1]);
            cpFloat body_distance = cpvdist(cpBodyGetPosition(center_ball), cpBodyGetPosition(this_ball->body));
            double mul = (Dr::Clamp(((stiffness+0.25)*0.8)*10.0, 1.0, 10.0));
            cpSpaceAddConstraint(m_space, cpDampedSpringNew(center_ball, this_ball->body, cpvzero, cpvzero, body_distance, c_soft_stiff*mul, c_soft_damp*mul));
            cpSpaceAddConstraint(m_space, cpRotaryLimitJointNew(center_ball, this_ball->body, Dr::DegreesToRadians(-1), Dr::DegreesToRadians(1)));

            if (x > 0) {
                DrEngineObject *ball_1 = findObjectByKey(ball_list[ball_list.size()-1]);
                DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-2]);
                DoubleJoinBodies(m_space, ball_1->body, ball_2->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::None);
            }
            if (y > 0) {
                DrEngineObject *ball_1 = findObjectByKey(ball_list[ball_list.size()-1]);
                DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-(row_size+1)]);
                DoubleJoinBodies(m_space, ball_1->body, ball_2->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::None);

                // Rotary Limit - Stops soft balls from rotating, this helps soft ball movement and motor movement (like a wheel)
                //cpSpaceAddConstraint(m_space, cpRotaryLimitJointNew(ball_1->body, ball_2->body, Dr::DegreesToRadians(-0.1), Dr::DegreesToRadians(0.1)) );
            }
        }
    }

    // Set collision groups so that soft bodies do not collide with each other, but do other things
    applyCategoryMask(central, central->getKey());
}














