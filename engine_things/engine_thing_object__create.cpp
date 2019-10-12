//
//      Created by Stephens Nunnally on 6/27/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>

#include "3rd_party/hull_finder.h"
#include "3rd_party/poly_partition.h"
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine_thing_object.h"
#include "helper.h"
#include "helper_qt.h"

//####################################################################################
//##    Creates a list of Vertices that represent a scaled circle
//####################################################################################
QVector<DrPointF> DrEngineObject::createEllipseFromCircle(const DrPointF &center, const double &radius, const int &point_count) {
    QVector<DrPointF> ellipse;
    int count = point_count;
    for (int i = 0; i < count; i++) {
        QTransform t = QTransform().translate(center.x, center.y).rotate(i * 360.0 / count);
        QPointF point = t.map(QPointF( 0, radius));
        ellipse.append( DrPointF(point.x(), point.y()) );
    }
    return ellipse;
}


//####################################################################################
//##    Add Shapes to Object
//####################################################################################
void DrEngineObject::addShapeBox(double width, double height) {
    cpShape *shape = cpBoxShapeNew(this->body, width * static_cast<double>(this->getScaleX()), height * static_cast<double>(this->getScaleY()), c_extra_radius);
    double    area = (width * height);
    applyShapeSettings(shape, area, Shape_Type::Box);
}
void DrEngineObject::addShapeBoxFromTexture(long texture_number) {
    double width =  getWorld()->getTexture(texture_number)->width();
    double height = getWorld()->getTexture(texture_number)->height();
    addShapeBox(width, height);
}

void DrEngineObject::addShapeCircle(double circle_radius, DrPointF shape_offset) {
    // Check if Circle, but not perfect square scale, if so, create with a polygon ellipse instead of a circle
    if (Dr::FuzzyCompare(this->getScaleX(), this->getScaleY()) == false) {
        QVector<DrPointF> points = createEllipseFromCircle(shape_offset, circle_radius, 18);
        addShapePolygon(points);
    } else {
        double  radius = circle_radius * static_cast<double>(this->getScaleX());
        cpVect  offset = cpv(shape_offset.x, shape_offset.y);                               // Offset of collision shape
        cpShape *shape = cpCircleShapeNew(this->body, radius, offset);
        double    area = cpAreaForCircle( 0, circle_radius );
        applyShapeSettings(shape, area, Shape_Type::Circle);
    }
}
void DrEngineObject::addShapeCircleFromTexture(long texture_number) {
    double radius = getWorld()->getTexture(texture_number)->width() / 2.0;
    addShapeCircle(radius, DrPointF(0, 0));
}

void DrEngineObject::addShapeSegment(DrPointF p1, DrPointF p2, double padding) {
    cpShape *shape = cpSegmentShapeNew(this->body, cpv(p1.x, p1.y), cpv(p2.x, p2.y), padding);
    double   area =  cpAreaForSegment(cpv(p1.x, p1.y), cpv(p2.x, p2.y), padding);
    applyShapeSettings(shape, area, Shape_Type::Segment);
}

