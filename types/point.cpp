//
//      Created by Stephens Nunnally on 9/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <math.h>

#include "helper.h"
#include "types/point.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrPoint::DrPoint() {
    x = 0;
    y = 0;
}

DrPoint::DrPoint(double _x, double _y) {
    x = _x;
    y = _y;
}

//####################################################################################
//##    Overload Operators
//####################################################################################
DrPoint & DrPoint::operator=(const DrPoint &other) {
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

DrPoint DrPoint::operator*(double k) const {
    return DrPoint(x * k, y * k);
}

DrPoint DrPoint::operator/(double k) const {
    return DrPoint(x / k, y / k);
}

bool DrPoint::operator==(const DrPoint &other) const {
    return Dr::FuzzyCompare(x, other.x) && Dr::FuzzyCompare(y, other.y);
}

//####################################################################################
//##    Point Functions
//####################################################################################
double DrPoint::DotProduct(const DrPoint &other) const {
    return x * other.x + y * other.y;
}

double DrPoint::DistanceSquared(const DrPoint &to) const {
    return static_cast<double>( ((to.x - x) * (to.x - x) + (to.y - y) * (to.y - y)) );
}

double DrPoint::Distance(const DrPoint &to) const {
    return sqrt(DistanceSquared(to));
}

double DrPoint::Distance(const DrPoint &segmentStart, const DrPoint &segmentEnd) const {
    const double l2 = segmentStart.DistanceSquared(segmentEnd);
    if (l2 == 0.0) {
        return Distance(segmentStart);          // v == w case
    }

    // Consider the line extending the segment, parameterized as v + t (w - v)
    // We find projection of DrPoint p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    const double t = ((*this - segmentStart).DotProduct(segmentEnd - segmentStart)) / l2;
    if (t < 0.0) {
        return Distance(segmentStart);          // Beyond the 'v' end of the segment
    } else if (t > 1.0) {
        return Distance(segmentEnd);            // Beyond the 'w' end of the segment
    }

    // Projection falls on the segment
    DrPoint projection = segmentStart + (segmentEnd - segmentStart) * t;
    return Distance(projection);
}

double DrPoint::DecisionDistance(const std::vector<DrPoint> &DrPoints) const {
    DrPoint result = DrPoints[0];
    double dst = Distance(DrPoints[0]);
    for (std::size_t i = 1; i < DrPoints.size(); i++) {
        DrPoint cur = DrPoints[i];
        double curDistance = Distance(cur);
        if (curDistance < dst) {
            result = cur;
            dst = curDistance;
        }
    }
    return dst;
}












