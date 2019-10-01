//
//
//
//
//
#include "hullfinder.h"
#include "types/point.h"


HullFinder::HullFinder() { }

double HullFinder::IsLeft(DrPoint p0, DrPoint p1, DrPoint p2) {
    return (p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y);
}

bool HullFinder::IsPointInsidePolygon(DrPoint v, const QVector<DrPoint> &polygon) {
    bool result = false;
    int j = polygon.count() - 1;
    for (int i = 0; i < polygon.count(); i++) {
        if ((polygon[i].y < v.y && polygon[j].y > v.y) || (polygon[j].y < v.y && polygon[i].y > v.y)) {
            if (polygon[i].x + (v.y - polygon[i].y) / (polygon[j].y - polygon[i].y) * (polygon[j].x - polygon[i].x) < v.x) {
                result = !result;
            }
        }
        j = i;
    }
    return result;
}

bool HullFinder::CheckEdgeIntersection(const DrPoint &p0, const DrPoint &p1, const DrPoint &p2, const DrPoint &p3) {
    double s1_x = p1.x - p0.x;
    double s1_y = p1.y - p0.y;
    double s2_x = p3.x - p2.x;
    double s2_y = p3.y - p2.y;
    double s = (-s1_y * (p0.x - p2.x) + s1_x * (p0.y - p2.y)) / (-s2_x * s1_y + s1_x * s2_y);
    double t = ( s2_x * (p0.y - p2.y) - s2_y * (p0.x - p2.x)) / (-s2_x * s1_y + s1_x * s2_y);
    return (s > 0 && s < 1 && t > 0 && t < 1);
}

bool HullFinder::CheckEdgeIntersection(const QVector<DrPoint> &hull, DrPoint curEdgeStart, DrPoint curEdgeEnd, DrPoint checkEdgeStart, DrPoint checkEdgeEnd) {
    for (int i = 0; i < hull.size() - 2; i++) {
        int e1 = i;
        int e2 = i + 1;
        DrPoint p1 = hull[e1];
        DrPoint p2 = hull[e2];

        if (curEdgeStart == p1 && curEdgeEnd == p2) {
            continue;
        }

        if (CheckEdgeIntersection(checkEdgeStart, checkEdgeEnd, p1, p2)) {
            return true;
        }
    }
    return false;
}

