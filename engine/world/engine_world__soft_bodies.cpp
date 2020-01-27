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
const double            c_stiff =         100.0;    // Range from 100 to 1000
const double            c_damp =            1.0;    // Range from   1 to   10


//####################################################################################
//##    Adds Body to World
//####################################################################################
enum class Soft_Shape { Circle, Square, };

DrEngineObject* AddBall(DrEngineWorld *world, std::vector<long> &ball_keys, long texture, Soft_Shape shape,
                        double pos_x, double pos_y, DrPointF scale, DrPointF radius_multiplier, double friction, double bounce,
                        bool collides, bool can_rotate) {
    DrEngineObject *ball = new DrEngineObject(world, world->getNextKey(), c_no_key, Body_Type::Dynamic, texture, pos_x, pos_y, 0,
                                              scale, friction, bounce, collides, can_rotate);
    ball->setTouchDrag(true);
    ball->setTouchDragForce(5000.0);
    if (shape == Soft_Shape::Circle)        ball->addShapeCircleFromTexture(texture, radius_multiplier.x);
    else if (shape == Soft_Shape::Square)   ball->addShapeBoxFromTexture(texture, radius_multiplier);
    ball_keys.push_back(ball->getKey());
    world->addThing(ball);
    return ball;
}

void ApplyCategoryMask(DrEngineWorld *world, DrEngineObject *central) {
    // Set collision groups so that soft bodies do not collide with each other, but do other things
    unsigned int all_categories = ~(static_cast<unsigned int>(0));
    cpShapeFilter filter;
    filter.group =      central->getKey();      // Any int > 0, maybe use unique project id of parent? or keep a key generator when Engine starts
    filter.categories = all_categories;         // CP_ALL_CATEGORIES
    filter.mask =       all_categories;         // CP_ALL_CATEGORIES
    for (auto shape : central->shapes) {
        cpShapeSetFilter(shape, filter);
    }
    for (auto ball_number : central->soft_balls) {
        DrEngineObject *ball = world->findObjectByKey(ball_number);
        for (auto shape : ball->shapes) {
            cpShapeSetFilter( shape, filter);
        }
    }
}

//####################################################################################
//##    Joins two Bodies with Springs
//####################################################################################
enum class Sides { Bottom, Right, Top, Left, Circle, Square, None };

void JoinBodies(cpSpace *space, cpBody *body1, cpBody *body2, double stiffness, Sides side) {
    cpVect body_a = cpBodyGetPosition(body1);
    cpVect body_b = cpBodyGetPosition(body2);
    double body_distance = cpvdist(body_a, body_b);

    // !!!!! TEMP
    if (side == Sides::Bottom || side == Sides::Top || side == Sides::Right || side == Sides::Left) {
        side = Sides::Square;
    }

    double max_force(0);
    if (side == Sides::Circle) {
        max_force = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 10000.0,  60000.0);
    } else if (side == Sides::Square) {
        max_force = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15000.0, 100000.0);
    }

    if (side == Sides::Circle || side == Sides::Square) {
        // WORKS #1, BEST:  Springy Slide Joint
        ///double mul = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 5.0, 50.0);
        ///cpSpaceAddConstraint( space, cpSlideJointNew(   body1, body2, cpvzero, cpvzero, body_distance - (body_distance*0.10), body_distance) );
        ///cpSpaceAddConstraint( space, cpDampedSpringNew( body1, body2, cpvzero, cpvzero, body_distance, c_stiff*mul, c_damp*mul) );

        // WORKS #2:        Pivot Joint in Middle of Bodies - Used to be very stiff, improved with SetMaxForce
        cpVect middle_of_bodies = cpvsub(cpBodyGetPosition(body2), cpvmult(cpvsub(cpBodyGetPosition(body2), cpBodyGetPosition(body1)), 0.5));
        cpConstraint *pivot = cpPivotJointNew(body1, body2, middle_of_bodies);
        cpConstraintSetMaxForce(pivot, max_force);
        cpSpaceAddConstraint( space, pivot );

        // WORKS #3:        Simple Pivot Around One Body
        ///cpSpaceAddConstraint(space, cpPivotJointNew(body1, body2, cpBodyGetPosition(body2)));

    } else {
        // WORKS #4:        Pivot Outside, Spring inside - Gets a little stuck sometimes, a lot going on
        double radius = body_distance / 2.0;
        cpVect middle_of_bodies = cpvsub(cpBodyGetPosition(body2), cpvmult(cpvsub(body_b, body_a), 0.5));
        cpConstraint *hard_pivot = nullptr;
        cpConstraint *soft_pivot = nullptr;
        if (side == Sides::Bottom) {
            hard_pivot = cpPivotJointNew(  body1, body2, cpv(middle_of_bodies.x, body_a.y - radius));
            soft_pivot = cpPivotJointNew(  body1, body2, cpv(middle_of_bodies.x, body_a.y + radius));
        } else if (side == Sides::Top) {
            hard_pivot = cpPivotJointNew(  body1, body2, cpv(middle_of_bodies.x, body_a.y + radius));
            soft_pivot = cpPivotJointNew(  body1, body2, cpv(middle_of_bodies.x, body_a.y - radius));
        } else if (side == Sides::Right) {
            hard_pivot = cpPivotJointNew(  body1, body2, cpv(body_a.x + radius, middle_of_bodies.y));
            soft_pivot = cpPivotJointNew(  body1, body2, cpv(body_a.x - radius, middle_of_bodies.y));
        } else if (side == Sides::Left) {
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
void JoinCenterBody(cpSpace *space, cpBody *center_body, cpBody *body2, cpVect center_join, double stiffness) {
    // Springy Slide Joint
    cpFloat body_distance = cpvdist(cpBodyGetPosition(center_body) + center_join, cpBodyGetPosition(body2));
    cpFloat full_distance = cpvdist(cpBodyGetPosition(center_body), cpBodyGetPosition(body2));
    double  stiff_mul = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 60.0);
    double  damp_mul =  Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 15.0, 60.0);

    cpConstraint *slide_joint =   cpSlideJointNew(center_body, body2, center_join, cpvzero, body_distance * 0.0, body_distance);
    cpConstraint *damped_spring = cpDampedSpringNew(center_body, body2, cpvzero, cpvzero, full_distance, c_stiff*stiff_mul, c_damp*damp_mul);
    cpSpaceAddConstraint( space, slide_joint );
    cpSpaceAddConstraint( space, damped_spring );

    // Rotary Limit - Stops soft balls from rotating, this helps soft ball movement and motor movement (like a wheel)
    cpConstraint *rotary_joint = cpRotaryLimitJointNew(center_body, body2, Dr::DegreesToRadians(-10), Dr::DegreesToRadians(10));
    cpSpaceAddConstraint( space, rotary_joint );
}


