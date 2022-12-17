//
//      Created by Stephens Nunnally on 10/2/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      Useful 2D Point Class with Integers, adapted from HullFinder
//  Note:
//      To be Engine compatible, this file does NOT, and should NOT #include any Qt Library types
//
#ifndef DR_POINT_H
#define DR_POINT_H

#include <vector>

// Forward Declarations
class DrPointF;

//####################################################################################
//##    DrPoint
//##        Useful 2D int Point Class, adapted from HullFinder
//############################
class DrPoint
{
public:
    int x;
    int y;

public:
    // Constructors
    DrPoint();
    DrPoint(int x, int y_);
    DrPoint(long x, long y_);
    DrPoint(float x_, float y_);
    DrPoint(double x_, double y_);
    DrPoint(const DrPointF pointf);

    // Operator Overloads
    DrPoint&    operator=   (const DrPoint &other);
    DrPoint     operator+   (const DrPoint &other) const;
    DrPoint     operator-   (const DrPoint &other) const;
    DrPoint     operator*   (int k) const;
    DrPoint     operator/   (int k) const;
    bool        operator==  (const DrPoint &other) const;

    // Conversions
    DrPointF toPointF();
};


#endif // DR_POINT_H
