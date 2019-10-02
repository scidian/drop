//
//      Created by Stephens Nunnally on 10/2/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Useful 2D Point Class with Integers, adapted from HullFinder
//
//
#ifndef DR_POINT_H
#define DR_POINT_H

#include <vector>

//####################################################################################
//##    DrPoint
//##        Useful 2D Point Class, adapted from HullFinder
//############################
class DrPoint
{
public:
    int x;
    int y;
    DrPoint();
    DrPoint(int x, int y_);
    DrPoint(float x_, float y_);
    DrPoint(double x_, double y_);
    DrPoint & operator=(const DrPoint &other);
    DrPoint operator+(const DrPoint &other) const;
    DrPoint operator-(const DrPoint &other) const;
    DrPoint operator*(int k) const;
    DrPoint operator/(int k) const;
    bool    operator==(const DrPoint &other) const;
};


#endif // DR_POINT_H
