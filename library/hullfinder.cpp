#include "hullfinder.h"

Point::Point()
{
    x = 0;
    y = 0;
}

Point::Point(double _x, double _y)
{
    x = _x;
    y = _y;
}

Point & Point::operator=(const Point & other)
{
    x = other.x;
    y = other.y;
    return *this;
}

Point Point::operator+(const Point & other) const
{
    return Point(x + other.x, y + other.y);
}

Point Point::operator-(const Point & other) const
{
    return Point(x - other.x, y - other.y);
}

Point Point::operator*(double k) const
{
    return Point(x * k, y * k);
}

Point Point::operator/(double k) const
{
    return Point(x / k, y / k);
}

bool Point::operator==(const Point & other) const
{
    return x == other.x && y == other.y;
}

double Point::DotProduct(const Point & other) const
{
    return x * other.x + y * other.y;
}

double Point::DistanceSquared(const Point & to) const
{
    return (double)((to.x - x) * (to.x - x) + (to.y - y) * (to.y - y));
}

double Point::Distance(const Point & to) const
{
    return sqrt(DistanceSquared(to));
}

double Point::Distance(const Point & segmentStart, const Point & segmentEnd) const
{
    const double l2 = segmentStart.DistanceSquared(segmentEnd);
    if (l2 == 0.0) {
        return Distance(segmentStart);   // v == w case
    }

    // Consider the line extending the segment, parameterized as v + t (w - v)
    // We find projection of point p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    const double t = ((*this - segmentStart).DotProduct(segmentEnd - segmentStart)) / l2;
    if (t < 0.0) {
        return Distance(segmentStart); // Beyond the 'v' end of the segment
    } else if (t > 1.0) {
        return Distance(segmentEnd);   // Beyond the 'w' end of the segment
    }

    // Projection falls on the segment
    Point projection = segmentStart + (segmentEnd - segmentStart) * t;
    return Distance(projection);
}

double Point::DecisionDistance(const QList<Point> & points) const
{
    Point result = points[0];
    double dst = Distance(points[0]);
    for (int i = 1; i < points.size(); i++) {
        Point cur = points[i];
        double curDistance = Distance(cur);
        if (curDistance < dst) {
            result = cur;
            dst = curDistance;
        }
    }
    return dst;
}

HullFinder::HullFinder()
{
}

double HullFinder::IsLeft(Point p0, Point p1, Point p2)
{
    return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
}

bool HullFinder::IsPointInsidePolygon(Point v, const QVector<Point> & polygon)
{
    bool result = false;
    int j = polygon.count() - 1;
    for (int i = 0; i < polygon.count(); i++)
    {
        if ((polygon[i].y < v.y && polygon[j].y > v.y) || (polygon[j].y < v.y && polygon[i].y > v.y))
        {
            if (polygon[i].x + (v.y - polygon[i].y) / (polygon[j].y - polygon[i].y) * (polygon[j].x - polygon[i].x) < v.x)
            {
                result = !result;
            }
        }
        j = i;
    }
    return result;
}

bool HullFinder::CheckEdgeIntersection(const Point & p0, const Point & p1, const Point & p2, const Point & p3)
{
    double s1_x = p1.x - p0.x;
    double s1_y = p1.y - p0.y;
    double s2_x = p3.x - p2.x;
    double s2_y = p3.y - p2.y;
    double s = (-s1_y * (p0.x - p2.x) + s1_x * (p0.y - p2.y)) / (-s2_x * s1_y + s1_x * s2_y);
    double t = ( s2_x * (p0.y - p2.y) - s2_y * (p0.x - p2.x)) / (-s2_x * s1_y + s1_x * s2_y);
    return (s > 0 && s < 1 && t > 0 && t < 1);
}

bool HullFinder::CheckEdgeIntersection(const QVector<Point> & hull, Point curEdgeStart, Point curEdgeEnd, Point checkEdgeStart, Point checkEdgeEnd)
{
    for (int i = 0; i < hull.size() - 2; i++) {
        int e1 = i;
        int e2 = i + 1;
        Point p1 = hull[e1];
        Point p2 = hull[e2];

        if (curEdgeStart == p1 && curEdgeEnd == p2) {
            continue;
        }

        if (CheckEdgeIntersection(checkEdgeStart, checkEdgeEnd, p1, p2)) {
            return true;
        }
    }
    return false;
}

Point HullFinder::NearestInnerPoint(Point edgeStart, Point edgeEnd, const QVector<Point> &points, const QVector<Point> &hull, bool * found)
{
    Point result;
    double distance = 0;
    *found = false;

    foreach (Point p, points) {
        // Skip points that are already in he hull
        if (hull.contains(p)) {
            continue;
        }
        /*if (!IsPointInsidePolygon(p, hull)) {
            continue;
        }*/

        double d = p.Distance(edgeStart, edgeEnd);
        bool skip = false;
        for (int i = 0; !skip && i < hull.size() - 1; i++) {
            double dTmp = p.Distance(hull[i], hull[i + 1]);
            skip |= dTmp < d;
        }
        if (skip) {
            continue;
        }

        if (!(*found) || distance > d) {
            result = p;
            distance = d;
            *found = true;
        }
    }
    return result;
}

