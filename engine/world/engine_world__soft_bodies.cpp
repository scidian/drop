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
//##    Local Constants
//####################################################################################
const Body_Type         c_body_type =       Body_Type::Dynamic;
const double            c_add_friction =    0.25;
const double            c_add_bounce =      0.25;

const double            c_stiff =         100.0;    // Range from 100 to 1000
const double            c_damp =            1.0;    // Range from   1 to   10


//####################################################################################
//##    Adds Body to World
//####################################################################################
enum class Soft_Shape { Circle, Square, };

DrEngineObject* addBall(DrEngineWorld *world, std::vector<DrEngineObject*> &balls,
             long texture, Soft_Shape shape,
             double pos_x, double pos_y, DrPointF scale, double radius_multiplier,
             bool collides = true, bool can_rotate = true) {
    DrEngineObject *ball = new DrEngineObject(world, world->getNextKey(), c_no_key, c_body_type, texture, pos_x, pos_y, 0,
                                              scale, c_add_friction, c_add_bounce, collides, can_rotate);
    ball->setTouchDrag(true);
    ball->setTouchDragForce(5000.0);
    if (shape == Soft_Shape::Circle)        ball->addShapeCircleFromTexture(texture, radius_multiplier);
    else if (shape == Soft_Shape::Square)   ball->addShapeBoxFromTexture(texture);
    balls.push_back(ball);
    world->addThing(ball);
    return ball;
}


//####################################################################################
//##    Joins two Bodies with Springs
//####################################################################################
enum class Sides { Right, Left, Top, Bottom, None };

void joinBodies(cpSpace *space, cpBody *body1, cpBody *body2, double stiffness) {
    cpVect body_a = cpBodyGetPosition(body1);
    cpVect body_b = cpBodyGetPosition(body2);
    double body_distance = cpvdist(body_a, body_b);

    // WORKS #1, BEST:  Springy Slide Joint
    double mul = 5.0 * (Dr::Clamp(((stiffness+0.25)*0.8)*10.0, 1.0, 10.0));
    cpSpaceAddConstraint( space, cpSlideJointNew(   body1, body2, cpvzero, cpvzero, body_distance - (body_distance*0.10), body_distance) );
    cpSpaceAddConstraint( space, cpDampedSpringNew( body1, body2, cpvzero, cpvzero, body_distance, c_stiff*mul, c_damp*mul) );

    // WORKS #2:        Pivot Joint in Middle of Bodies - Very stiff
    ///cpVect middle_of_bodies = cpvsub(cpBodyGetPosition(body2), cpvmult(cpvsub(cpBodyGetPosition(body2), cpBodyGetPosition(body1)), 0.5));
    ///cpSpaceAddConstraint( space, cpPivotJointNew(body1, body2, middle_of_bodies) );

    // WORKS #3:        Simple Pivot Around One Body
    ///cpSpaceAddConstraint(space, cpPivotJointNew(body1, body2, cpBodyGetPosition(body2)));

    // WORKS #4:        Pivot Outside, Spring inside - Gets a little stuck sometimes, a lot going on
    /**
    double mul = 0.1 * (stiffness*10.0);
    double radius = c_ball_spacing / 2.0;
    cpVect middle_of_bodies = cpvsub(cpBodyGetPosition(body2), cpvmult(cpvsub(body_b, body_a), 0.5));
    if (side == Sides::Bottom) {
        cpSpaceAddConstraint( space, cpPivotJointNew(  body1, body2, cpv(middle_of_bodies.x, body_a.y - radius)) );
        cpSpaceAddConstraint( space, cpSlideJointNew(  body1, body2, cpv(      0, +radius), cpv(      0, +radius), body_distance, body_distance*1.15) );
        cpSpaceAddConstraint( space, cpDampedSpringNew(body1, body2, cpv(      0, +radius), cpv(      0, +radius), body_distance, c_stiff*mul, c_damp*mul) );
    } else if (side == Sides::Top) {
        cpSpaceAddConstraint( space, cpPivotJointNew(  body1, body2, cpv(middle_of_bodies.x, body_a.y + radius)) );
        cpSpaceAddConstraint( space, cpSlideJointNew(  body1, body2, cpv(      0, -radius), cpv(      0, -radius), body_distance, body_distance*1.15) );
        cpSpaceAddConstraint( space, cpDampedSpringNew(body1, body2, cpv(      0, -radius), cpv(      0, -radius), body_distance, c_stiff*mul, c_damp*mul) );
    } else if (side == Sides::Right) {
        cpSpaceAddConstraint( space, cpPivotJointNew(  body1, body2, cpv(body_a.x + radius, middle_of_bodies.y)) );
        cpSpaceAddConstraint( space, cpSlideJointNew(  body1, body2, cpv(-radius,       0), cpv(-radius,       0), body_distance, body_distance*1.15) );
        cpSpaceAddConstraint( space, cpDampedSpringNew(body1, body2, cpv(-radius,       0), cpv(-radius,       0), body_distance, c_stiff*mul, c_damp*mul) );
    } else if (side == Sides::Left) {
        cpSpaceAddConstraint( space, cpPivotJointNew(  body1, body2, cpv(body_a.x - radius, middle_of_bodies.y)) );
        cpSpaceAddConstraint( space, cpSlideJointNew(  body1, body2, cpv(+radius,       0), cpv(+radius,       0), body_distance, body_distance*1.15) );
        cpSpaceAddConstraint( space, cpDampedSpringNew(body1, body2, cpv(+radius,       0), cpv(+radius,       0), body_distance, c_stiff*mul, c_damp*mul) );
    }   */

    // TEST:            Damped Rotary Spring
    ///cpSpaceAddConstraint( space, cpDampedRotarySpringNew(body1, body2, Dr::DegreesToRadians(0), stiff, damp) );
}

