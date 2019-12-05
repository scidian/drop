//
//      Created by Stephens Nunnally on 9/30/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_SHAPE_LIST_H
#define THING_SHAPE_LIST_H

#include <QDataStream>

#include <list>

#include "common/types/pointf.h"

#include "enums_engine.h"
#include "helper.h"


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

    std::list<std::vector<DrPointF>>    m_polygons  { };            // List of polygon shapes
    std::list<DrCircle>                 m_circles   { };            // List of circle shapes

public:
    // Constructor
    DrPropertyCollision() { }

    // Shape Functions
    void        addPolygon(std::vector<DrPointF> point_list);

    // Getters / Setters
    double&                             getDirection()  { return m_direction; }
    One_Way&                            getOneWayType() { return m_one_way; }
    std::list<std::vector<DrPointF>>&   getPolygons()   { return m_polygons; }
    std::list<DrCircle>&                getCircles()    { return m_circles; }



    // Operator Overloads for file saving
    bool operator==(const DrPropertyCollision &o){
        return Dr::FuzzyCompare(o.m_direction, this->m_direction);
    }

    friend QDataStream &operator<<(QDataStream &out, const DrPropertyCollision &rhs){
        out << rhs.m_direction;
        return out;
    }
    friend QDataStream &operator>>(QDataStream &in, DrPropertyCollision &rhs){
        in >> rhs.m_direction;
        return in;
    }

};

// //############################ Must declare for use with QVariant
Q_DECLARE_METATYPE(DrPropertyCollision);


#endif // THING_SHAPE_LIST_H