QVector<Point> HullFinder::FindConvexHull(const QVector<Point> & points)
{
    QVector<Point> P = points;
    QVector<Point> H;

    // Sort P by x and y
    for (int i = 0; i < P.size(); i++) {
        for (int j = i + 1; j < P.size(); j++) {
            if (P[j].x < P[i].x || (P[j].x == P[i].x && P[j].y < P[i].y)) {
                Point tmp = P[i];
                P[i] = P[j];
                P[j] = tmp;
            }
        }
    }

    // the output array H[] will be used as the stack
    int i;                 // array scan index

    // Get the indices of points with min x-coord and min|max y-coord
    int minmin = 0, minmax;
    double xmin = P[0].x;
    for (i = 1; i < P.size(); i++)
        if (P[i].x != xmin) break;
    minmax = i - 1;
    if (minmax == P.size() - 1) {       // degenerate case: all x-coords == xmin
        H.push_back(P[minmin]);
        if (P[minmax].y != P[minmin].y) // a  nontrivial segment
            H.push_back(P[minmax]);
        H.push_back(P[minmin]);            // add polygon endpoint
        return H;
    }

    // Get the indices of points with max x-coord and min|max y-coord
    int maxmin, maxmax = P.size() - 1;
    double xmax = P.last().x;
    for (i = P.size() - 2; i >= 0; i--)
        if (P[i].x != xmax) break;
    maxmin = i+1;

    // Compute the lower hull on the stack H
    H.push_back(P[minmin]);      // push  minmin point onto stack
    i = minmax;
    while (++i <= maxmin)
    {
        // the lower line joins P[minmin]  with P[maxmin]
        if (IsLeft(P[minmin], P[maxmin], P[i]) >= 0 && i < maxmin)
            continue;           // ignore P[i] above or on the lower line

        while (H.size() > 1)         // there are at least 2 points on the stack
        {
            // test if  P[i] is left of the line at the stack top
            if (IsLeft(H[H.size() - 2], H.last(), P[i]) > 0)
                break;         // P[i] is a new hull  vertex
            H.pop_back();         // pop top point off  stack
        }
        H.push_back(P[i]);        // push P[i] onto stack
    }

    // Next, compute the upper hull on the stack H above  the bottom hull
    if (maxmax != maxmin)      // if  distinct xmax points
         H.push_back(P[maxmax]);  // push maxmax point onto stack
    int bot = H.size();                  // the bottom point of the upper hull stack
    i = maxmin;
    while (--i >= minmax)
    {
        // the upper line joins P[maxmax]  with P[minmax]
        if (IsLeft( P[maxmax], P[minmax], P[i])  >= 0 && i > minmax)
            continue;           // ignore P[i] below or on the upper line

        while (H.size() > bot)     // at least 2 points on the upper stack
        {
            // test if  P[i] is left of the line at the stack top
            if (IsLeft(H[H.size() - 2], H.last(), P[i]) > 0)
                break;         // P[i] is a new hull  vertex
            H.pop_back();         // pop top point off stack
        }
        H.push_back(P[i]);        // push P[i] onto stack
    }
    if (minmax != minmin)
        H.push_back(P[minmin]);  // push  joining endpoint onto stack

    return H;
}

QVector<Point> HullFinder::FindConcaveHull(const QVector<Point> &points, double N) {
    QVector<Point> concaveList = FindConvexHull(points);

    for (int i = 0; i < concaveList.size() - 1; i++) {
        // Find the nearest inner point pk ∈ G from the edge (ci1, ci2);
        Point ci1 = concaveList[i];
        Point ci2 = concaveList[i + 1];

        bool found;
        Point pk = NearestInnerPoint(ci1, ci2, points, concaveList, &found);
        if (!found || concaveList.contains(pk)) {
            continue;
        }

        double eh = ci1.Distance(ci2);  // the lenght of the edge
        QList<Point> tmp;
        tmp.push_back(ci1);
        tmp.push_back(ci2);
        double dd = pk.DecisionDistance(tmp);

        if (eh / dd > N) {
            // Check that new candidate edge will not intersect existing edges.
            bool intersects = CheckEdgeIntersection(concaveList, ci1, ci2, ci1, pk);
            intersects |= CheckEdgeIntersection(concaveList, ci1, ci2, pk, ci2);
            if (!intersects) {
                concaveList.insert(concaveList.begin() + i + 1, pk);
                i--;
            }
        }
    }
    return concaveList;
}