void joinCenterBody(cpSpace *space, cpBody *center_body, cpBody *body2, cpVect center_join, double stiffness) {
    // Springy Slide Joint
    cpFloat body_distance = cpvdist(cpBodyGetPosition(center_body) + center_join, cpBodyGetPosition(body2));
    double mul = (body_distance / 100.0);
           mul = Dr::Clamp(mul, 1.5, 5.0);
           mul = mul * (Dr::Clamp(((stiffness+0.25)*0.8)*10.0, 1.0, 10.0));
    cpSpaceAddConstraint( space, cpSlideJointNew(   center_body, body2, center_join, cpvzero, body_distance * 0.0, body_distance) );

    cpFloat full_distance = cpvdist(cpBodyGetPosition(center_body), cpBodyGetPosition(body2));
    cpSpaceAddConstraint( space, cpDampedSpringNew( center_body, body2, cpvzero,     cpvzero, full_distance, c_stiff*mul, c_damp*mul) );

    // Rotary Limit
    ///cpSpaceAddConstraint( space, cpRotaryLimitJointNew(center_body, body2, Dr::DegreesToRadians(-5), Dr::DegreesToRadians(5)) );
}


//####################################################################################
//##    Creates Circle Soft Body
//##        RETURNS: Central body of soft body
//####################################################################################
DrEngineObject* DrEngineWorld::addSoftBodyCircle(long texture_number, DrPointF point, double diameter, double stiffness) {
    // Number of circles to use to make soft body
    long number_of_circles = 36;
    if (diameter < 100) number_of_circles = 24;

    // SOFTNESS
    stiffness = (Dr::Clamp(stiffness, 0.0, 1.0) + 0.25) * 0.8;              // Percentage of 0.0 == gooey, 1.0 == stiff
    double inner_size = 0.80 - ((1.0-stiffness) * 0.30);                    // Reduce size of central ball with softer objects

    // Figure out actual diameter of texture / scaling
    long   center_texture =     texture_number;
    double center_diameter =    getTexture(center_texture)->width();
    double height_width_ratio = static_cast<double>(getTexture(center_texture)->height()) / static_cast<double>(getTexture(center_texture)->width());
    double center_scale =       diameter/center_diameter;
    double center_radius =      (center_diameter * center_scale) / 2.0;

    // Main Centeral Ball
    std::vector<DrEngineObject*> center_list;
    DrEngineObject *central = addBall(this, center_list, center_texture, Soft_Shape::Circle, point.x, point.y, DrPointF(center_scale, center_scale), inner_size, true, true);
                    central->circle_soft_body = true;
                    central->soft_diameter = diameter;
                    central->height_width_ratio = height_width_ratio;
    cpBody *center_ball = center_list[0]->body;

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
    central->soft_scale = (center_radius / inside_radius) * 1.05;

    // Add Soft Balls
    std::vector<DrEngineObject*> &balls = central->soft_balls;
    for (int circle = 0; circle < number_of_circles; circle++) {
        // Figure out location
        DrPointF ball_at =      Dr::RotatePointAroundOrigin(DrPointF(inside_radius, 0), DrPointF(0, 0), (360.0/double(number_of_circles))*double(circle), false);
                 ball_at.y =    ball_at.y * height_width_ratio;
        DrPointF outside_at =   Dr::RotatePointAroundOrigin(DrPointF(center_radius, 0), DrPointF(0, 0), (360.0/double(number_of_circles))*double(circle), false);

        // Store Starting Position, and also Converted Texture Coordinates
        central->soft_start.push_back(ball_at);
        DrPointF uv_coord = DrPointF((outside_at.x + center_radius)/diameter, (outside_at.y + center_radius)/diameter);
        central->soft_uv.push_back(uv_coord);

        // Add soft ball to world
        DrEngineObject *soft_ball = addBall(this, balls, c_key_image_empty, Soft_Shape::Circle, ball_at.x + point.x, ball_at.y + point.y, DrPointF(empty_scale, empty_scale), 1.0);
                        soft_ball->setPhysicsParent(central);

        // Create joints to central body / neighbor body
        DrPointF center_join =      Dr::RotatePointAroundOrigin(DrPointF(center_radius * inner_size, 0), DrPointF(0, 0), (360.0/double(number_of_circles))*double(circle), false);
                 center_join.y =    center_join.y * height_width_ratio;
        joinCenterBody(m_space, center_ball, balls[balls.size()-1]->body, cpv(center_join.x, center_join.y), stiffness);
        if (balls.size() > 1) joinBodies(m_space, balls[balls.size()-1]->body, balls[balls.size()-2]->body, stiffness);
    }

    // Create joints from First / Last
    joinBodies(m_space, balls[0]->body, balls[balls.size()-1]->body, stiffness);

    // Set collision groups so that soft bodies do not collide with each other, but do other things
    unsigned int all_categories = ~(static_cast<unsigned int>(0));
    cpShapeFilter filter;
    filter.group =      central->getKey();      // Any int > 0, maybe use unique project id of parent? or keep a key generator when Engine starts
    filter.categories = all_categories;         // CP_ALL_CATEGORIES
    filter.mask =       all_categories;         // CP_ALL_CATEGORIES
    for (auto shape : central->shapes) cpShapeSetFilter(shape, filter);
    for (auto ball : balls) {
        for (auto shape : ball->shapes) {
            cpShapeSetFilter( shape, filter);
        }
    }

    return central;
}




