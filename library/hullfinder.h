#ifndef HULLFINDER_H
#define HULLFINDER_H

#include <math.h>
#include <QList>
#include <QVector>
#include <QMap>

class Point
{
public:
    double x;
    double y;
    Point();
    Point(double _x, double _y);
    Point & operator=(const Point & other);
    Point operator+(const Point & other) const;
    Point operator-(const Point & other) const;
    Point operator*(double k) const;
    Point operator/(double k) const;
    bool operator==(const Point & other) const;

    double DotProduct(const Point & other) const;
    double DistanceSquared(const Point & to) const;
    double Distance(const Point & to) const;
    double Distance(const Point & segmentStart, const Point & segmentEnd) const;
    double DecisionDistance(const QList<Point> & points) const;
};

class HullFinder
{
public:
    HullFinder();

    static double IsLeft(Point p0, Point p1, Point p2);
    static bool IsPointInsidePolygon(Point v, const QVector<Point> & polygon);
    static bool CheckEdgeIntersection(const Point & p1, const Point & p2, const Point & p3, const Point & p4);
    static bool CheckEdgeIntersection(const QVector<Point> & hull, Point curEdgeStart, Point curEdgeEnd, Point checkEdgeStart, Point checkEdgeEnd);
    static Point NearestInnerPoint(Point edgeStart, Point edgeEnd, const QVector<Point> & points, const QVector<Point> & hull, bool * found);
    static QVector<Point> FindConvexHull(const QVector<Point> & points);
    static QVector<Point> FindConcaveHull(const QVector<Point> & points, double N);
};

#endif // HULLFINDER_H
