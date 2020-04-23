//
//      Created by Stephens Nunnally on 1/28/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "core/dr_random.h"
#include "core/types/dr_point.h"
#include "engine/engine_texture.h"
#include "engine/thing/engine_thing.h"
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

    // WORKS #1, BEST:  Pivot Joint in Middle of Bodies - Used to be very stiff, improved with SetMaxForce
    double max_force =        std::pow(Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 10.0, 316.0), 2.0);   // (100 to 100000)
    cpVect middle_of_bodies = cpvsub(cpBodyGetPosition(body2), cpvmult(cpvsub(cpBodyGetPosition(body2), cpBodyGetPosition(body1)), 0.5));
    cpConstraint *pivot =     cpPivotJointNew(body1, body2, middle_of_bodies);
    cpConstraintSetMaxForce(pivot, max_force);
    cpSpaceAddConstraint(space, pivot);

    // WORKS #2:        Springy Slide Joint
    double stiff_mul = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 60.0);
    double damp_mul =  Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 60.0);
    cpSpaceAddConstraint(space, cpSlideJointNew(   body1, body2, cpvzero, cpvzero, body_distance*0.5, body_distance*1.5));
    cpSpaceAddConstraint(space, cpDampedSpringNew( body1, body2, cpvzero, cpvzero, body_distance, c_soft_stiff*stiff_mul, c_soft_damp*damp_mul));


    // WORKS #3:        Simple Pivot Around One Body
    ///cpSpaceAddConstraint(space, cpPivotJointNew(body1, body2, cpBodyGetPosition(body2)));

    // TEST:            Damped Rotary Spring
    ///cpSpaceAddConstraint(space, cpDampedRotarySpringNew(body1, body2, Dr::DegreesToRadians(0), c_soft_stiff, c_soft_damp));

    // TEST:            Rotary Limit
    ///cpSpaceAddConstraint(space, cpRotaryLimitJointNew(body1, body2, Dr::DegreesToRadians(-1), Dr::DegreesToRadians(1)));
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
//##    Creates Square Soft Body Mesh - ORIGINAL
//##      This implementation creates a grid of soft bodies for a truly full soft body
//####################################################################################
DrEngineThing* DrEngineWorld::addSoftBodyMesh(long original_key, long asset_key,
                                              double pos_x, double pos_y, double pos_z, DrPointF size, DrPointF scale,
                                              double stiffness, double friction, double bounce, bool can_rotate) {
    long   min_balls =      10;
    double render_scale =   1.01;

    // SOFTNESS
    stiffness = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 0.2, 1.0);       // Percentage of 0.0 == gooey, 1.0 == stiff

    // Figure out actual diameter of texture / scaling
    double center_width =       (size.x);
    double center_height =      (size.y);
    double height_width_ratio = center_height / center_width;

    // Calculate size and location of soft balls
    double  empty_diameter =  getTexture(c_key_image_empty)->width();
    double  target_diameter;
    long    x_balls = min_balls;
    long    y_balls = min_balls;
    if (abs(center_width) < abs(center_height)) {
        target_diameter = (center_width / min_balls);
        y_balls = abs(static_cast<int>(center_height / target_diameter));
    } else {
        target_diameter = (center_height / min_balls);
        x_balls = abs(static_cast<int>(center_width  / target_diameter));
    }
    x_balls = Dr::Min(x_balls, static_cast<long>(15));
    y_balls = Dr::Min(y_balls, static_cast<long>(15));
    double empty_scale = target_diameter / empty_diameter;
    double outside_x_spacing = (center_width ) / static_cast<double>(x_balls - 1);
    double outside_y_spacing = (center_height) / static_cast<double>(y_balls - 1);
    double inside_x_spacing =  (center_width -  target_diameter) / static_cast<double>(x_balls - 1);
    double inside_y_spacing =  (center_height - target_diameter) / static_cast<double>(y_balls - 1);
    double inside_radius = target_diameter/2.0;

    // Inner Block Sizing
    DrPointF radius_multiplier(abs(target_diameter / center_width), abs(target_diameter / center_height));

    // Figure out which ball will be center soft ball
    long center_ball_x = (x_balls / 2);
    long center_ball_y = (y_balls / 2);

    // Add Soft Ball Grid
    std::vector<DrEngineThing*> balls;
    std::vector<DrPointF>       starting_positions;
    std::vector<DrPointF>       uv_coordinates;
    DrEngineThing              *central = nullptr;
    double  first_ball_angle = 0;
    long    count = 0;
    for (long y = 0; y < y_balls; y++) {
        for (long x = 0; x < x_balls; x++) {

            // Calculate Soft Ball Position
            DrPointF ball_at, outside_at;
            ball_at.x =     -(center_width/ 2.0) + inside_radius + (static_cast<double>(x) * inside_x_spacing);
            ball_at.y =     -(center_height/2.0) + inside_radius + (static_cast<double>(y) * inside_y_spacing);
            outside_at.x =  -(center_width/ 2.0) + (static_cast<double>(x) * outside_x_spacing);
            outside_at.y =  -(center_height/2.0) + (static_cast<double>(y) * outside_y_spacing);
            starting_positions.push_back(ball_at);

            // Store Starting Position, and also Converted Texture Coordinates
            DrPointF uv_coord = DrPointF((outside_at.x + center_width/2.0) / center_width, (outside_at.y + center_height/2.0) / center_height);
            uv_coordinates.push_back(uv_coord);

            // Store starting angle of first child
            if (count == 0) first_ball_angle = Dr::CalcRotationAngleInDegrees(DrPointF(0, 0), ball_at);

            // Add soft ball to world
            DrEngineThing *soft_ball;
            if (x == center_ball_x && y == center_ball_y) {
                soft_ball = addBall(this, original_key, asset_key, Soft_Body_Shape::Mesh, ball_at.x + pos_x, ball_at.y + pos_y, pos_z,
                                    scale, radius_multiplier, friction, bounce, true, can_rotate);
                central =   soft_ball;
            } else {
                soft_ball = addBall(this, c_no_key, c_key_image_empty, Soft_Body_Shape::Circle, ball_at.x + pos_x, ball_at.y + pos_y, pos_z,
                                    DrPointF(empty_scale, empty_scale), DrPointF(1.0, 1.0), friction, bounce, true, true);
            }
            balls.push_back(soft_ball);
            soft_ball->compPhysics()->body_style = Body_Style::Mesh_Blob;

            // Toward the left
            if (x > 0) {
                DrEngineThing *ball_1 = balls[balls.size()-1];
                DrEngineThing *ball_2 = balls[balls.size()-2];
                JoinBodiesMesh(m_space, ball_1->physics()->body, ball_2->physics()->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::None);
            }
            // Toward the bottom
            if (y > 0) {
                DrEngineThing *ball_1 = balls[balls.size()-1];
                DrEngineThing *ball_2 = balls[balls.size()-(x_balls+1)];
                JoinBodiesMesh(m_space, ball_1->physics()->body, ball_2->physics()->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::None);
            }
            // Diagonal left and down
            if (x > 0 && y > 0) {
                DrEngineThing *ball_1 = balls[balls.size()-1];
                DrEngineThing *ball_2 = balls[balls.size()-(x_balls+2)];
                JoinBodiesMesh(m_space, ball_1->physics()->body, ball_2->physics()->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::None);
            }

            if (x == 0         && y == 0)         soft_ball->compSoftBody()->soft_corner = true;
            if (x == 0         && y == y_balls-1) soft_ball->compSoftBody()->soft_corner = true;
            if (x == x_balls-1 && y == 0)         soft_ball->compSoftBody()->soft_corner = true;
            if (x == x_balls-1 && y == y_balls-1) soft_ball->compSoftBody()->soft_corner = true;
            count++;
        }
    }

    // Set central mass to match other soft balls
    cpBodySetMass(central->physics()->body, cpBodyGetMass(balls[0]->physics()->body));

    // Copy soft ball arrays to Central Ball
    for (size_t i = 0; i < balls.size(); ++i) {
        // Attach rotary limit joints to help fight rotation for non rotating soft bodies
        if (can_rotate == false) {
            ///if (i % 2 == 0) {    // #NOTE: Chipmunk doesnt like too many constraints on one body
                cpSpaceAddConstraint(m_space, cpRotaryLimitJointNew(balls[i]->physics()->body, central->physics()->body, 0.0, 0.0));
            ///}
        }
        // Copy ball data
        balls[i]->physics()->setPhysicsParent(central);
        central->compSoftBody()->soft_balls.push_back(balls[i]->getKey());
        central->compSoftBody()->soft_uv.push_back(   uv_coordinates[i]);
        central->compSoftBody()->soft_start.push_back(starting_positions[i]);
    }

    // Built outline ball list (for debug drawing)
    long outline_count = 0;
    long outline_loop = 0;
    long ox = 0, oy = 0;
    do {
        size_t outline_index = (oy * x_balls) + ox;
        central->compSoftBody()->soft_outline_indexes.push_back(outline_index);

        // Increment loop variables
        if (outline_loop == static_cast<long>(Soft_Sides::Bottom)) {
            ox++;   if (ox > x_balls - 1)   { outline_loop++; ox = x_balls - 1; oy = 1; }
        } else if (outline_loop == static_cast<long>(Soft_Sides::Right)) {
            oy++;   if (oy > y_balls - 1)   { outline_loop++; ox = x_balls - 2; oy = y_balls - 1; }
        } else if (outline_loop == static_cast<long>(Soft_Sides::Top)) {
            ox--;   if (ox < 0)             { outline_loop++; ox = 0;           oy = y_balls - 2; }
        } else if (outline_loop == static_cast<long>(Soft_Sides::Left)) {
            oy--;   if (oy < 1)             { outline_loop++; }
        }
        outline_count++;
    } while (outline_loop < 4);

    // Update Central Ball properties
    central->compPhysics()->setPhysicsParent(nullptr);
    central->compSoftBody()->height_width_ratio =   height_width_ratio;
    central->compSoftBody()->soft_size =            DrPointF(center_width, center_height);
    central->compSoftBody()->soft_grid_size =       DrPoint(x_balls, y_balls);
    central->compSoftBody()->soft_scale.x =         ((center_width/2.0)  / ((center_width  - target_diameter)/2.0)) * render_scale;
    central->compSoftBody()->soft_scale.y =         ((center_height/2.0) / ((center_height - target_diameter)/2.0)) * render_scale;
    central->compSoftBody()->soft_start_angle =     first_ball_angle;

    // Set collision groups so that soft bodies do not collide with each other, but do other things
    applyCategoryMask(central, central->getKey());

    // Return central soft body DrEngineObject*
    return central;
}














