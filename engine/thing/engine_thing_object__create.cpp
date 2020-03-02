//
//      Created by Stephens Nunnally on 6/27/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/hull_finder.h"
#include "3rd_party/poly_partition.h"
#include "core/dr_debug.h"
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Creates a list of Vertices that represent a scaled circle
//####################################################################################
PointList DrEngineObject::createEllipseFromCircle(const DrPointF &center, const double &radius, const int &point_count) {
    std::vector<DrPointF> ellipse;
    int count = point_count;
    for (int i = 0; i < count; i++) {
        DrPointF point = Dr::RotatePointAroundOrigin(DrPointF(0, radius), DrPointF(0, 0), i * 360.0 / count);
        ellipse.push_back( point + center );
    }
    return ellipse;
}

//####################################################################################
//##    Add Shapes to Object
//####################################################################################
void DrEngineObject::addShapeBox(double width, double height) {
    width =  width *  abs(static_cast<double>(this->getScaleX()));
    height = height * abs(static_cast<double>(this->getScaleY()));
    cpShape *shape = cpBoxShapeNew(this->body, width, height, c_extra_radius);
    double   area = (width * height);
    applyShapeSettings(shape, area, Shape_Type::Box);
}
void DrEngineObject::addShapeBox(cpBB box) {
    cpShape *shape = cpBoxShapeNew2(this->body, box, 14);//c_extra_radius);
    double   area = ((box.r - box.l) * (box.t - box.b));
    applyShapeSettings(shape, area, Shape_Type::Box);
}
void DrEngineObject::addShapeBoxFromTexture(long texture_number, DrPointF extra_scale) {
    double width =  world()->getTexture(texture_number)->width() *  abs(extra_scale.x);
    double height = world()->getTexture(texture_number)->height() * abs(extra_scale.y);
    addShapeBox(width, height);
}

void DrEngineObject::addShapeCircle(double circle_radius, DrPointF shape_offset) {
    // Check if Circle, but not perfect square scale, if so, create with a polygon ellipse instead of a circle
    if (Dr::FuzzyCompare(abs(this->getScaleX()), abs(this->getScaleY())) == false) {
        std::vector<DrPointF> points = createEllipseFromCircle(shape_offset, circle_radius, 18);
        addShapePolygon(points);
    } else {
        double  radius = circle_radius * static_cast<double>(abs(this->getScaleX()));
        cpVect  offset = cpv(shape_offset.x, shape_offset.y);                                       // Offset of collision shape
        cpShape *shape = cpCircleShapeNew(this->body, radius, offset);
        double    area = cpAreaForCircle( 0, circle_radius );
        applyShapeSettings(shape, area, Shape_Type::Circle);
    }
}
void DrEngineObject::addShapeCircleFromTexture(long texture_number, double radius_multiplier, DrPointF extra_scale) {
    double width =  world()->getTexture(texture_number)->width() *  abs(extra_scale.x);
    double height = world()->getTexture(texture_number)->height() * abs(extra_scale.y);
    double radius = (width / 2.0) * radius_multiplier;

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  TEMP WATER
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //if (this->getThingType() != DrThingType::Character) {
    //    radius /= 2.0;
    //}

    double ratio =  height / (width + 0.0001);
    // Is Square, can be a nice circle
    if (Dr::IsCloseTo(width, height, 0.001)) {
        addShapeCircle(radius, DrPointF(0, 0));
    // Image is Rectangular, needs ellipse
    } else {
        std::vector<DrPointF> points = createEllipseFromCircle(DrPointF(0, 0), radius, 18);
        for (auto &point : points) {
            point.y *= ratio;
        }
        addShapePolygon(points);
    }
}

void DrEngineObject::addShapeTriangleFromTexture(long texture_number) {
    double width =  world()->getTexture(texture_number)->width();
    double height = world()->getTexture(texture_number)->height();
    std::vector<DrPointF> points;
    points.push_back(DrPointF(       0.0, +height/2.0));
    points.push_back(DrPointF(-width/2.0, -height/2.0));
    points.push_back(DrPointF( width/2.0, -height/2.0));
    addShapePolygon(points);
}

void DrEngineObject::addShapeSegment(DrPointF p1, DrPointF p2, double padding) {
    cpShape *shape = cpSegmentShapeNew(this->body, cpv(p1.x, p1.y), cpv(p2.x, p2.y), padding);
    double   area =  cpAreaForSegment(cpv(p1.x, p1.y), cpv(p2.x, p2.y), padding);
    applyShapeSettings(shape, area, Shape_Type::Segment);
}

void DrEngineObject::addShapePolygon(const std::vector<DrPointF> &points) {

    // Apply scale to points, verify Winding
    int old_point_count = static_cast<int>(points.size());
    double scale_x = static_cast<double>(this->getScaleX());
    double scale_y = static_cast<double>(this->getScaleY());
    std::vector<DrPointF> scaled_points;
    for (auto &point : points) {
        scaled_points.push_back( DrPointF(point.x * scale_x, point.y * scale_y) );
    }
    HullFinder::EnsureWindingOrientation(scaled_points, Winding_Orientation::CounterClockwise);

    // Copy polygon Vertices into a scaled cpVect array, and scaled TPPLPoly array
    TPPLPoly            poly;   poly.Init(old_point_count);
    std::vector<cpVect> verts;  verts.resize(old_point_count);
    for (int i = 0; i < old_point_count; i++) {
        double x = scaled_points[i].x;
        double y = scaled_points[i].y;
        poly[i].x = x;
        poly[i].y = y;
        verts[i] = cpv(x, y);
    }

    // Add polygon to list of concave polygons for debug drawing
    polygons.push_back(verts);

    // Calculate the convex hull of a given set of points. Returns the count of points in the hull. Result must be a pointer to a cpVect array
    // with at least count elements. If result is NULL, then verts array wil be reduced instead. first is an optional pointer to an integer to store
    // where the first vertex in the hull came from (i.e. verts[first] == result[0]). Tolerance (tol) is the allowed amount to shrink the hull when
    // simplifying it. A tolerance of 0.0 creates an exact hull.
    int first = 0;
    std::vector<cpVect> hull { }; hull.resize(old_point_count);                 // Temporary array for ConvexHull call below
    int new_point_count = cpConvexHull(old_point_count, verts.data(), hull.data(), &first, 0.0);

    // !!!!! #NOTE: For Chipmunk Polygon Shapes, points must be in Counter-Clockwise Winding !!!!!
    // Shape is convex or could not determine convex hull
    if (new_point_count == 0) {
        Dr::PrintDebug("Warning! From addShapePolygon()... Could not form convex hull!");
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
            std::vector<cpVect> verts { };
            verts.resize(static_cast<size_t>(poly.GetNumPoints()));
            for (int i = 0; i < poly.GetNumPoints(); i++) {
                verts[static_cast<size_t>(i)] = cpv(poly[i].x, poly[i].y);
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
    double friction = (this->getCustomFriction() < 0) ? world()->getFriction() : this->getCustomFriction();
    double bounce =   (this->getCustomBounce() < 0)   ? world()->getBounce()   : this->getCustomBounce();

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











