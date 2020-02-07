//
//      Created by Stephens Nunnally on 1/28/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//      To be Engine compatible, this file does NOT, and should NOT #include any Qt Library types
//
#ifndef DR_POLYGONF_H
#define DR_POLYGONF_H

#include <vector>

// Forward Declarations
class DrPointF;
enum class Winding_Orientation;


//####################################################################################
//##    DrPolygonF
//##        A polygon class, useful for intersections
//############################
class DrPolygonF
{
private:
    std::vector<DrPointF> m_points;

public:
    // Constructor
    DrPolygonF();
    DrPolygonF(const DrPolygonF &polygon);

    // Access
    const std::vector<DrPointF>& points()       { return m_points; }

    // Info
    bool            isEmpty() const             { return (m_points.size() == 0); }
    long            numberOfPoints() const      { return static_cast<long>(m_points.size()); }

    // Manangement
    void            addPoint(DrPointF point);
    void            clear()                     { m_points.clear(); }

    // Polygon / Line Functions
    static bool                 onSegment(DrPointF line_a, DrPointF point, DrPointF line_b);
    static Winding_Orientation  orientation(DrPointF p, DrPointF q, DrPointF r);
    static bool                 doIntersect(DrPointF p1, DrPointF q1, DrPointF p2, DrPointF q2);
    bool                        isInside(DrPointF point);


};

#endif // DR_POLYGONF_H
