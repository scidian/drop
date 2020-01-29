//
//      Created by Stephens Nunnally on 1/23/2020, (c) 2020 Scidian Software, All Rights Reserved
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
//##    Adds Body to World
//####################################################################################
DrEngineObject* DrEngineWorld::addBall(DrEngineWorld *world, long texture, Soft_Body_Shape shape,
                                       double pos_x, double pos_y, DrPointF scale, DrPointF radius_multiplier, double friction, double bounce,
                                       bool collides, bool can_rotate) {
    DrEngineObject *ball = new DrEngineObject(world, world->getNextKey(), c_no_key, Body_Type::Dynamic, texture, pos_x, pos_y, 0,
                                              scale, friction, bounce, collides, can_rotate);
    ball->setTouchDrag(true);
    ball->setTouchDragForce(5000.0);
    if (shape == Soft_Body_Shape::Circle)       ball->addShapeCircleFromTexture(texture, radius_multiplier.x);
    else if (shape == Soft_Body_Shape::Square)  ball->addShapeBoxFromTexture(   texture, radius_multiplier);
    world->addThing(ball);
    return ball;
}


//####################################################################################
//##    Joins two Bodies with Springs
//####################################################################################
void JoinBodiesBlob(cpSpace *space, cpBody *body1, cpBody *body2, double stiffness, Soft_Body_Shape shape, Soft_Sides side = Soft_Sides::None) {
    cpVect body_a = cpBodyGetPosition(body1);
    cpVect body_b = cpBodyGetPosition(body2);
    double body_distance = cpvdist(body_a, body_b);

    double max_force(0);
    if (shape == Soft_Body_Shape::Circle) {
        ///max_force = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 10000.0, 1010000.0);
        max_force = 10000.0 + std::pow(Dr::Clamp(stiffness, 0.0, 1.0) * 15.85, 5.0);
    } else if (shape == Soft_Body_Shape::Square) {
        ///max_force = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 10000.0,  500000.0);
        max_force = 10000.0 + std::pow(Dr::Clamp(stiffness, 0.0, 1.0) * 13.79, 5.0);
    }

    if (shape == Soft_Body_Shape::Circle || shape == Soft_Body_Shape::Square) {
        // WORKS #1, BEST:  Pivot Joint in Middle of Bodies - Used to be very stiff, improved with SetMaxForce
        cpVect middle_of_bodies = cpvsub(cpBodyGetPosition(body2), cpvmult(cpvsub(cpBodyGetPosition(body2), cpBodyGetPosition(body1)), 0.5));
        cpConstraint *pivot = cpPivotJointNew(body1, body2, middle_of_bodies);
        cpConstraintSetMaxForce(pivot, max_force);
        cpSpaceAddConstraint( space, pivot );

        // WORKS #2:        Springy Slide Joint
        ///double mul = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 60.0);
        ///cpSpaceAddConstraint( space, cpSlideJointNew(   body1, body2, cpvzero, cpvzero, body_distance*0.9, body_distance*1.1) );
        ///cpSpaceAddConstraint( space, cpDampedSpringNew( body1, body2, cpvzero, cpvzero, body_distance, c_soft_stiff*mul, c_soft_damp*mul) );

        // WORKS #3:        Simple Pivot Around One Body
        ///cpSpaceAddConstraint(space, cpPivotJointNew(body1, body2, cpBodyGetPosition(body2)));

    } else {
        // WORKS #4:        Pivot Outside, Spring inside - Gets a little stuck sometimes, a lot going on
        double radius = body_distance / 2.0;
        cpVect middle_of_bodies = cpvsub(cpBodyGetPosition(body2), cpvmult(cpvsub(body_b, body_a), 0.5));
        cpConstraint *hard_pivot = nullptr;
        cpConstraint *soft_pivot = nullptr;
        if (side == Soft_Sides::Bottom) {
            hard_pivot = cpPivotJointNew(  body1, body2, cpv(middle_of_bodies.x, body_a.y - radius));
            soft_pivot = cpPivotJointNew(  body1, body2, cpv(middle_of_bodies.x, body_a.y + radius));
        } else if (side == Soft_Sides::Top) {
            hard_pivot = cpPivotJointNew(  body1, body2, cpv(middle_of_bodies.x, body_a.y + radius));
            soft_pivot = cpPivotJointNew(  body1, body2, cpv(middle_of_bodies.x, body_a.y - radius));
        } else if (side == Soft_Sides::Right) {
            hard_pivot = cpPivotJointNew(  body1, body2, cpv(body_a.x + radius, middle_of_bodies.y));
            soft_pivot = cpPivotJointNew(  body1, body2, cpv(body_a.x - radius, middle_of_bodies.y));
        } else if (side == Soft_Sides::Left) {
            hard_pivot = cpPivotJointNew(  body1, body2, cpv(body_a.x - radius, middle_of_bodies.y));
            soft_pivot = cpPivotJointNew(  body1, body2, cpv(body_a.x + radius, middle_of_bodies.y));
        }
        if (hard_pivot != nullptr) {
            cpConstraintSetMaxForce(hard_pivot, max_force );
            cpConstraintSetMaxForce(soft_pivot, max_force*0.25);
            cpSpaceAddConstraint( space, hard_pivot );
            cpSpaceAddConstraint( space, soft_pivot );
        }
    }

    // TEST:            Damped Rotary Spring
    ///cpSpaceAddConstraint( space, cpDampedRotarySpringNew(body1, body2, Dr::DegreesToRadians(0), stiff, damp) );
}


