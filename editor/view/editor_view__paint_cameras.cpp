//
//      Created by Stephens Nunnally on 1/14/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QPainter>

#include "core/colors/colors.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/view/editor_view.h"
#include "engine/engine_camera.h"
#include "engine/opengl/opengl.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/properties/property_collision.h"


//####################################################################################
//##    Local Constants
//####################################################################################
const float     c_ring_size =       250.f;              // Diameter of Camera Circle
const int       c_outline_width =   2;                  // Pen width of QPainter
const double    c_square_radius =   4.0;                // Size of square / circle corner and side handles
const double    c_edge_radius =     3.0;                // Size of frame edges



//####################################################################################
//##    Paints corners of Character Camera Frame
//####################################################################################
void paintCornerSquare(QPainter &painter, QPointF location) {
    QRectF square(location - QPointF(c_square_radius, c_square_radius), location + QPointF(c_square_radius, c_square_radius));
    QPen corner_pen = QPen(QBrush(Dr::ToQColor(Dr::GetColor(Window_Colors::Seperator))), 1);
         corner_pen.setCosmetic(true);
    painter.setPen(corner_pen);
    QLinearGradient corner_fade(square.topLeft(), square.bottomRight());
    corner_fade.setColorAt(0.0, Dr::ToQColor(Dr::GetColor(Window_Colors::Highlight).lighter(150)));
    corner_fade.setColorAt(1.0, Dr::ToQColor(Dr::GetColor(Window_Colors::Highlight).darker( 200)));
    painter.setBrush(corner_fade);
    painter.drawRect(square);
}

void paintFrameEdge(QPainter &painter, QPointF point1, QPointF point2) {
    QPolygonF poly;
    poly << point1 << point2;
    QRectF box = poly.boundingRect();
    QRectF square(box.topLeft() - QPointF(c_edge_radius, c_edge_radius), box.bottomRight() + QPointF(c_edge_radius, c_edge_radius));
    painter.drawRect(square);
}


