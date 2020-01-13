//
//      Created by Stephens Nunnally on 1/10/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPainter>

#include "editor/helper_library.h"
#include "editor/view/editor_view.h"
#include "engine/opengl/opengl.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/properties/property_collision.h"


//####################################################################################
//##    Finds color to use in Editor for DrThing debug shapes
//####################################################################################
DrColor editorThingDebugColor(DrThing *thing) {
    bool collides = true;
    Collision_Type damage = Collision_Type::Damage_None;

    if (thing->getThingType() == DrThingType::Object) {
        collides =          thing->getComponentPropertyValue(Components::Thing_Settings_Object, Properties::Thing_Object_Collide).toBool();
        long damage_type =  thing->getComponentPropertyValue(Components::Thing_Settings_Object, Properties::Thing_Object_Damage).toInt();
        damage = static_cast<Collision_Type>(damage_type);
    } else if (thing->getThingType() == DrThingType::Character) {
        damage =    Collision_Type::Damage_Enemy;
        collides =  true;
    } else {
        return Dr::orange;
    }
    DrColor color = DrOpenGL::objectDebugColor(damage, false);
    return ((collides) ? color : color.lighter());
}


//####################################################################################
//##    PAINT: Paints Collision Shapes of all Things in Stage
//####################################################################################
void DrView::paintCollisionShapes(QPainter &painter, DrStage *stage) {
    for (auto &thing_pair : stage->getThingMap()) {
        DrThing *thing = thing_pair.second;
        if (thing == nullptr) continue;

        // Figure out what color to make the debug shapes
        if (thing->getThingType() != DrThingType::Object && thing->getThingType() != DrThingType::Character) continue;
        DrColor color = editorThingDebugColor(thing);

        // Set up QPainter
        QPen cosmetic_pen(QBrush(QColor(color.red(), color.green(), color.blue())), 1);
        cosmetic_pen.setCosmetic(true);
        painter.setPen(cosmetic_pen);
        QColor brush_color = QColor(color.red(), color.green(), color.blue());
        brush_color.setAlpha(64);
        painter.setBrush(QBrush( brush_color));

        // Get underlying DrAsset
        DrAsset    *asset =     m_project->findAssetFromKey(thing->getAssetKey());
                if (asset == nullptr) continue;

        // Get collision shape type
        int shape_type =        asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Shape).toInt();
        Collision_Shape shape = static_cast<Collision_Shape>(shape_type);

        // Load Object Position
        DrPointF center =   thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
        DrPointF size =     thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Size).toPointF();
        DrPointF scale =    thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Scale).toPointF();
        double   angle =    thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Rotation).toDouble();

        // ***** Process Auto Collision Shape
        if (shape == Collision_Shape::Image) {
            DrVariant shapes =  asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Image_Shape);
            DrPropertyCollision shape = boost::any_cast<DrPropertyCollision>(shapes.value());
            for (auto poly : shape.getPolygons()) {
                QTransform t = QTransform().translate(center.x, center.y).rotate(angle).scale(scale.x, -scale.y);
                QPolygonF polygon;
                for (auto &point : poly) {
                    polygon.append( mapFromScene( t.map(QPointF(point.x, point.y)) ));
                }

                // Don't draw if not touching or inside of visible area
                QRect bounding_box = polygon.boundingRect().normalized().toRect();
                if ((this->rect().intersects(bounding_box) || this->rect().contains(bounding_box)) &&
                    (bounding_box.width() * 0.1 < this->width()) && (bounding_box.height() * 0.1 < this->height())) {
                    painter.drawPolygon(polygon);
                }
            }

        // ***** Process Circle Collision Shape
        } else if (shape == Collision_Shape::Circle) {
            // Circle
            if (Dr::FuzzyCompare(size.x, size.y) == true) {
                QPointF offset = mapFromScene(QPointF(center.x, center.y));
                double  radius = mapFromScene(QPointF(size.x/2, 0)).x() - mapFromScene(0, 0).x();
                QPointF top =    mapFromScene(QPointF(center.x, center.y - size.y/2));

                QTransform t = QTransform().translate(offset.x(), offset.y()).rotate(angle).translate(-offset.x(), -offset.y());
                QRect bounding_box = QRectF(QPointF(offset.x() - radius, offset.y() - radius), QPointF(offset.x() + radius, offset.y() + radius)).toRect();
                if ((this->rect().intersects(bounding_box) || this->rect().contains(bounding_box)) &&
                    (bounding_box.width() * 0.1 < this->width()) && (bounding_box.height() * 0.1 < this->height())) {
                    painter.drawEllipse(offset, radius, radius);                // Draw circle
                    painter.drawLine(t.map(top), offset);                       // Draw orientation line
                }

            // Ellipse
            } else {
                DrPointF offset(0, 0);
                double   radius_x = size.x/2;
                double   radius_y = size.y/2;
                double   curve = 0.925;

                QTransform t = QTransform().translate(center.x, center.y).rotate(angle);
                QPointF mid =   mapFromScene( t.map(QPointF(offset.x, offset.y)) );
                QPointF top =   mapFromScene( t.map(QPointF(offset.x, offset.y - radius_y)) );
                QPointF bot =   mapFromScene( t.map(QPointF(offset.x, offset.y + radius_y)) );
                QPointF left =  mapFromScene( t.map(QPointF(offset.x - radius_x, offset.y)) );
                QPointF right = mapFromScene( t.map(QPointF(offset.x + radius_x, offset.y)) );
                QPointF tl =    mapFromScene( t.map(QPointF((offset.x - radius_x) * curve, (offset.y - radius_y) * curve)) );
                QPointF tr =    mapFromScene( t.map(QPointF((offset.x + radius_x) * curve, (offset.y - radius_y) * curve)) );
                QPointF bl =    mapFromScene( t.map(QPointF((offset.x - radius_x) * curve, (offset.y + radius_y) * curve)) );
                QPointF br =    mapFromScene( t.map(QPointF((offset.x + radius_x) * curve, (offset.y + radius_y) * curve)) );

                QPainterPath path(top);
                path.quadTo(tl, left);
                path.quadTo(bl, bot);
                path.quadTo(br, right);
                path.quadTo(tr, top);

                // Don't draw if not touching or inside of visible area
                QRect bounding_box = path.boundingRect().normalized().toRect();
                if ((this->rect().intersects(bounding_box) || this->rect().contains(bounding_box)) &&
                    (bounding_box.width() * 0.1 < this->width()) && (bounding_box.height() * 0.1 < this->height())) {
                    painter.drawPath(path);                                 // Draw circle
                    painter.drawLine(top, mid);                             // Draw orientation line
                }
            }

        } else if (shape == Collision_Shape::Square || shape == Collision_Shape::Triangle) {
            QTransform t = QTransform().translate(center.x, center.y).rotate(angle).translate(-center.x, -center.y);

            QPolygonF polygon;
            if (shape == Collision_Shape::Square) {
                polygon.append( mapFromScene( t.map(QPointF(center.x-size.x/2, center.y-size.y/2)) ));
                polygon.append( mapFromScene( t.map(QPointF(center.x+size.x/2, center.y-size.y/2)) ));
                polygon.append( mapFromScene( t.map(QPointF(center.x+size.x/2, center.y+size.y/2)) ));
                polygon.append( mapFromScene( t.map(QPointF(center.x-size.x/2, center.y+size.y/2)) ));
            } else if (shape == Collision_Shape::Triangle) {
                polygon.append( mapFromScene( t.map(QPointF(center.x,          center.y-size.y/2)) ));
                polygon.append( mapFromScene( t.map(QPointF(center.x+size.x/2, center.y+size.y/2)) ));
                polygon.append( mapFromScene( t.map(QPointF(center.x-size.x/2, center.y+size.y/2)) ));
            }

            // Don't draw if not touching or inside of visible area
            QRect bounding_box = polygon.boundingRect().normalized().toRect();
            if ((this->rect().intersects(bounding_box) || this->rect().contains(bounding_box)) &&
                (bounding_box.width() * 0.1 < this->width()) && (bounding_box.height() * 0.1 < this->height())) {
                painter.drawPolygon(polygon);
            }
        }

    }
}