//####################################################################################
//##    Joins circle of soft bodies to circular central body
//####################################################################################
void JoinCenterBodyBlob(cpSpace *space, DrEngineObject *center, cpBody *body2, cpVect center_join, double stiffness) {
    // Springy Slide Joint
    cpFloat body_distance = cpvdist(cpBodyGetPosition(center->body) + center_join, cpBodyGetPosition(body2));
    cpFloat full_distance = cpvdist(cpBodyGetPosition(center->body), cpBodyGetPosition(body2));
    double  stiff_mul = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 90.0);
    double  damp_mul =  Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 60.0);

    cpConstraint *slide_joint =     cpSlideJointNew(  center->body, body2, center_join, cpvzero, body_distance * 0.0, body_distance * 1.25);
    cpConstraint *damped_spring_1 = cpDampedSpringNew(center->body, body2, cpvzero,     cpvzero, full_distance, c_soft_stiff*stiff_mul, c_soft_damp*damp_mul);
    cpSpaceAddConstraint( space, slide_joint );
    cpSpaceAddConstraint( space, damped_spring_1 );

    // Rotary Limit - Stops soft balls from rotating, this helps soft ball movement and motor movement (like a wheel)
    double min_angle = Dr::DegreesToRadians(-0);///-10);
    double max_angle = Dr::DegreesToRadians( 0);/// 10);
    cpConstraint *rotary_joint = cpRotaryLimitJointNew(center->body, body2, min_angle, max_angle);
    cpSpaceAddConstraint( space, rotary_joint );
}


