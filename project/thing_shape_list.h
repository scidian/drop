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

#include "enums_engine.h"
#include "types/pointf.h"


// Local Structs
struct DrCircle {
    double x;
    double y;
    double radius;
};


//####################################################################################
//##    DrShapeList, defines a collsion shape of an object
//##        - One Way Type / Direction
//##        - Collection of Polygons and Circles
//############################
class DrShapeList
{
private:
    double      m_direction = 0.0;                                  // One-Way Angle in Degrees
    One_Way     m_one_way = One_Way::None;                          // One-Way Type

    std::list<std::vector<DrPointF>>    m_polygons { };             // List of polygon shapes
    std::list<DrCircle>                 m_circles  { };             // List of circle shapes

public:
    DrShapeList() { }

    // Shape Functions
    void        addPolygon(std::vector<DrPointF> point_list);

    // Getters / Setters
    double&                             getDirection() { return m_direction; }
    One_Way&                            getOneWayType() { return m_one_way; }
    std::list<std::vector<DrPointF>>&   getPolygons() { return m_polygons; }
    std::list<DrCircle>&                getCircles() { return m_circles; }


    // Operator Overloads for file saving
    bool operator==(const DrShapeList & o){
        //return o.myStringVar == this->myStringVar;
        return true;
    }

    friend QDataStream &operator<<(QDataStream &out, const DrShapeList &rhs){
        //out << rhs.myStringVar;
        return out;
    }
    friend QDataStream &operator>>(QDataStream &in, DrShapeList &rhs){
        //in >> rhs.myStringVar;
        return in;
    }

};

// ########## Must declare for use with QVariant
Q_DECLARE_METATYPE(DrShapeList);


#endif // THING_SHAPE_LIST_H