DrPoint HullFinder::NearestInnerPoint(DrPoint edgeStart, DrPoint edgeEnd, const QVector<DrPoint> &DrPoints, const QVector<DrPoint> &hull, bool *found) {
    DrPoint result;
    double distance = 0;
    *found = false;

    foreach (DrPoint p, DrPoints) {
        // Skip DrPoints that are already in he hull
        if (hull.contains(p)) {
            continue;
        }
        /*if (!IsDrPointInsidePolygon(p, hull)) {
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

QVector<DrPoint> HullFinder::FindConvexHull(const QVector<DrPoint> &DrPoints) {
    QVector<DrPoint> P = DrPoints;
    QVector<DrPoint> H;

    // Sort P by x and y
    for (int i = 0; i < P.size(); i++) {
        for (int j = i + 1; j < P.size(); j++) {
            if (P[j].x < P[i].x || (qFuzzyCompare(P[j].x, P[i].x) && P[j].y < P[i].y)) {
                DrPoint tmp = P[i];
                P[i] = P[j];
                P[j] = tmp;
            }
        }
    }

    // the output array H[] will be used as the stack
    int i;                              // array scan index

    // Get the indices of DrPoints with min x-coord and min|max y-coord
    int minmin = 0, minmax;
    double xmin = P[0].x;
    for (i = 1; i < P.size(); i++)
        if (qFuzzyCompare(P[i].x, xmin) == false) break;
    minmax = i - 1;
    if (minmax == P.size() - 1) {       // degenerate case: all x-coords == xmin
        H.push_back(P[minmin]);
        if (qFuzzyCompare(P[minmax].y, P[minmin].y) == false) // a  nontrivial segment
            H.push_back(P[minmax]);
        H.push_back(P[minmin]);         // add polygon endDrPoint
        return H;
    }

    // Get the indices of DrPoints with max x-coord and min|max y-coord
    int maxmin, maxmax = P.size() - 1;
    double xmax = P.last().x;
    for (i = P.size() - 2; i >= 0; i--)
        if (qFuzzyCompare(P[i].x, xmax) == false) break;
    maxmin = i+1;

    // Compute the lower hull on the stack H
    H.push_back(P[minmin]);             // push  minmin DrPoint onto stack
    i = minmax;
    while (++i <= maxmin) {
        // the lower line joins P[minmin]  with P[maxmin]
        if (IsLeft(P[minmin], P[maxmin], P[i]) >= 0 && i < maxmin)
            continue;                   // ignore P[i] above or on the lower line

        while (H.size() > 1)            // there are at least 2 DrPoints on the stack
        {
            // test if  P[i] is left of the line at the stack top
            if (IsLeft(H[H.size() - 2], H.last(), P[i]) > 0)
                break;                  // P[i] is a new hull  vertex
            H.pop_back();               // pop top DrPoint off  stack
        }
        H.push_back(P[i]);              // push P[i] onto stack
    }

    // Next, compute the upper hull on the stack H above  the bottom hull
    if (maxmax != maxmin)               // if  distinct xmax DrPoints
         H.push_back(P[maxmax]);        // push maxmax DrPoint onto stack
    int bot = H.size();                 // the bottom DrPoint of the upper hull stack
    i = maxmin;
    while (--i >= minmax) {
        // the upper line joins P[maxmax]  with P[minmax]
        if (IsLeft( P[maxmax], P[minmax], P[i])  >= 0 && i > minmax)
            continue;                   // ignore P[i] below or on the upper line

        while (H.size() > bot)          // at least 2 DrPoints on the upper stack
        {
            // test if  P[i] is left of the line at the stack top
            if (IsLeft(H[H.size() - 2], H.last(), P[i]) > 0)
                break;                  // P[i] is a new hull  vertex
            H.pop_back();               // pop top DrPoint off stack
        }
        H.push_back(P[i]);              // push P[i] onto stack
    }
    if (minmax != minmin)
        H.push_back(P[minmin]);         // push joining endDrPoint onto stack

    return H;
}

QVector<DrPoint> HullFinder::FindConcaveHull(const QVector<DrPoint> &DrPoints, double N) {
    QVector<DrPoint> concaveList = FindConvexHull(DrPoints);

    for (int i = 0; i < concaveList.size() - 1; i++) {
        // Find the nearest inner DrPoint pk ∈ G from the edge (ci1, ci2);
        DrPoint ci1 = concaveList[i];
        DrPoint ci2 = concaveList[i + 1];

        bool found;
        DrPoint pk = NearestInnerPoint(ci1, ci2, DrPoints, concaveList, &found);
        if (!found || concaveList.contains(pk)) {
            continue;
        }

        double eh = ci1.Distance(ci2);  // the length of the edge
        std::vector<DrPoint> tmp;
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

//####################################################################################
//##    Makes sure points are in the desired Winding Orientation
//####################################################################################
void HullFinder::EnsureWindingOrientation(QVector<DrPoint> &points, Winding_Orientation direction_desired) {
    Winding_Orientation winding = HullFinder::FindWindingOrientation(points);
    if ((winding == Winding_Orientation::Clockwise        && direction_desired == Winding_Orientation::CounterClockwise) ||
        (winding == Winding_Orientation::CounterClockwise && direction_desired == Winding_Orientation::Clockwise))
    {
        std::reverse(points.begin(), points.end());
    }
}

//####################################################################################
//##    Returns winding direction of points
//####################################################################################
Winding_Orientation HullFinder::FindWindingOrientation(const QVector<DrPoint> &points) {
    int i1, i2;
    double area = 0;
    for (i1 = 0; i1 < points.count(); i1++) {
        i2 = i1 + 1;
        if (i2 == points.count()) i2 = 0;
        area += points[i1].x * points[i2].y - points[i1].y * points[i2].x;
    }
    if (area > 0) return Winding_Orientation::CounterClockwise;
    if (area < 0) return Winding_Orientation::Clockwise;
    return Winding_Orientation::Unknown;
}












