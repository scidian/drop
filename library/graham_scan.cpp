//
//      Created by Stephens Nunnally on 5/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <stack>
#include <stdlib.h>

#include "graham_scan.h"

// A global point needed for  sorting points with reference to the first point Used in compare function of qsort()
static gsPoint p0;

// A utility function to find next to top in a stack
gsPoint nextToTop(std::stack<gsPoint> &S) {
    gsPoint p = S.top();
    S.pop();
    gsPoint res = S.top();
    S.push(p);
    return res;
}

// A utility function to swap two points
void swap(gsPoint &p1, gsPoint &p2) {
    gsPoint temp = p1;
    p1 = p2;
    p2 = temp;
}

// A utility function to return square of distance between p1 and p2
double distSq(gsPoint p1, gsPoint p2) {
    return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(gsPoint p, gsPoint q, gsPoint r) {
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (qFuzzyCompare(val, 0)) return 0;            // colinear
    return (val > 0)? 1: 2;                         // clock or counterclock wise
}

// A function used by library function qsort() to sort an array of points with respect to the first point
int compare(const void *vp1, const void *vp2) {
    ///Point *p1 = (Point *)vp1;
    ///Point *p2 = (Point *)vp2;
    const gsPoint *p1 = static_cast<const gsPoint*>(vp1);
    const gsPoint *p2 = static_cast<const gsPoint*>(vp2);

    // Find orientation
    int o = orientation(p0, *p1, *p2);
    if (o == 0)
        return (distSq(p0, *p2) >= distSq(p0, *p1))? -1 : 1;

    return (o == 2)? -1: 1;
}

// Prints convex hull of a set of np points.
extern QVector<QPointF> convexHull(QVector<gsPoint> points, int np) {
    QVector<QPointF> new_points;

    // Find the bottommost point
    double ymin = points[0].y;
    int min = 0;
    for (int i = 1; i < np; i++) {
        double y = points[i].y;

        // Pick the bottom-most or chose the left most point in case of tie
        if ((y < ymin) || (qFuzzyCompare(ymin, y) && points[i].x < points[min].x)) {
            ymin = points[i].y;
            min = i;
        }
    }

    // Place the bottom-most point at first position
    swap(points[0], points[min]);

    // Sort n-1 points with respect to the first point.
    // A point p1 comes before p2 in sorted ouput if p2 has larger polar angle (in counterclockwise direction) than p1
    p0 = points[0];
    qsort(&points[1], static_cast<unsigned long>(np - 1), sizeof(gsPoint), compare);

    // If two or more points make same angle with p0, Remove all but the one that is farthest from p0
    // Remember that, in above sorting, our criteria was to keep the farthest point at the end when more than
    // one points have same angle.
    int m = 1; // Initialize size of modified array
    for (int i = 1; i < np; i++) {
        // Keep removing i while angle of i and i+1 is same with respect to p0
        while (i < np - 1 && orientation(p0, points[i], points[i+1]) == 0)
            i++;

        points[m] = points[i];
        m++;                                         // Update size of modified array
    }

    // If modified array of points has less than 3 points, convex hull is not possible
    if (m < 3)
       return new_points;

    // Create an empty stack and push first three points to it.
    std::stack<gsPoint> S;
    S.push(points[0]);
    S.push(points[1]);
    S.push(points[2]);

    // Process remaining n-3 points
    for (int i = 3; i < m; i++) {
        // Keep removing top while the angle formed by points next-to-top, top, and points[i] makes a non-left turn
        while (orientation(nextToTop(S), S.top(), points[i]) != 2)
            S.pop();
        S.push(points[i]);
    }

    // Now stack has the output points, save them to new point list
    while (!S.empty()) {
        gsPoint p = S.top();
        new_points.append( QPointF(p.x, p.y));
        S.pop();
    }

    return new_points;
}














