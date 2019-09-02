//
// Description:     Hull Finder
// Author:          Sam Hocevar
// License:         Licensed under WTFPL 2.0
// Source(s):       http://sam.zoy.org
//                  https://bitbucket.org/vostreltsov/concave-hull/src/default/
//
//            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
//                    Version 2, December 2004
//
// Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
//
// Everyone is permitted to copy and distribute verbatim or modified
// copies of this license document, and changing it is allowed as long
// as the name is changed.
//
//            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
//   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
//
//  0. You just DO WHAT THE FUCK YOU WANT TO.
//
//
#ifndef HULLFINDER_H
#define HULLFINDER_H

#include <math.h>
#include <QList>
#include <QVector>
#include <QMap>

class HullPoint
{
public:
    double x;
    double y;
    HullPoint();
    HullPoint(double _x, double _y);
    HullPoint & operator=(const HullPoint & other);
    HullPoint operator+(const HullPoint & other) const;
    HullPoint operator-(const HullPoint & other) const;
    HullPoint operator*(double k) const;
    HullPoint operator/(double k) const;
    bool operator==(const HullPoint & other) const;

    double DotProduct(const HullPoint & other) const;
    double DistanceSquared(const HullPoint & to) const;
    double Distance(const HullPoint & to) const;
    double Distance(const HullPoint & segmentStart, const HullPoint & segmentEnd) const;
    double DecisionDistance(const QList<HullPoint> & points) const;
};

class HullFinder
{
public:
    HullFinder();

    static double IsLeft(HullPoint p0, HullPoint p1, HullPoint p2);
    static bool IsPointInsidePolygon(HullPoint v, const QVector<HullPoint> & polygon);
    static bool CheckEdgeIntersection(const HullPoint & p1, const HullPoint & p2, const HullPoint & p3, const HullPoint & p4);
    static bool CheckEdgeIntersection(const QVector<HullPoint> & hull, HullPoint curEdgeStart, HullPoint curEdgeEnd, HullPoint checkEdgeStart, HullPoint checkEdgeEnd);
    static HullPoint NearestInnerPoint(HullPoint edgeStart, HullPoint edgeEnd, const QVector<HullPoint> & points, const QVector<HullPoint> & hull, bool * found);
    static QVector<HullPoint> FindConvexHull(const QVector<HullPoint> & points);
    static QVector<HullPoint> FindConcaveHull(const QVector<HullPoint> & points, double N);
};

#endif // HULLFINDER_H