//####################################################################################
//##
//##    SOFT CIRCLE
//##        RETURNS: Central body of soft body
//##
//####################################################################################
DrEngineObject* DrEngineWorld::addSoftBodyCircle(long texture, DrPointF point, double diameter,
                                                 double stiffness, double friction, double bounce, bool can_rotate) {
    // Number of circles to use to make soft body
    long   number_of_circles =  36;
    double max_radius =         0.70;
    double render_scale =       1.03;
    if (diameter < 100) { number_of_circles = 24; render_scale = 1.06; }
    if (diameter <  70) { number_of_circles = 16; render_scale = 1.12; }

    // SOFTNESS
    stiffness = Dr::Clamp(stiffness, 0.0, 1.0);                                             // Percentage of 0.0 == gooey, 1.0 == stiff
    double inner_size = max_radius - ((1.0-stiffness) * 0.25);                              // Reduce size of central ball with softer objects

    // Figure out actual diameter of texture / scaling
    long   center_texture =     texture;
    double center_diameter =    getTexture(center_texture)->width();
    double height_width_ratio = static_cast<double>(getTexture(center_texture)->height()) / static_cast<double>(getTexture(center_texture)->width());
    double center_scale =       diameter/center_diameter;
    double center_radius =      (center_diameter * center_scale) / 2.0;

    // Main Central Ball
    DrEngineObject *central = addBall(this, center_texture, Soft_Body_Shape::Circle, point.x, point.y,
                                      DrPointF(center_scale, center_scale), DrPointF(inner_size, inner_size), friction, bounce, true, can_rotate);
                    central->body_style = Body_Style::Circular_Blob;
                    central->soft_size = DrPointF(diameter, diameter*height_width_ratio);
                    central->height_width_ratio = height_width_ratio;

    // Calculate size and location of soft balls
    double empty_diameter = getTexture(c_key_image_empty)->width();
    double inside_radius = center_radius;
    double circumference, target_size, empty_scale;
    do {
        inside_radius *= 0.99;
        circumference = 2.0 * DR_PI * inside_radius;
        target_size = (circumference) / static_cast<double>(number_of_circles);
        empty_scale = target_size / empty_diameter;
    } while (inside_radius + (target_size/2.0) > center_radius);
    central->soft_scale.x = (center_radius / inside_radius) * render_scale;
    central->soft_scale.y = central->soft_scale.x;

    // Add Soft Balls
    std::vector<long> &ball_list = central->soft_balls;
    for (int circle = 0; circle < number_of_circles; circle++) {
        // Figure out location
        DrPointF ball_at =      Dr::RotatePointAroundOrigin(DrPointF(inside_radius, 0), DrPointF(0, 0), (360.0/double(number_of_circles))*double(circle), false);
                 ball_at.y =    ball_at.y * height_width_ratio;
        DrPointF outside_at =   Dr::RotatePointAroundOrigin(DrPointF(center_radius, 0), DrPointF(0, 0), (360.0/double(number_of_circles))*double(circle), false);

        // Store Starting Position, and also Converted Texture Coordinates
        DrPointF uv_coord = DrPointF((outside_at.x + center_radius)/diameter, (outside_at.y + center_radius)/diameter);
        central->soft_uv.push_back(uv_coord);
        central->soft_start.push_back(ball_at);

        // Store starting angle of first child
        if (circle == 0) {
            central->soft_start_angle = Dr::CalcRotationAngleInDegrees(DrPointF(0, 0), ball_at);
        }

        // Add soft ball to world
        DrEngineObject *soft_ball = addBall(this, c_key_image_empty, Soft_Body_Shape::Circle, ball_at.x + point.x, ball_at.y + point.y,
                                            DrPointF(empty_scale, empty_scale), DrPointF(1.0, 1.0), friction, bounce, true, true);
                        soft_ball->setPhysicsParent(central);
        ball_list.push_back(soft_ball->getKey());

        // Create joints to central body / neighbor body
        DrPointF center_join =   Dr::RotatePointAroundOrigin(DrPointF(center_radius * inner_size, 0), DrPointF(0, 0), (360.0/double(number_of_circles))*double(circle), false);
                 center_join.y = center_join.y * height_width_ratio;
        DrEngineObject *ball_1 = findObjectByKey(ball_list[ball_list.size()-1]);
        JoinCenterBodyBlob(m_space, central, ball_1->body, cpv(center_join.x, center_join.y), stiffness);
        if (ball_list.size() > 1) {
            DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-2]);
            JoinBodiesBlob(m_space, ball_1->body, ball_2->body, stiffness, Soft_Body_Shape::Circle);
        }
    }

    // Create joints from First / Last
    DrEngineObject *ball_1 = findObjectByKey(ball_list[0]);
    DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-1]);
    JoinBodiesBlob(m_space, ball_1->body, ball_2->body, stiffness, Soft_Body_Shape::Circle);

    // Set collision groups so that soft bodies do not collide with each other, but do other things
    applyCategoryMask(central, central->getKey());

    // Return central soft body DrEngineObject*
    return central;
}