//####################################################################################
//##    PAINT: Paints Camera Boxes
//####################################################################################
void DrView::paintCameras(QPainter &painter, DrStage *stage) {
    // Reset camera data
    m_cam_data.clear();

    // Loop through all Stage Things
    for (auto &thing_pair : stage->getThingMap()) {
        DrThing *thing = thing_pair.second;
        if (thing == nullptr) continue;
        if (thing->getThingType() != DrThingType::Character && thing->getThingType() != DrThingType::Camera) continue;

        // ***** Load Thing Info
        DrPointF center =   thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
        double   angle =    thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Rotation).toDouble();

        // ***** Load Camera Info
        Cam_Info    cam;
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

        // ***** Figure out Camera Lag Box
        QPointF middle = mapFromScene( QPointF(cam_position.x, cam_position.y) );
        QPointF tl_b =   mapFromScene( QPointF(cam_position.x-cam.lag.x/2, cam_position.y-cam.lag.y/2) );
        QPointF tr_b =   mapFromScene( QPointF(cam_position.x+cam.lag.x/2, cam_position.y-cam.lag.y/2) );
        QPointF br_b =   mapFromScene( QPointF(cam_position.x+cam.lag.x/2, cam_position.y+cam.lag.y/2) );
        QPointF bl_b =   mapFromScene( QPointF(cam_position.x-cam.lag.x/2, cam_position.y+cam.lag.y/2) );
        QPolygonF square;
        square << tl_b << tr_b << br_b << bl_b;

        // ***** Bounds Check, don't draw if not touching or inside of visible area
        QRect bounding_box = square.boundingRect().normalized().toRect();
        if ((this->rect().intersects(bounding_box) || this->rect().contains(bounding_box) || (bounding_box.width() == 0 && bounding_box.height() == 0)) &&
            (bounding_box.width() * 0.1 < this->width()) && (bounding_box.height() * 0.1 < this->height())) {

            // ***** Draw Lag Box
            bool aliasing = painter.renderHints() & QPainter::Antialiasing;
            painter.setRenderHint(QPainter::Antialiasing, false);
            QColor box_fill = Dr::ToQColor(Dr::GetColor(Window_Colors::Highlight));
                   box_fill.setAlpha(64);

            std::map<Frame_Edge, QBrush> frame_brushes;
            frame_brushes[Frame_Edge::Normal] =    QBrush(Dr::ToQColor(0xFF75F42E));    // Green
            frame_brushes[Frame_Edge::Blocking] =  QBrush(Dr::ToQColor(0xFFFFD71E));    // Yellow
            frame_brushes[Frame_Edge::Death] =     QBrush(Dr::ToQColor(0xFFEF1C16));    // Red

            // Adjustable Box Frame
            if (thing->getThingType() == DrThingType::Character) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QBrush(box_fill));
                painter.drawPolygon(square);

                QPen edge_pen = QPen(QBrush(Dr::ToQColor(Dr::GetColor(Window_Colors::Seperator))), 1);
                     edge_pen.setCosmetic(true);
                painter.setPen(edge_pen);
                painter.setBrush(frame_brushes[cam.frame_top]);     paintFrameEdge(painter, tl_b, tr_b);
                painter.setBrush(frame_brushes[cam.frame_right]);   paintFrameEdge(painter, tr_b, br_b);
                painter.setBrush(frame_brushes[cam.frame_bottom]);  paintFrameEdge(painter, br_b, bl_b);
                painter.setBrush(frame_brushes[cam.frame_left]);    paintFrameEdge(painter, bl_b, tl_b);

                paintCornerSquare(painter, tl_b);
                paintCornerSquare(painter, tr_b);
                paintCornerSquare(painter, br_b);
                paintCornerSquare(painter, bl_b);

            // Non-Adjustable Box
            } else {
                ///QPen normal_box = QPen(QBrush(Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Light))), c_outline_width);
                ///painter.setPen(normal_box);
                ///painter.setBrush(QBrush(box_fill));
                ///painter.drawPolygon(square);
            }
            painter.setRenderHint(QPainter::Antialiasing, aliasing);


            // ***** Rotational Circles
            painter.setBrush(Qt::NoBrush);
            float radius = c_ring_size * static_cast<float>(currentZoomLevel());
            ///radius *= (1.f / static_cast<float>(cam.zoom));

            // Calculate circle bounds
            QMatrix4x4 circle_x, circle_y, circle_z;
            circle_y.rotate(static_cast<float>(-cam.rotation.y) + 0.0001f, 0.0f, 1.0f, 0.0f);
            circle_x.rotate(static_cast<float>( cam.rotation.x) + 0.0001f, 1.0f, 0.0f, 0.0f);

            double z_angle = ((cam.match_angle) ? angle : 0.0) + cam.tilt;
            circle_z.rotate(static_cast<float>( z_angle       ) + 0.0001f, 0.0f, 0.0f, 1.0f);

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

            // ***** Draw Big X Circle
            /**
            QLinearGradient gradient_x;
            QColor ring_x_back(Qt::white);
            QColor ring_x_front(Qt::white);
            float distance_x = (tl_x.y() > tr_x.y()) ? (tl_x.y() - tr_x.y()) : (tr_x.y() - tl_x.y());
            float percent_x = 1.0f - ((distance_x / (radius * 2.f)) / 2.f);
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
                QPen pen_zero(Dr::ToQColor(Dr::white));
                pen_zero.setWidth(c_outline_width);
                painter.setPen(pen_zero);
            }
            painter.translate(middle);
            painter.drawRect(QRectF(QPointF(double(tl_x.x()), double(tl_x.y())), QPointF(double(br_x.x()), double(br_x.y()))));
            ///painter.drawEllipse(QRectF(QPointF(double(tl_x.x()), double(tl_x.y())), QPointF(double(br_x.x()), double(br_x.y()))));
            painter.resetTransform();
            */

            // ***** Draw Big Y Circle
            QLinearGradient gradient_y;
            QColor ring_y_back( Qt::white);
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
                QPen pen_zero(Dr::ToQColor(Dr::white));
                pen_zero.setWidth(c_outline_width);
                painter.setPen(pen_zero);
            }
            painter.translate(middle);
            painter.drawEllipse(QRectF(QPointF(double(tl_y.x()), double(tl_y.y())), QPointF(double(br_y.x()), double(br_y.y()))));
            painter.resetTransform();

            // ***** Set Up Painter for Camera
            ///DrColor cam_color = DrColor(Dr::purple).lighter(125);
            ///DrColor cam_color(0xFF7E28AA);
            DrColor cam_color = Dr::GetColor(Window_Colors::Icon_Dark);
            if (m_cam_mouse_over == thing || m_cam_selected == thing) cam_color = cam_color.lighter(150);
            float percent_z = cam_point.z() / radius;
            if (percent_z < 0) {
                cam_color = cam_color.lighter(100 + static_cast<int>(abs(percent_z) * 50.f));
            } else {
                cam_color = cam_color.darker( 100 + static_cast<int>(abs(percent_z) * 30.f));
            }
            painter.setPen(QPen(QBrush(Dr::ToQColor(cam_color)), 1));
            painter.translate(middle);

            // Old Small Circle Drawing
            /**
            painter.translate(middle + QPointF(double(cam_point.x()), double(cam_point.y())));
            double cam_diameter_x = ((static_cast<double>(radius) * cam.zoom) / 24.0) * static_cast<double>(percent_y);
            double cam_diameter_y = ((static_cast<double>(radius) * cam.zoom) / 24.0) * static_cast<double>(percent_x);
            double equalized_x = Dr::EqualizeAngle0to360( cam.rotation.x);
            double equalized_y = Dr::EqualizeAngle0to360(-cam.rotation.y);
            painter.rotate( (equalized_y > 180) ? equalized_x : -equalized_x);
            cam_diameter_y /= static_cast<double>(percent_x);
            painter.drawEllipse(QPointF(0, 0), cam_diameter_x, cam_diameter_y);
            painter.resetTransform();
            */

            float rad = ((radius * static_cast<float>(cam.zoom)) / 32.f) + 10.f;
            float curve =  0.925f;
            float curve2 = 0.71f;

            // ***** Circle
            QVector3D top_c =   circle_y * (circle_x * QVector3D(   0, -rad, -radius));
            QVector3D bot_c =   circle_y * (circle_x * QVector3D(   0,  rad, -radius));
            QVector3D left_c =  circle_y * (circle_x * QVector3D(-rad,    0, -radius));
            QVector3D right_c = circle_y * (circle_x * QVector3D( rad,    0, -radius));
            QVector3D tl_c =    circle_y * (circle_x * QVector3D(-rad*curve, -rad*curve, -radius));
            QVector3D tr_c =    circle_y * (circle_x * QVector3D( rad*curve, -rad*curve, -radius));
            QVector3D br_c =    circle_y * (circle_x * QVector3D( rad*curve,  rad*curve, -radius));
            QVector3D bl_c =    circle_y * (circle_x * QVector3D(-rad*curve,  rad*curve, -radius));
            QVector3D mid_c =   circle_y * (circle_x * QVector3D(   0,    0, -radius - (rad*1.0f)));
            QPointF top =   QPointF(static_cast<double>(top_c.x()),     static_cast<double>(top_c.y()));
            QPointF bot =   QPointF(static_cast<double>(bot_c.x()),     static_cast<double>(bot_c.y()));
            QPointF left =  QPointF(static_cast<double>(left_c.x()),    static_cast<double>(left_c.y()));
            QPointF right = QPointF(static_cast<double>(right_c.x()),   static_cast<double>(right_c.y()));
            QPointF tl =    QPointF(static_cast<double>(tl_c.x()),      static_cast<double>(tl_c.y()));
            QPointF tr =    QPointF(static_cast<double>(tr_c.x()),      static_cast<double>(tr_c.y()));
            QPointF br =    QPointF(static_cast<double>(br_c.x()),      static_cast<double>(br_c.y()));
            QPointF bl =    QPointF(static_cast<double>(bl_c.x()),      static_cast<double>(bl_c.y()));
            QPointF mid =   QPointF(static_cast<double>(mid_c.x()),     static_cast<double>(mid_c.y()));

            QPainterPath path(top);
            path.quadTo(tl, left);
            path.quadTo(bl, bot);
            path.quadTo(br, right);
            path.quadTo(tr, top);
            painter.drawPath(path);

            // ***** Cone Lines
            QVector3D tl_c2 = circle_y * (circle_x * (circle_z * QVector3D(-rad*curve2, -rad*curve2, -radius)));
            QVector3D tr_c2 = circle_y * (circle_x * (circle_z * QVector3D( rad*curve2, -rad*curve2, -radius)));
            QVector3D br_c2 = circle_y * (circle_x * (circle_z * QVector3D( rad*curve2,  rad*curve2, -radius)));
            QVector3D bl_c2 = circle_y * (circle_x * (circle_z * QVector3D(-rad*curve2,  rad*curve2, -radius)));
            QPointF tl2 = QPointF(static_cast<double>(tl_c2.x()), static_cast<double>(tl_c2.y()));
            QPointF tr2 = QPointF(static_cast<double>(tr_c2.x()), static_cast<double>(tr_c2.y()));
            QPointF br2 = QPointF(static_cast<double>(br_c2.x()), static_cast<double>(br_c2.y()));
            QPointF bl2 = QPointF(static_cast<double>(bl_c2.x()), static_cast<double>(bl_c2.y()));

            painter.drawLine(mid, tl2);
            painter.drawLine(mid, tr2);
            painter.drawLine(mid, br2);
            painter.drawLine(mid, bl2);

            // ***** Camera Housing
            QVector3D box1_tl = circle_y * (circle_x * (circle_z * QVector3D(-rad, -rad, -radius - (rad*1.0f))));
            QVector3D box1_tr = circle_y * (circle_x * (circle_z * QVector3D( rad, -rad, -radius - (rad*1.0f))));
            QVector3D box1_br = circle_y * (circle_x * (circle_z * QVector3D( rad,  rad, -radius - (rad*1.0f))));
            QVector3D box1_bl = circle_y * (circle_x * (circle_z * QVector3D(-rad,  rad, -radius - (rad*1.0f))));
            QVector3D box2_tl = circle_y * (circle_x * (circle_z * QVector3D(-rad, -rad, -radius - (rad*3.0f))));
            QVector3D box2_tr = circle_y * (circle_x * (circle_z * QVector3D( rad, -rad, -radius - (rad*3.0f))));
            QVector3D box2_br = circle_y * (circle_x * (circle_z * QVector3D( rad,  rad, -radius - (rad*3.0f))));
            QVector3D box2_bl = circle_y * (circle_x * (circle_z * QVector3D(-rad,  rad, -radius - (rad*3.0f))));
            QPointF b1_tl = QPointF(static_cast<double>(box1_tl.x()),   static_cast<double>(box1_tl.y()));
            QPointF b1_tr = QPointF(static_cast<double>(box1_tr.x()),   static_cast<double>(box1_tr.y()));
            QPointF b1_br = QPointF(static_cast<double>(box1_br.x()),   static_cast<double>(box1_br.y()));
            QPointF b1_bl = QPointF(static_cast<double>(box1_bl.x()),   static_cast<double>(box1_bl.y()));
            QPointF b2_tl = QPointF(static_cast<double>(box2_tl.x()),   static_cast<double>(box2_tl.y()));
            QPointF b2_tr = QPointF(static_cast<double>(box2_tr.x()),   static_cast<double>(box2_tr.y()));
            QPointF b2_br = QPointF(static_cast<double>(box2_br.x()),   static_cast<double>(box2_br.y()));
            QPointF b2_bl = QPointF(static_cast<double>(box2_bl.x()),   static_cast<double>(box2_bl.y()));

            QPolygonF box1, box2;
            box1 << b1_tl << b1_tr << b1_br << b1_bl;
            box2 << b2_tl << b2_tr << b2_br << b2_bl;
            painter.drawPolygon(box1);
            painter.drawPolygon(box2);
            painter.drawLine(b1_tl, b2_tl);
            painter.drawLine(b1_tr, b2_tr);
            painter.drawLine(b1_br, b2_br);
            painter.drawLine(b1_bl, b2_bl);

            painter.resetTransform();


            // ***** Store Camera Housing Bounding Rect for Mouse Interaction
            box1.translate(middle);
            box2.translate(middle);

            Camera_Data cam_data;
            cam_data.thing =        thing;
            cam_data.z_order =      thing->getZOrderWithSub() + static_cast<double>(mid_c.z());
            cam_data.view_rect =    box1.boundingRect().united(box2.boundingRect()).toRect();
            m_cam_data[thing] = cam_data;


        }   // End If visible
    }   // End For auto DrThing
}




















