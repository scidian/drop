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
class DrPointF;

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

    static double IsLeft(DrPointF p0, DrPointF p1, DrPointF p2);
    static bool IsPointInsidePolygon(DrPointF v, const QVector<DrPointF> &polygon);
    static bool CheckEdgeIntersection(const DrPointF &p1, const DrPointF &p2, const DrPointF &p3, const DrPointF &p4);
    static bool CheckEdgeIntersection(const QVector<DrPointF> &hull, DrPointF curEdgeStart, DrPointF curEdgeEnd, DrPointF checkEdgeStart, DrPointF checkEdgeEnd);
    static DrPointF NearestInnerPoint(DrPointF edgeStart, DrPointF edgeEnd, const QVector<DrPointF> & points, const QVector<DrPointF> & hull, bool *found);
    static QVector<DrPointF> FindConvexHull(const QVector<DrPointF> &points);
    static QVector<DrPointF> FindConcaveHull(const QVector<DrPointF> &points, double N);

    // Functions Added by Stevinz
    static void                 EnsureWindingOrientation(QVector<DrPointF> &points, Winding_Orientation direction_desired);
    static Winding_Orientation  FindWindingOrientation(const QVector<DrPointF> &points);
};

#endif // HULLFINDER_H