void DrEngineObject::addShapePolygon(const QVector<DrPointF> &points) {

    // Apply scale to points, verify Winding
    int old_point_count =static_cast<int>(points.size());
    double scale_x = static_cast<double>(this->getScaleX());
    double scale_y = static_cast<double>(this->getScaleY());
    QVector<DrPointF> scaled_points;
    for (auto &point : points) {
        scaled_points.push_back( DrPointF(point.x * scale_x, point.y * scale_y) );
    }
    HullFinder::EnsureWindingOrientation(scaled_points, Winding_Orientation::CounterClockwise);

    // Copy polygon Vertices into a scaled cpVect array, and scaled TPPLPoly array
    TPPLPoly            poly;   poly.Init(old_point_count);
    std::vector<cpVect> verts;  verts.resize( static_cast<size_t>(old_point_count) );
    for (int i = 0; i < old_point_count; i++) {
        double x = scaled_points[i].x;
        double y = scaled_points[i].y;
        poly[i].x = x;
        poly[i].y = y;
        verts[static_cast<size_t>(i)] = cpv(x, y);
    }

    // Calculate the convex hull of a given set of points. Returns the count of points in the hull. Result must be a pointer to a cpVect array
    // with at least count elements. If result is NULL, then verts array wil be reduced instead. first is an optional pointer to an integer to store
    // where the first vertex in the hull came from (i.e. verts[first] == result[0]). Tolerance (tol) is the allowed amount to shrink the hull when
    // simplifying it. A tolerance of 0.0 creates an exact hull.
    int first = 0;
    std::vector<cpVect> hull;   hull.clear();   hull.resize(  static_cast<size_t>(old_point_count) );       // Temporary array for ConvexHull call below
    int new_point_count = cpConvexHull(old_point_count, verts.data(), hull.data(), &first, 0.0);

    // !!!!! #NOTE: For Chipmunk Polygon Shapes, points must be in Counter-Clockwise Winding !!!!!
    // Shape is convex or could not determine convex hull
    if (new_point_count == 0) {
        Dr::ShowMessageBox("Warning! From addShapePolygon()... Could not form convex hull!");
    }
    if ((new_point_count == old_point_count || (new_point_count == 0))) {
        cpShape *shape = cpPolyShapeNew( this->body, old_point_count, verts.data(), cpTransformIdentity, c_extra_radius );
        double   area =  cpAreaForPoly(old_point_count, verts.data(), c_extra_radius );
        applyShapeSettings(shape, area, Shape_Type::Polygon);

    // Shape is concave
    } else {
        // Use 3rd_party library Poly Partition to partition concave polygon into convex polygons
        std::list<TPPLPoly> testpolys, result;
        testpolys.push_back( poly );
        TPPLPartition pp;
        pp.ConvexPartition_OPT(&(*testpolys.begin()), &result);
        ///pp.ConvexPartition_HM(&testpolys, &result);

        for (auto poly : result) {
            std::vector<cpVect> verts;
            verts.clear();
            verts.resize( static_cast<ulong>( poly.GetNumPoints()) );
            for (int i = 0; i < poly.GetNumPoints(); i++) {
                verts[static_cast<ulong>(i)] = cpv( poly[i].x, poly[i].y );
            }

            cpShape *shape = cpPolyShapeNew( this->body, static_cast<int>(poly.GetNumPoints()), verts.data(), cpTransformIdentity, c_extra_radius );
            double   area =  cpAreaForPoly(static_cast<int>(poly.GetNumPoints()), verts.data(), c_extra_radius );
            applyShapeSettings(shape, area, Shape_Type::Polygon);
        }
    }
}


//####################################################################################
//##    Applies Object Settings to Shape
//####################################################################################
void DrEngineObject::applyShapeSettings(cpShape *shape, double area, Shape_Type shape_type) {
    // Figrue out friction and bounce to use for this object
    double friction = (this->getCustomFriction() < 0) ? getWorld()->getFriction() : this->getCustomFriction();
    double bounce =   (this->getCustomBounce() < 0)   ? getWorld()->getBounce()   : this->getCustomBounce();

    // Setting mass of shape, body mass and moment of inertia are calculated by Chipmunk
    cpShapeSetMass( shape, area * c_mass_multiplier );
    cpShapeSetFriction( shape, friction );
    cpShapeSetElasticity( shape, bounce );                                          // Ideally between 0 and .99999
    cpShapeSetUserData( shape, this );                                              // Set UserData to DrEngineObject pointer

    // If we don't want the object to collide with other objects, set as sensor
    cpShapeSetSensor( shape, !(this->doesCollide()) );

    // Add shape to the list of shapes for this body
    this->shapes.push_back( shape );
    this->shape_type[shape] = shape_type;
}











