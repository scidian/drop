//
//      Created by Stephens Nunnally on 1/14/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QPainter>

#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/view/editor_view.h"
#include "engine/opengl/opengl.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/properties/property_collision.h"


// Local Constants
const float c_ring_size = 200.f;            // Diameter of camera circle
const int   c_outline_width = 2;            // Pen width of QPainter


//####################################################################################
//##    PAINT: Paints Camera Boxes
//####################################################################################
void DrView::paintCameras(QPainter &painter, DrStage *stage) {

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

        // ***** Bounds Check, don't draw if not touching or inside of visible area
        QRect bounding_box = square.boundingRect().normalized().toRect();
        if ((this->rect().intersects(bounding_box) || this->rect().contains(bounding_box) || (bounding_box.width() == 0 && bounding_box.height() == 0)) &&
            (bounding_box.width() * 0.1 < this->width()) && (bounding_box.height() * 0.1 < this->height())) {

            // Set up QPainter
            DrColor color = Dr::white;
            QPen cosmetic_pen(QBrush(Dr::ToQColor(color)), c_outline_width);
            cosmetic_pen.setCosmetic(true);
            painter.setPen(cosmetic_pen);
            QColor brush_color = Dr::ToQColor(color);
            brush_color.setAlpha(64);
            painter.setBrush(QBrush(brush_color));

            // ***** Draw Camera Box
            painter.drawPolygon(square);

            // ***** Y Up Vector Arrow
            /**
            QPen outline_pen = QPen(QBrush(Qt::black), c_outline_width);
            outline_pen.setCosmetic(true);
            painter.setPen(outline_pen);
            if (cam.up == Up_Vector::Y) {
                QPointF p1 =    mapFromScene( cam_transform.map(QPointF(cam_position.x,      (cam_position.y-cam.lag.y/2) - 9)) );
                QPointF p2 =    mapFromScene( cam_transform.map(QPointF(cam_position.x - 12, (cam_position.y-cam.lag.y/2) + 9)) );
                QPointF p3 =    mapFromScene( cam_transform.map(QPointF(cam_position.x + 12, (cam_position.y-cam.lag.y/2) + 9)) );
                ///painter.drawLine(middle, p1);

                QPolygonF arrow;
                arrow << p1 << p2 << p3;
                painter.drawPolygon(arrow);
                }
            */

            // ***** Z Up Vector Circle
            /**
            if (cam.up == Up_Vector::Z) {
                QPointF c =     mapFromScene( cam_transform.map(QPointF(cam_position.x, cam_position.y)) );
                painter.drawEllipse(c, 10 * currentZoomLevel(), 10 * currentZoomLevel());
            }
            **/

            // ***** Rotation Circles
            painter.setPen(cosmetic_pen);
            painter.setBrush(Qt::NoBrush);
            float radius = c_ring_size * static_cast<float>(currentZoomLevel());
            ///radius *= (1.f / static_cast<float>(cam.zoom));

            // Calculate circle bounds
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

            // ***** Draw big X circle
            QLinearGradient gradient_x;
            QColor ring_x_back(Qt::white);
            QColor ring_x_front(Qt::white);
            float distance_x = (tl_x.y() > tr_x.y()) ? (tl_x.y() - tr_x.y()) : (tr_x.y() - tl_x.y());
            float percent_x = 1.0f - ((distance_x / (radius * 2.f)) / 2.f);
            /**
            if (tl_x.z() < tr_x.z()) {
                gradient_x.setStart(    QPointF(0, static_cast<double>(tl_x.y())));
                gradient_x.setFinalStop(QPointF(0, static_cast<double>(tr_x.y())));
            } else {
                gradient_x.setStart(    QPointF(0, static_cast<double>(tr_x.y())));
                gradient_x.setFinalStop(QPointF(0, static_cast<double>(tl_x.y())));
            }
            if (percent_x < 0.999f) {
                ring_x_back.setAlphaF(static_cast<double>(1.0f - percent_x));
                ring_x_front.setAlphaF(static_cast<double>(percent_x));
                gradient_x.setColorAt(1.0, ring_x_back);
                gradient_x.setColorAt(0.0, ring_x_front);
                QBrush brush_x(gradient_x);
                QPen pen_x;
                pen_x.setWidth(c_outline_width);
                pen_x.setBrush(brush_x);
                painter.setPen(pen_x);
            } else {
                painter.setPen(cosmetic_pen);
            }
            painter.translate(middle);
            painter.drawRect(QRectF(QPointF(double(tl_x.x()), double(tl_x.y())), QPointF(double(br_x.x()), double(br_x.y()))));
            ///painter.drawEllipse(QRectF(QPointF(double(tl_x.x()), double(tl_x.y())), QPointF(double(br_x.x()), double(br_x.y()))));
            painter.resetTransform();
            */

            // ***** Draw big Y circle
            QLinearGradient gradient_y;
            QColor ring_y_back(Qt::white);
            QColor ring_y_front(Qt::white);
            float distance_y = (tl_y.x() > tr_y.x()) ? (tl_y.x() - tr_y.x()) : (tr_y.x() - tl_y.x());
            float percent_y = 1.0f - ((distance_y / (radius * 2.f)) / 2.f);
            if (tl_y.z() < tr_y.z()) {
                gradient_y.setStart(    QPointF(static_cast<double>(tl_y.x()), 0));
                gradient_y.setFinalStop(QPointF(static_cast<double>(tr_y.x()), 0));
            } else {
                gradient_y.setStart(    QPointF(static_cast<double>(tr_y.x()), 0));
                gradient_y.setFinalStop(QPointF(static_cast<double>(tl_y.x()), 0));
            }
            if (percent_y < 0.999f) {
                ring_y_back.setAlphaF(static_cast<double>(1.0f - percent_y));
                ring_y_front.setAlphaF(static_cast<double>(percent_y));
                gradient_y.setColorAt(1.0, ring_y_back);
                gradient_y.setColorAt(0.0, ring_y_front);
                QBrush brush_y(gradient_y);
                QPen pen_y;
                pen_y.setWidth(c_outline_width);
                pen_y.setBrush(brush_y);
                painter.setPen(pen_y);
            } else {
                painter.setPen(cosmetic_pen);
            }
            painter.translate(middle);
            painter.drawEllipse(QRectF(QPointF(double(tl_y.x()), double(tl_y.y())), QPointF(double(br_y.x()), double(br_y.y()))));
            painter.resetTransform();

            // ***** Draw small camera circle
            DrColor cam_color = DrColor(Dr::purple).lighter(130);
            float percent_z = cam_point.z() / radius;
            if (percent_z < 0) {
                cam_color = cam_color.lighter(100 + static_cast<int>(abs(percent_z) * 100.f));
            } else {
                cam_color = cam_color.darker( 100 + static_cast<int>(abs(percent_z) * 50.f));
            }
            painter.setPen(QPen(QBrush(Dr::ToQColor(cam_color)), c_outline_width));
            painter.translate(middle + QPointF(double(cam_point.x()), double(cam_point.y())));

            double cam_diameter_x = ((static_cast<double>(radius) * cam.zoom) / 24.0) * static_cast<double>(percent_y);
            double cam_diameter_y = ((static_cast<double>(radius) * cam.zoom) / 24.0) * static_cast<double>(percent_x);
            double equalized_x = Dr::EqualizeAngle0to360( cam.rotation.x);
            double equalized_y = Dr::EqualizeAngle0to360(-cam.rotation.y);
            painter.rotate( (equalized_y > 180) ? equalized_x : -equalized_x);
            cam_diameter_y /= static_cast<double>(percent_x);
            painter.drawEllipse(QPointF(0, 0), cam_diameter_x, cam_diameter_y);
            painter.resetTransform();

        }   // End If visible
    }   // End For auto DrThing
}




















