//
//      Created by Stephens Nunnally on 9/30/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_SHAPE_LIST_H
#define THING_SHAPE_LIST_H

#include "core/dr_containers.h"
#include "core/dr_math.h"
#include "core/types/dr_pointf.h"

// Local Enumerations
enum class One_Way {                        // One Way Collide
    None                    = 0,
    Pass_Through            = 1,            // Objects can pass through going one_way_direction
    Weak_Spot               = 2,            // Only takes damage from one_way_direction (bustable block, turtle enemy)
    Directional_Spring      = 3,            // Repulse force is applied to one_way_direction only (instead of also taking current object direction into account)
    ///Damage_Direction     = 4,            // Only gives damage from one_way_direction
};


// Local Structs
struct DrCircle {
    double x;
    double y;
    double radius;
};


//####################################################################################
//##    DrPropertyCollision, defines a collsion shape of an object
//##        - One Way Type / Direction
//##        - Collection of Polygons and Circles
//############################
class DrPropertyCollision
{
private:
    double      m_direction = 0.0;                                  // One-Way Angle in Degrees
    One_Way     m_one_way = One_Way::None;                          // One-Way Type

    std::vector<std::vector<DrPointF>>  m_polygons  { };            // List of polygon shapes
    std::vector<DrCircle>               m_circles   { };            // List of circle shapes

public:
    // Constructor
    DrPropertyCollision() { }

    // Shape Functions
    void        addPolygon(std::vector<DrPointF> point_list);

    // Getters / Setters
    double&                             getDirection()  { return m_direction; }
    One_Way&                            getOneWayType() { return m_one_way; }
    std::vector<std::vector<DrPointF>>& getPolygons()   { return m_polygons; }
    std::vector<DrCircle>&              getCircles()    { return m_circles; }


    // Operator Overloads for file saving
    bool operator==(const DrPropertyCollision &o) {
        return Dr::FuzzyCompare(o.m_direction, this->m_direction);
    }

};


#endif // THING_SHAPE_LIST_H