//####################################################################################
//##
//##    SOFT SQUARE
//##        RETURNS: Central body of soft body
//##
//####################################################################################
DrEngineObject* DrEngineWorld::addSoftBodySquare(long texture, DrPointF point, DrPointF scale,
                                                 double stiffness, double friction, double bounce, bool can_rotate) {
    double max_radius =     0.60;
    long   min_balls =      5;
    double render_scale =   1.015;

    // SOFTNESS
    stiffness = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 0.2, 1.0);       // Percentage of 0.0 == gooey, 1.0 == stiff
    double inner_size = max_radius - ((1.0-stiffness) * 0.25);                              // Reduce size of central ball with softer objects

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
    DrEngineObject *central = addBall(this, center_texture, Soft_Body_Shape::Square, point.x, point.y,
                                      scale, radius_multiplier, friction, bounce, true, can_rotate);
                    central->body_style = Body_Style::Square_Blob;
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

    // Add Soft Balls
    std::vector<long> &ball_list = central->soft_balls;
    long count = 0;
    long loop = 0;
    long x =    0;
    long y =    0;
    do {
        bool is_corner = false;

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
        DrEngineObject *soft_ball = addBall(this, c_key_image_empty, Soft_Body_Shape::Circle, ball_at.x + point.x, ball_at.y + point.y,
                                            DrPointF(empty_scale, empty_scale), DrPointF(1.0, 1.0), friction, bounce, true, true);
                        soft_ball->setPhysicsParent(central);
        ball_list.push_back(soft_ball->getKey());

        // Create joints to central body / neighbor body
        DrPointF center_join = DrPointF(outside_at.x * radius_multiplier.x, outside_at.y * radius_multiplier.y);
        DrEngineObject *ball_1 = findObjectByKey(ball_list[ball_list.size()-1]);
        JoinCenterBodyBlob(m_space, central, ball_1->body, cpv(center_join.x, center_join.y), stiffness);
        if (ball_list.size() > 1) {
            DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-2]);
            if      (loop == static_cast<long>(Soft_Sides::Bottom)) {
                JoinBodiesBlob(m_space, ball_1->body, ball_2->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::Bottom); }
            else if (loop == static_cast<long>(Soft_Sides::Right))  {
                JoinBodiesBlob(m_space, ball_1->body, ball_2->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::Right); }
            else if (loop == static_cast<long>(Soft_Sides::Top))    {
                JoinBodiesBlob(m_space, ball_1->body, ball_2->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::Top); }
            else if (loop == static_cast<long>(Soft_Sides::Left))   {
                JoinBodiesBlob(m_space, ball_1->body, ball_2->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::Left); }
        }

        // Increment loop variables
        if (loop == static_cast<long>(Soft_Sides::Bottom)) {
            if (x == 0 || x == x_balls-1) is_corner = true;
            x++;
            if (x > x_balls - 1)   { loop++; x = x_balls - 1;   y = 1; }
        } else if (loop == static_cast<long>(Soft_Sides::Right)) {
            if (y == y_balls-1) is_corner = true;
            y++;
            if (y > y_balls - 1)   { loop++; x = x_balls - 2;   y = y_balls - 1; }
        } else if (loop == static_cast<long>(Soft_Sides::Top)) {
            if (x == 0) is_corner = true;
            x--;
            if (x < 0)             { loop++; x = 0;             y = y_balls - 2; }
        } else if (loop == static_cast<long>(Soft_Sides::Left)) {
            y--;
            if (y < 1) { loop++; }
        }
        soft_ball->soft_corner = is_corner;

        count++;
    } while (loop < 4);

    // Create joints from First / Last
    DrEngineObject *ball_1 = findObjectByKey(ball_list[0]);
    DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-1]);
    JoinBodiesBlob(m_space, ball_1->body, ball_2->body, stiffness, Soft_Body_Shape::Square, Soft_Sides::Left);

    // Set collision groups so that soft bodies do not collide with each other, but do other things
    applyCategoryMask(central, central->getKey());

    // Return central soft body DrEngineObject*
    return central;
}




















