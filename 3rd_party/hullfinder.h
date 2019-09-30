//
// Description:     Concave Hull Finder from a collection of Points
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

#include <QList>
#include <QVector>
#include <QMap>

#include <math.h>
#include <vector>

// Forward Declarations
class DrPoint;

// Local Enums
enum class Winding_Orientation {
    Clockwise,
    CounterClockwise,
    Unknown,
};


//####################################################################################
//##    HullFinder
//##        Finds concave hulls from a random set of points
//############################
class HullFinder
{
public:
    HullFinder();

    static double IsLeft(DrPoint p0, DrPoint p1, DrPoint p2);
    static bool IsPointInsidePolygon(DrPoint v, const QVector<DrPoint> &polygon);
    static bool CheckEdgeIntersection(const DrPoint &p1, const DrPoint &p2, const DrPoint &p3, const DrPoint &p4);
    static bool CheckEdgeIntersection(const QVector<DrPoint> &hull, DrPoint curEdgeStart, DrPoint curEdgeEnd, DrPoint checkEdgeStart, DrPoint checkEdgeEnd);
    static DrPoint NearestInnerPoint(DrPoint edgeStart, DrPoint edgeEnd, const QVector<DrPoint> & points, const QVector<DrPoint> & hull, bool *found);
    static QVector<DrPoint> FindConvexHull(const QVector<DrPoint> &points);
    static QVector<DrPoint> FindConcaveHull(const QVector<DrPoint> &points, double N);

    // Functions Added by Stevinz
    static Winding_Orientation FindWindingOrientation(const QVector<DrPoint> &points);
};

#endif // HULLFINDER_H