//####################################################################################
//##    Draws the health of each object using QPainter
//####################################################################################
void DrView::paintDebugHealth(QPainter &painter, DrStage *stage) {
    QFont health_font("Avenir", static_cast<int>(18 * m_zoom_scale));
    health_font.setBold(true);
    painter.setPen(Qt::NoPen);

    for (auto &thing_pair : stage->getThingMap()) {
        DrThing *thing = thing_pair.second;

        // Figure out what color to make the debug shapes
        if (thing->getThingType() != DrThingType::Object && thing->getThingType() != DrThingType::Character) continue;
        DrColor color = editorThingDebugColor(thing);

        // Get underlying DrAsset
        DrAsset    *asset =     m_project->findAssetFromKey(thing->getAssetKey());
                if (asset == nullptr) continue;

        // Load Object Position
        DrPointF center = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
        QPointF  text_coord = mapFromScene(center.x, center.y);

        if (rect().contains( text_coord.toPoint() )) {
            // Health as a QPainterPath
            QPainterPath health;
            QString hp = Dr::RemoveTrailingDecimals( asset->getComponentPropertyValue(Components::Asset_Health, Properties::Asset_Health_Health).toDouble(), 2 );
            health.addText(text_coord, health_font, hp);
            painter.setBrush( QBrush(QColor(color.red(), color.green(), color.blue())) );

            double fw = Dr::CheckFontWidth(health_font, hp);
            painter.translate( -(fw / 2.0), health.boundingRect().height() * 1.5);
            painter.drawPath(health);
            painter.resetTransform();
        }
    }
}


