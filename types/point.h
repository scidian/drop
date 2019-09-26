//
//      Created by Stephens Nunnally on 9/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Useful 2D Point Class, adapted from HullFinder
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
    double x;
    double y;
    DrPoint();
    DrPoint(double _x, double _y);
    DrPoint & operator=(const DrPoint &other);
    DrPoint operator+(const DrPoint &other) const;
    DrPoint operator-(const DrPoint &other) const;
    DrPoint operator*(double k) const;
    DrPoint operator/(double k) const;
    bool operator==(const DrPoint &other) const;

    double DotProduct(const DrPoint &other) const;
    double DistanceSquared(const DrPoint &to) const;
    double Distance(const DrPoint &to) const;
    double Distance(const DrPoint &segmentStart, const DrPoint &segmentEnd) const;
    double DecisionDistance(const std::vector<DrPoint> &points) const;
};


#endif // DR_POINT_H









