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
        painter.setPen( cosmetic_pen );
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

















