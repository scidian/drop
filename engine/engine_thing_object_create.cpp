//
//      Created by Stephens Nunnally on 6/27/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>

#include "engine.h"
#include "engine_texture.h"
#include "engine_thing_object.h"
#include "engine_world.h"
#include "helper.h"
#include "library/poly_partition.h"

//####################################################################################
//##    Creates a list of Vertices that represent a scaled circle
//####################################################################################
static QVector<QPointF> createEllipseFromCircle(const QPointF &center, const double &radius) {
    QVector<QPointF> ellipse;
    int count = 18;
    for (int i = 0; i < count; i++) {
        QTransform t = QTransform().translate(center.x(), center.y()).rotate(i * 360.0 / count);
        QPointF point = t.map(QPointF( 0, radius));
        ellipse.append( point );
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

void DrEngineObject::addShapeCircle(double circle_radius, QPointF shape_offset) {
    // Check if Circle, but not perfect square scale, if so, create with a polygon ellipse instead of a circle
    if (qFuzzyCompare(this->getScaleX(), this->getScaleY()) == false) {
        QVector<QPointF> points = createEllipseFromCircle(shape_offset, circle_radius);
        addShapePolygon(points);
    } else {
        double  radius = circle_radius * static_cast<double>(this->getScaleX());
        cpVect  offset = cpv(shape_offset.x(), shape_offset.y());                           // Offset of collision shape
        cpShape *shape = cpCircleShapeNew(this->body, radius, offset);
        double    area = cpAreaForCircle( 0, circle_radius );
        applyShapeSettings(shape, area, Shape_Type::Circle);
    }
}
void DrEngineObject::addShapeCircleFromTexture(long texture_number) {
    double radius = getWorld()->getTexture(texture_number)->width() / 2.0;
    addShapeCircle(radius, QPointF(0, 0));
}

void DrEngineObject::addShapeSegment(QPointF p1, QPointF p2, double padding) {
    cpShape *shape = cpSegmentShapeNew(this->body, cpv(p1.x(), p1.y()), cpv(p2.x(), p2.y()), padding);
    double   area =  cpAreaForSegment(cpv(p1.x(), p1.y()), cpv(p2.x(), p2.y()), padding);
    applyShapeSettings(shape, area, Shape_Type::Segment);
}

void DrEngineObject::addShapePolygon(QVector<QPointF> &points) {

    int old_point_count =static_cast<int>(points.size());
    double scale_x = static_cast<double>(this->getScaleX());
    double scale_y = static_cast<double>(this->getScaleY());

    // Copy polygon Vertices into a scaled cpVect array
    std::vector<cpVect> verts;
    verts.clear();
    verts.resize( static_cast<ulong>(old_point_count) );
    for (int i = 0; i < old_point_count; i++)
        verts[static_cast<ulong>(i)] = cpv( points[i].x() * scale_x, points[i].y() * scale_y);

    // Determine if polygon is concave, if it is create multiple shapes, otherwise create one shape
    std::list<TPPLPoly> testpolys, result;                              // Used by library Poly Partition
    TPPLPoly poly;
    poly.Init(old_point_count);
    for (int i = 0; i < old_point_count; i++) {
        poly[i].x = points[i].x() * scale_x;
        poly[i].y = points[i].y() * scale_y;
    }
    testpolys.push_back( poly );

    // Calculate the convex hull of a given set of points. Returns the count of points in the hull. Result must be a pointer to a cpVect array
    // with at least count elements. If result is NULL, then verts array wil be reduced instead. first is an optional pointer to an integer to store
    // where the first vertex in the hull came from (i.e. verts[first] == result[0]) tol is the allowed amount to shrink the hull when simplifying it.
    // A tolerance of 0.0 creates an exact hull.
    cpVect hull[99];
    int first = 0;
    int new_point_count = cpConvexHull(old_point_count, verts.data(), hull, &first, 0.0);

    // Shape is convex or could not determine convex hull
    if (new_point_count == 0) Dr::ShowMessageBox("Warning! Could not form convex hull!");
    if ((new_point_count == old_point_count || (new_point_count == 0))) {
        cpShape *shape = cpPolyShapeNew( this->body, old_point_count, verts.data(), cpTransformIdentity, c_extra_radius);
        double   area =  cpAreaForPoly(old_point_count, verts.data(), c_extra_radius );
        applyShapeSettings(shape, area, Shape_Type::Polygon);

    // Shape is concave
    } else {
        TPPLPartition pp;
        pp.ConvexPartition_OPT(&(*testpolys.begin()), &result);
        ///pp.ConvexPartition_HM(&testpolys, &result);

        for (auto poly : result) {
            std::vector<cpVect> verts;
            verts.clear();
            verts.resize( static_cast<ulong>( poly.GetNumPoints()) );
            for (int i = 0; i < poly.GetNumPoints(); i++)
                verts[static_cast<ulong>(i)] = cpv( poly[i].x, poly[i].y );
            cpShape *shape = cpPolyShapeNew( this->body, static_cast<int>(poly.GetNumPoints()), verts.data(), cpTransformIdentity, c_extra_radius);
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