//####################################################################################
//##
//##    SOFT CIRCLE
//##        RETURNS: Central body of soft body
//##
//####################################################################################
DrEngineObject* DrEngineWorld::addSoftBodyCircle(long texture_number, DrPointF point, double diameter,
                                                 double stiffness, double friction, double bounce, bool can_rotate) {
    // Number of circles to use to make soft body
    long   number_of_circles =  36;
    double max_radius =         0.65;
    double render_scale =       1.04;
    if (diameter < 100) { number_of_circles = 24; max_radius = 0.60; render_scale = 1.08; }
    if (diameter <  70) { number_of_circles = 16; max_radius = 0.55; render_scale = 1.16; }

    // SOFTNESS
    stiffness = Dr::Clamp(stiffness, 0.0, 1.0);                                             // Percentage of 0.0 == gooey, 1.0 == stiff
    double inner_size = max_radius - ((1.0-stiffness) * 0.20);                              // Reduce size of central ball with softer objects

    // Figure out actual diameter of texture / scaling
    long   center_texture =     texture_number;
    double center_diameter =    getTexture(center_texture)->width();
    double height_width_ratio = static_cast<double>(getTexture(center_texture)->height()) / static_cast<double>(getTexture(center_texture)->width());
    double center_scale =       diameter/center_diameter;
    double center_radius =      (center_diameter * center_scale) / 2.0;

    // Main Central Ball
    std::vector<long> center_list;
    DrEngineObject *central = AddBall(this, center_list, center_texture, Soft_Shape::Circle, point.x, point.y,
                                      DrPointF(center_scale, center_scale), DrPointF(inner_size, inner_size), friction, bounce, true, can_rotate);
                    central->body_style = Body_Style::Circular_Blob;
                    central->soft_size = DrPointF(diameter, diameter*height_width_ratio);
                    central->height_width_ratio = height_width_ratio;
    cpBody *center_ball = central->body;

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

        // Add soft ball to world
        DrEngineObject *soft_ball = AddBall(this, ball_list, c_key_image_empty, Soft_Shape::Circle, ball_at.x + point.x, ball_at.y + point.y,
                                            DrPointF(empty_scale, empty_scale), DrPointF(1.0, 1.0), friction, bounce, true, true);
                        soft_ball->setPhysicsParent(central);

        // Create joints to central body / neighbor body
        DrPointF center_join =   Dr::RotatePointAroundOrigin(DrPointF(center_radius * inner_size, 0), DrPointF(0, 0), (360.0/double(number_of_circles))*double(circle), false);
                 center_join.y = center_join.y * height_width_ratio;
        DrEngineObject *ball_1 = findObjectByKey(ball_list[ball_list.size()-1]);
        JoinCenterBody(m_space, center_ball, ball_1->body, cpv(center_join.x, center_join.y), stiffness);
        if (ball_list.size() > 1) {
            DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-2]);
            JoinBodies(m_space, ball_1->body, ball_2->body, stiffness, Sides::Circle);
        }
    }

    // Create joints from First / Last
    DrEngineObject *ball_1 = findObjectByKey(ball_list[0]);
    DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-1]);
    JoinBodies(m_space, ball_1->body, ball_2->body, stiffness, Sides::Circle);

    // Set collision groups so that soft bodies do not collide with each other, but do other things
    ApplyCategoryMask(this, central);

    // Return central soft body DrEngineObject*
    return central;
}


