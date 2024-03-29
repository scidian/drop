//
//      Created by Stephens Nunnally on 9/25/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      Useful 2D Point Class with Doubles, adapted from HullFinder
//  Note:
//      To be Engine compatible, this file does NOT, and should NOT #include any Qt Library types
//
#ifndef DR_POINT_F_H
#define DR_POINT_F_H

#include <vector>

// Forward Declarations
class DrPoint;

//####################################################################################
//##    DrPointF
//##        Useful 2D double Point Class, adapted from HullFinder
//############################
class DrPointF
{
public:
    double x;
    double y;

public:
    // Constructor
    DrPointF();
    DrPointF(double x_, double y_);
    DrPointF(const DrPoint point);

    // Operator Overloads
    DrPointF&   operator=   (const DrPointF &other);
    DrPointF    operator+   (const DrPointF &other) const;
    DrPointF    operator-   (const DrPointF &other) const;
    DrPointF    operator*   (double k) const;
    DrPointF    operator/   (double k) const;
    DrPointF&   operator*=  (double k);
    bool        operator==  (const DrPointF &other) const;

    // Functions
    double      dotProduct(const DrPointF &other) const;
    double      distanceSquared(const DrPointF &to) const;
    double      distance(const DrPointF &to) const;
    double      distance(const DrPointF &segment_start, const DrPointF &segment_end) const;
    double      decisionDistance(const std::vector<DrPointF> &points) const;

    // Conversions
    DrPoint     toPoint();
};




#endif // DR_POINT_F_H