//####################################################################################
//##    Creates Square Soft Body
//####################################################################################
void DrEngineWorld::addSoftBodySquare(DrPointF point) {
    (void) point;
//    std::vector<DrEngineObject*> balls;
//    long row_size = 5;
//    long x = 0;
//    long y = 0;

//    // Center support ball
//    addBall(this, balls, Asset_Textures::Ball, Soft_Shape::Circle,
//            (row_size/2 * c_ball_spacing) + point.x, (row_size/2 * c_ball_spacing) + point.y, DrPointF(0.25, 0.25), 0.75, true, true);
//    cpBody *center_ball = balls[0]->body;
//    balls.clear();


//    // Bottom
//    for (x = 0; x < row_size; x++) {
//        addBall(this, balls, Asset_Textures::Ball, Soft_Shape::Circle, (x * c_ball_spacing) + point.x, (y * c_ball_spacing) + point.y, DrPointF(0.25, 0.25), 1.0);
//        joinCenterBody(m_space, center_ball, balls[balls.size()-1]->body, cpvzero);
//        if (balls.size() > 1) joinBodies(m_space, balls[balls.size()-1]->body, balls[balls.size()-2]->body, Sides::Bottom);
//    }

//    // Right
//    x = row_size - 1;
//    for (y = 1; y < row_size; y++) {
//        addBall(this, balls, Asset_Textures::Ball, Soft_Shape::Circle, (x * c_ball_spacing) + point.x, (y * c_ball_spacing) + point.y, DrPointF(0.25, 0.25), 1.0);
//        joinCenterBody(m_space, center_ball, balls[balls.size()-1]->body, cpvzero);
//        joinBodies(m_space, balls[balls.size()-1]->body, balls[balls.size()-2]->body, Sides::Right);
//    }

//    // Top
//    y = row_size - 1;
//    for (x = row_size - 2; x >= 0; x--) {
//        addBall(this, balls, Asset_Textures::Ball, Soft_Shape::Circle, (x * c_ball_spacing) + point.x, (y * c_ball_spacing) + point.y, DrPointF(0.25, 0.25), 1.0);
//        joinCenterBody(m_space, center_ball, balls[balls.size()-1]->body, cpvzero);
//        joinBodies(m_space, balls[balls.size()-1]->body, balls[balls.size()-2]->body, Sides::Top);
//    }

//    // Left
//    x = 0;
//    for (y = row_size - 2; y > 0; y--) {
//        addBall(this, balls, Asset_Textures::Ball, Soft_Shape::Circle, (x * c_ball_spacing) + point.x, (y * c_ball_spacing) + point.y, DrPointF(0.25, 0.25), 1.0);
//        joinCenterBody(m_space, center_ball, balls[balls.size()-1]->body, cpvzero);
//        joinBodies(m_space, balls[balls.size()-1]->body, balls[balls.size()-2]->body, Sides::Left);
//    }

//    // First / Last
//    joinBodies(m_space, balls[0]->body, balls[balls.size()-1]->body, Sides::Left);
}

