//####################################################################################
//##    PAINT: Paints Camera Boxes
//####################################################################################
void DrView::paintCameras(QPainter &painter, DrStage *stage) {
    QFont zoom_font("Avenir", static_cast<int>(36 * m_zoom_scale));
    zoom_font.setBold(true);

    // Loop through all Stage Things
    for (auto &thing_pair : stage->getThingMap()) {
        DrThing *thing = thing_pair.second;
        if (thing == nullptr) continue;
        if (thing->getThingType() != DrThingType::Character && thing->getThingType() != DrThingType::Camera) continue;

        // ***** Load Thing Info
        DrPointF center =   thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
        ///DrPointF size =     thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Size).toPointF();
        ///DrPointF scale =    thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Scale).toPointF();
        double   angle =    thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Rotation).toDouble();

        // ***** Load Camera Info
        QTransform  cam_transform;
        CamInfo     cam;
        DrPointF    cam_position;

        if (thing->getThingType() == DrThingType::Character) {
            cam = DrEngineWorld::loadCharacterCameraSettings(thing);
            cam_position = center + DrPointF(cam.position.x, -cam.position.y);
        } else if (thing->getThingType() == DrThingType::Camera) {
            cam_position = center;
            cam.lag = DrPointF(100, 100);
            cam.tilt = angle;
            cam.rotation =      thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Rotation).toPointF();
            cam.zoom =          thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Zoom).toDouble();
            int up_vector =     thing->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Up_Vector).toInt();
            cam.up =            static_cast<Up_Vector>(up_vector);
        }
        if (cam.match_angle) cam_transform = cam_transform.translate(center.x, center.y).rotate(angle).translate(-center.x, -center.y);
        cam_transform = cam_transform.translate(cam_position.x, cam_position.y).rotate(cam.tilt).translate(-cam_position.x, -cam_position.y);

        // ***** Camera Lag Box
        QPointF middle = mapFromScene( cam_transform.map(QPointF(cam_position.x, cam_position.y)) );
        QPointF tl =     mapFromScene( cam_transform.map(QPointF(cam_position.x-cam.lag.x/2, cam_position.y-cam.lag.y/2)) );
        QPointF tr =     mapFromScene( cam_transform.map(QPointF(cam_position.x+cam.lag.x/2, cam_position.y-cam.lag.y/2)) );
        QPointF br =     mapFromScene( cam_transform.map(QPointF(cam_position.x+cam.lag.x/2, cam_position.y+cam.lag.y/2)) );
        QPointF bl =     mapFromScene( cam_transform.map(QPointF(cam_position.x-cam.lag.x/2, cam_position.y+cam.lag.y/2)) );
        QPolygonF square;
        square << tl << tr << br << bl;

        // Don't draw if not touching or inside of visible area
        QRect bounding_box = square.boundingRect().normalized().toRect();
        if ((this->rect().intersects(bounding_box) || this->rect().contains(bounding_box) || (bounding_box.width() == 0 && bounding_box.height() == 0)) &&
            (bounding_box.width() * 0.1 < this->width()) && (bounding_box.height() * 0.1 < this->height())) {

            // Set up QPainter
            DrColor color = Dr::white;
            QPen cosmetic_pen(QBrush(Dr::ToQColor(color)), 2);
            cosmetic_pen.setCosmetic(true);
            painter.setPen(cosmetic_pen);
            QColor brush_color = Dr::ToQColor(color);
            brush_color.setAlpha(64);
            painter.setBrush(QBrush(brush_color));

            // ***** Draw Camera Box
            painter.drawPolygon(square);

            // ***** Y Up Vector Arrow
            QPen outline_pen = QPen(QBrush(Qt::black), 2);
            outline_pen.setCosmetic(true);
            painter.setPen(outline_pen);
            painter.setBrush(QBrush(Dr::ToQColor(color)));
            if (cam.up == Up_Vector::Y) {
                QPointF p1 =    mapFromScene( cam_transform.map(QPointF(cam_position.x,      (cam_position.y-cam.lag.y/2) - 9)) );
                QPointF p2 =    mapFromScene( cam_transform.map(QPointF(cam_position.x - 12, (cam_position.y-cam.lag.y/2) + 9)) );
                QPointF p3 =    mapFromScene( cam_transform.map(QPointF(cam_position.x + 12, (cam_position.y-cam.lag.y/2) + 9)) );
                painter.drawLine(middle, p1);

                QPolygonF arrow;
                arrow << p1 << p2 << p3;
                painter.drawPolygon(arrow);

            // ***** Z Up Vector Circle
            } else if (cam.up == Up_Vector::Z) {
                QPointF c =     mapFromScene( cam_transform.map(QPointF(cam_position.x, cam_position.y)) );
                painter.drawEllipse(c, 10 * currentZoomLevel(), 10 * currentZoomLevel());
            }          

            // ***** Zoom Level
            QPainterPath zoom;
            QString mag = "x" + Dr::RemoveTrailingDecimals( cam.zoom, 2 );
            zoom.addText(QPointF(0, 0), zoom_font, mag);
            double fw = Dr::CheckFontWidth(zoom_font, mag);
            zoom.translate( -(fw / 2.0), zoom.boundingRect().height() * 2.0);
            if (currentZoomLevel() < 1.0) {
                outline_pen = QPen(QBrush(Qt::black), 1);
                painter.setPen(outline_pen);
            }

            painter.translate(middle);
            painter.drawPath(zoom);
            painter.resetTransform();


            // ***** Rotation Circles
            painter.setPen(cosmetic_pen);
            painter.setBrush(Qt::NoBrush);
            float radius = 200.f * static_cast<float>(currentZoomLevel());

            QMatrix4x4 circle_x, circle_y;
            circle_y.rotate(static_cast<float>(-cam.rotation.y) + 0.0001f, 0.0f, 1.0f, 0.0f);
            circle_x.rotate(static_cast<float>( cam.rotation.x) + 0.0001f, 1.0f, 0.0f, 0.0f);

            QVector3D tl_x, tr_x, bl_x, br_x;
            QVector3D tl_y, tr_y, bl_y, br_y;
            tl_x = circle_x * QVector3D(-radius, 0, -radius);
            tr_x = circle_x * QVector3D(-radius, 0,  radius);
            br_x = circle_x * QVector3D( radius, 0,  radius);
            bl_x = circle_x * QVector3D( radius, 0, -radius);

            tl_y = circle_y * QVector3D(0,  radius, -radius);
            tr_y = circle_y * QVector3D(0,  radius,  radius);
            br_y = circle_y * QVector3D(0, -radius,  radius);
            bl_y = circle_y * QVector3D(0, -radius, -radius);

            QVector3D cam_point = QVector3D(0, 0, -radius);
            cam_point = circle_x * cam_point;
            cam_point = circle_y * cam_point;

            // Draw Big Circles
            painter.translate(middle);
            painter.drawEllipse(QRectF(QPointF(double(tl_x.x()), double(tl_x.y())), QPointF(double(br_x.x()), double(br_x.y()))));
            painter.drawEllipse(QRectF(QPointF(double(tl_y.x()), double(tl_y.y())), QPointF(double(br_y.x()), double(br_y.y()))));

            // Draw Small Cam circle
            painter.setPen(QPen(QBrush(Dr::ToQColor(Dr::purple)), 2));
            painter.drawEllipse(QPointF(double(cam_point.x()), double(cam_point.y())), static_cast<double>(radius / 20.f), static_cast<double>(radius / 20.f));
            painter.resetTransform();
        }   // End If visible

    }   // End For auto DrThing
}


