//####################################################################################
//##
//##    SOFT SQUARE
//##        RETURNS: Central body of soft body
//##
//####################################################################################
DrEngineObject* DrEngineWorld::addSoftBodySquare(long texture_number, DrPointF point, DrPointF scale,
                                                 double stiffness, double friction, double bounce, bool can_rotate) {
    double max_radius =     0.50;
    long   min_balls =      5;
    double render_scale =   1.02;

    // SOFTNESS
    stiffness = Dr::RangeConvert(Dr::Clamp(stiffness, 0.0, 1.0), 0.0, 1.0, 0.2, 1.0);       // Percentage of 0.0 == gooey, 1.0 == stiff
    double inner_size = max_radius - ((1.0-stiffness) * 0.15);                              // Reduce size of central ball with softer objects

    // Figure out actual diameter of texture / scaling
    long   center_texture =     texture_number;
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
    DrEngineObject *central = AddBall(this, center_list, center_texture, Soft_Shape::Square, point.x, point.y,
                                      scale, radius_multiplier, friction, bounce, true, can_rotate);
                    central->body_style = Body_Style::Square_Blob;
                    central->height_width_ratio = height_width_ratio;
                    central->soft_size = DrPointF(center_width, center_height);
    cpBody *center_ball = central->body;

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
        DrPointF ball_at, outside_at;
        bool is_corner = false;

        if (loop == static_cast<long>(Sides::Bottom)) {
            ball_at.x =     -(center_width/ 2.0) + inside_radius + (static_cast<double>(x) * inside_x_spacing);
            ball_at.y =     -(center_height/2.0) + (inside_radius);
            outside_at.x =  -(center_width/ 2.0) + (static_cast<double>(x) * outside_x_spacing);
            outside_at.y =  -(center_height/2.0);
        } else if (loop == static_cast<long>(Sides::Right)) {
            ball_at.x =      (center_width/ 2.0) - inside_radius;
            ball_at.y =     -(center_height/2.0) + inside_radius + (static_cast<double>(y) * inside_y_spacing);
            outside_at.x =   (center_width/ 2.0);
            outside_at.y =  -(center_height/2.0) + (static_cast<double>(y) * outside_y_spacing);
        } else if (loop == static_cast<long>(Sides::Top)) {
            ball_at.x =     -(center_width/ 2.0) + inside_radius + (static_cast<double>(x) * inside_x_spacing);
            ball_at.y =      (center_height/2.0) - (inside_radius);
            outside_at.x =  -(center_width/ 2.0) + (static_cast<double>(x) * outside_x_spacing);
            outside_at.y =   (center_height/2.0);
        } else if (loop == static_cast<long>(Sides::Left)) {
            ball_at.x =     -(center_width/ 2.0) + inside_radius;
            ball_at.y =     -(center_height/2.0) + inside_radius + (static_cast<double>(y) * inside_y_spacing);
            outside_at.x =  -(center_width/ 2.0);
            outside_at.y =  -(center_height/2.0) + (static_cast<double>(y) * outside_y_spacing);
        }

        // Store Starting Position, and also Converted Texture Coordinates
        DrPointF uv_coord = DrPointF((outside_at.x + center_width/2.0)/center_width, (outside_at.y + center_height/2.0)/center_height);
        central->soft_uv.push_back(uv_coord);
        central->soft_start.push_back(ball_at);

        // Store starting angle of first child
        if (count == 0) {
            central->soft_start_angle = Dr::CalcRotationAngleInDegrees(DrPointF(0, 0), ball_at);
        }

        // Add soft ball to world
        DrEngineObject *soft_ball = AddBall(this, ball_list, c_key_image_empty, Soft_Shape::Circle, ball_at.x + point.x, ball_at.y + point.y,
                                            DrPointF(empty_scale, empty_scale), DrPointF(1.0, 1.0), friction, bounce, true, true);
                        soft_ball->setPhysicsParent(central);

        // Create joints to central body / neighbor body
        DrPointF center_join = DrPointF(outside_at.x * radius_multiplier.x, outside_at.y * radius_multiplier.y);
        DrEngineObject *ball_1 = findObjectByKey(ball_list[ball_list.size()-1]);
        JoinCenterBody(m_space, center_ball, ball_1->body, cpv(center_join.x, center_join.y), stiffness);
        if (ball_list.size() > 1) {
            DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-2]);
            if      (loop == static_cast<long>(Sides::Bottom)) { JoinBodies(m_space, ball_1->body, ball_2->body, stiffness, Sides::Bottom); }
            else if (loop == static_cast<long>(Sides::Right))  { JoinBodies(m_space, ball_1->body, ball_2->body, stiffness, Sides::Right); }
            else if (loop == static_cast<long>(Sides::Top))    { JoinBodies(m_space, ball_1->body, ball_2->body, stiffness, Sides::Top); }
            else if (loop == static_cast<long>(Sides::Left))   { JoinBodies(m_space, ball_1->body, ball_2->body, stiffness, Sides::Left); }
        }

        // Increment loop variables
        if (loop == static_cast<long>(Sides::Bottom)) {
            if (x == 0 || x == x_balls-1) is_corner = true;
            x++; if (x > x_balls - 1) { loop++; x = 0; y = 0; }
        } else if (loop == static_cast<long>(Sides::Right)) {
            if (y == y_balls-1) is_corner = true;
            y++; if (y > y_balls - 1) { loop++; x = x_balls - 2; y = 0; }
        } else if (loop == static_cast<long>(Sides::Top)) {
            if (x == 0) is_corner = true;
            x--; if (x < 0) { loop++; x = 0; y = y_balls - 2; }
        } else if (loop == static_cast<long>(Sides::Left)) {
            y--; if (y < 1) { loop++; }
        }
        soft_ball->soft_corner = is_corner;

        count++;
    } while (loop < 4);

    // Create joints from First / Last
    DrEngineObject *ball_1 = findObjectByKey(ball_list[0]);
    DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-1]);
    JoinBodies(m_space, ball_1->body, ball_2->body, stiffness, Sides::Left);

    // Set collision groups so that soft bodies do not collide with each other, but do other things
    ApplyCategoryMask(this, central);

    // Return central soft body DrEngineObject*
    return central;
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
    DrEngineObject *central = AddBall(this, center_list, center_texture, Soft_Shape::Circle, point.x, point.y,
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
            DrEngineObject *soft_ball = AddBall(this, ball_list, c_key_image_empty, Soft_Shape::Circle, px, py,
                                                DrPointF(empty_scale, empty_scale), DrPointF(1.0, 1.0), friction, bounce, true, true);
                            soft_ball->setPhysicsParent(central);

            DrEngineObject *this_ball = findObjectByKey(ball_list[ball_list.size()-1]);
            cpFloat body_distance = cpvdist(cpBodyGetPosition(center_ball), cpBodyGetPosition(this_ball->body));
            double mul = (Dr::Clamp(((stiffness+0.25)*0.8)*10.0, 1.0, 10.0));
            cpSpaceAddConstraint(m_space, cpDampedSpringNew(center_ball, this_ball->body, cpvzero, cpvzero, body_distance, c_stiff*mul, c_damp*mul));
            cpSpaceAddConstraint(m_space, cpRotaryLimitJointNew(center_ball, this_ball->body, Dr::DegreesToRadians(-1), Dr::DegreesToRadians(1)));


            if (x > 0) {
                DrEngineObject *ball_1 = findObjectByKey(ball_list[ball_list.size()-1]);
                DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-2]);
                JoinBodies(m_space, ball_1->body, ball_2->body, stiffness, Sides::Square);
            }
            if (y > 0) {
                DrEngineObject *ball_1 = findObjectByKey(ball_list[ball_list.size()-1]);
                DrEngineObject *ball_2 = findObjectByKey(ball_list[ball_list.size()-(row_size+1)]);
                JoinBodies(m_space, ball_1->body, ball_2->body, stiffness, Sides::Square);

                // Rotary Limit - Stops soft balls from rotating, this helps soft ball movement and motor movement (like a wheel)
                //cpSpaceAddConstraint(m_space, cpRotaryLimitJointNew(ball_1->body, ball_2->body, Dr::DegreesToRadians(-0.1), Dr::DegreesToRadians(0.1)) );
            }
        }
    }

    // Set collision groups so that soft bodies do not collide with each other, but do other things
    ApplyCategoryMask(this, central);
}

























