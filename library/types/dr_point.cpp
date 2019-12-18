//
//      Created by Stephens Nunnally on 10/2/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <math.h>

#include "library/types/dr_point.h"
#include "library/types/dr_pointf.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrPoint::DrPoint() {
    x = 0;
    y = 0;
}

DrPoint::DrPoint(int x_, int y_) {
    x = x_;
    y = y_;
}

DrPoint::DrPoint(float x_, float y_) {
    x = static_cast<int>(x_);
    y = static_cast<int>(y_);
}

DrPoint::DrPoint(double x_, double y_) {
    x = static_cast<int>(x_);
    y = static_cast<int>(y_);
}


//####################################################################################
//##    Overload Operators
//####################################################################################
DrPoint& DrPoint::operator=(const DrPoint &other) {
    x = other.x;
    y = other.y;
    return *this;
}

DrPoint DrPoint::operator+(const DrPoint &other) const {
    return DrPoint(x + other.x, y + other.y);
}

DrPoint DrPoint::operator-(const DrPoint &other) const {
    return DrPoint(x - other.x, y - other.y);
}

DrPoint DrPoint::operator*(int k) const {
    return DrPoint(x * k, y * k);
}

DrPoint DrPoint::operator/(int k) const {
    return DrPoint(x / k, y / k);
}

bool DrPoint::operator==(const DrPoint &other) const {
    return (x == other.x) && (y == other.y);
}


//####################################################################################
//##    Conversions
//####################################################################################
DrPointF DrPoint::toPointF() {
    return DrPointF( x, y );
}





