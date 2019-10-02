//
//      Created by Stephens Nunnally on 9/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Useful 2D Point Class with Doubles, adapted from HullFinder
//
//
#ifndef DR_POINT_F_H
#define DR_POINT_F_H

#include <vector>

//####################################################################################
//##    DrPointF
//##        Useful 2D Point Class, adapted from HullFinder
//############################
class DrPointF
{
public:
    double x;
    double y;
    DrPointF();
    DrPointF(double x_, double y_);
    DrPointF & operator=(const DrPointF &other);
    DrPointF operator+(const DrPointF &other) const;
    DrPointF operator-(const DrPointF &other) const;
    DrPointF operator*(double k) const;
    DrPointF operator/(double k) const;
    bool     operator==(const DrPointF &other) const;

    double DotProduct(const DrPointF &other) const;
    double DistanceSquared(const DrPointF &to) const;
    double Distance(const DrPointF &to) const;
    double Distance(const DrPointF &segment_start, const DrPointF &segment_end) const;
    double DecisionDistance(const std::vector<DrPointF> &points) const;
};


#endif // DR_POINT_F_H









