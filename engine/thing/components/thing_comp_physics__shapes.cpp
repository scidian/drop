//
//      Created by Stephens Nunnally on 4/22/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/chipmunk/chipmunk.h"
#include "3rd_party/hull_finder.h"
#include "3rd_party/poly_partition.h"
#include "core/dr_debug.h"
#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_message.h"
#include "engine/engine_texture.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities_physics_2d/dr_asset.h"


//####################################################################################
//##    Creates a list of Vertices that represent a scaled circle
//####################################################################################
PointList ThingCompPhysics::createEllipseFromCircle(const DrPointF &center, const double &radius, const int &point_count) {
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
cpShape* ThingCompPhysics::addShapeBox(double width, double height) {
    width =  width *  abs(static_cast<double>(thing()->getScaleX()));
    height = height * abs(static_cast<double>(thing()->getScaleY()));
    cpShape *shape = cpBoxShapeNew(this->body, width, height, c_extra_radius);
    double   area = (width * height);
    return applyShapeSettings(shape, area, Shape_Type::Box);
}
cpShape* ThingCompPhysics::addShapeBox(cpBB box) {
    cpShape *shape = cpBoxShapeNew2(this->body, box, c_extra_radius);
    double   area = ((box.r - box.l) * (box.t - box.b));
    return applyShapeSettings(shape, area, Shape_Type::Box);
}
cpShape* ThingCompPhysics::addShapeBoxFromTexture(long texture_number, DrPointF extra_scale) {
    double width =  world()->getTexture(texture_number)->width() *  abs(extra_scale.x);
    double height = world()->getTexture(texture_number)->height() * abs(extra_scale.y);
    return addShapeBox(width, height);
}

cpShape* ThingCompPhysics::addShapeCircle(double circle_radius, DrPointF shape_offset) {
    // Check if Circle, but not perfect square scale, if so, create with a polygon ellipse instead of a circle
    if (Dr::FuzzyCompare(abs(thing()->getScaleX()), abs(thing()->getScaleY())) == false) {
        std::vector<DrPointF> points = createEllipseFromCircle(shape_offset, circle_radius, 18);
        return addShapePolygon(points);
    } else {
        double  radius = circle_radius * static_cast<double>(abs(thing()->getScaleX()));
        cpVect  offset = cpv(shape_offset.x, shape_offset.y);                                       // Offset of collision shape
        cpShape *shape = cpCircleShapeNew(this->body, radius, offset);
        double    area = cpAreaForCircle( 0, circle_radius );
        return applyShapeSettings(shape, area, Shape_Type::Circle);
    }
}
cpShape* ThingCompPhysics::addShapeCircleFromTexture(long texture_number, double radius_multiplier, DrPointF extra_scale) {
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
        return addShapeCircle(radius, DrPointF(0, 0));
    // Image is Rectangular, needs ellipse
    } else {
        std::vector<DrPointF> points = createEllipseFromCircle(DrPointF(0, 0), radius, 18);
        for (auto &point : points) {
            point.y *= ratio;
        }
        return addShapePolygon(points);
    }
}

cpShape* ThingCompPhysics::addShapeTriangleFromTexture(long texture_number) {
    double width =  world()->getTexture(texture_number)->width();
    double height = world()->getTexture(texture_number)->height();
    std::vector<DrPointF> points;
    points.push_back(DrPointF(       0.0, +height/2.0));
    points.push_back(DrPointF(-width/2.0, -height/2.0));
    points.push_back(DrPointF( width/2.0, -height/2.0));
    return addShapePolygon(points);
}

cpShape* ThingCompPhysics::addShapeSegment(DrPointF p1, DrPointF p2, double padding) {
    cpShape *shape = cpSegmentShapeNew(this->body, cpv(p1.x, p1.y), cpv(p2.x, p2.y), padding);
    double   area =  cpAreaForSegment(cpv(p1.x, p1.y), cpv(p2.x, p2.y), padding);
    return applyShapeSettings(shape, area, Shape_Type::Segment);
}

cpShape* ThingCompPhysics::addShapePolygon(const std::vector<DrPointF> &points) {
    // Apply scale to points, verify Winding
    int old_point_count = static_cast<int>(points.size());
    double scale_x = static_cast<double>(thing()->getScaleX());
    double scale_y = static_cast<double>(thing()->getScaleY());
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
        return applyShapeSettings(shape, area, Shape_Type::Polygon);

    // Shape is concave
    } else {
        // Use 3rd_party library Poly Partition to partition concave polygon into convex polygons
        std::list<TPPLPoly> testpolys, result;
        testpolys.push_back( poly );
        TPPLPartition pp;
        int partitioned = 0;
        // Method #1, !!!!! #NOTE: Fails on some images (like built in prefab ladder)
        if (partitioned == 0) partitioned = pp.ConvexPartition_OPT(&(*testpolys.begin()), &result);
        // Method #2, if OPT fails, fall back to HM
        if (partitioned == 0) partitioned = pp.ConvexPartition_HM(&testpolys, &result);

        for (auto &poly : result) {
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
    return nullptr;
}


//####################################################################################
//##    Applies Object Settings to Shape
//####################################################################################
cpShape* ThingCompPhysics::applyShapeSettings(cpShape *shape, double area, Shape_Type shape_type) {
    // Figrue out friction and bounce to use for this object
    double friction = (this->getCustomFriction() < 0) ? world()->getFriction() : this->getCustomFriction();
    double bounce =   (this->getCustomBounce() < 0)   ? world()->getBounce()   : this->getCustomBounce();

    // Setting mass of shape, body mass and moment of inertia are calculated by Chipmunk
    cpShapeSetMass( shape, area * c_mass_multiplier );
    cpShapeSetFriction( shape, friction );
    cpShapeSetElasticity( shape, bounce );                                          // Ideally between 0 and .99999
    cpShapeSetUserData( shape, thing() );                                           // Set UserData to DrEngineObject pointer

    // If we don't want the object to collide with other objects, set as sensor
    cpShapeSetSensor( shape, !(this->doesCollide()) );

    // Add shape to the list of shapes for this body
    this->shapes.push_back( shape );
    this->shape_type[shape] = shape_type;

    return shape;
}









