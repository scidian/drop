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
//##    PAINT: Paints Collision Shapes of all Things in Stage
//####################################################################################
void DrView::paintCollisionShapes(QPainter &painter, DrStage *stage) {
    for (auto &thing_pair : stage->getThingMap()) {
        DrThing *thing = thing_pair.second;
        if (thing == nullptr) continue;

        // Figure out what color to make the debug shapes
        Collision_Type damage = Collision_Type::Damage_None;
        if (thing->getThingType() == DrThingType::Object) {
            long    damage_type = thing->getComponentPropertyValue(Components::Thing_Settings_Object, Properties::Thing_Object_Damage).toInt();
            damage = static_cast<Collision_Type>(damage_type);
        } else if (thing->getThingType() == DrThingType::Character) {
            damage = Collision_Type::Damage_Enemy;
        } else {
            continue;
        }
        DrColor color = DrOpenGL::objectDebugColor(damage, true, false);

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

        // ***** Process Auto Collision Shape
        if (shape == Collision_Shape::Image) {
            DrVariant shapes =  asset->getComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Image_Shape);
            DrPropertyCollision shape = boost::any_cast<DrPropertyCollision>(shapes.value());
            for (auto poly : shape.getPolygons()) {
                ///object->addShapePolygon(poly);
            }

        // ***** Process Circle Collision Shape
        } else if (shape == Collision_Shape::Circle) {
            QPointF top_left =  mapFromScene( QPointF( center.x - size.x/2, center.y - size.y/2) );
            QPointF bot_right = mapFromScene( QPointF( center.x + size.x/2, center.y + size.y/2) );
            painter.drawEllipse(QRectF(top_left, bot_right));

        } else if (shape == Collision_Shape::Square) {
            ///object->addShapeBoxFromTexture( asset->getIdleAnimationFirstFrameImageKey() );

        } else if (shape == Collision_Shape::Triangle) {
            ///object->addShapeTriangleFromTexture( asset->getIdleAnimationFirstFrameImageKey() );

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
        Collision_Type damage = Collision_Type::Damage_None;
        if (thing->getThingType() == DrThingType::Object) {
            long    damage_type = thing->getComponentPropertyValue(Components::Thing_Settings_Object, Properties::Thing_Object_Damage).toInt();
            damage = static_cast<Collision_Type>(damage_type);
        } else if (thing->getThingType() == DrThingType::Character) {
            damage = Collision_Type::Damage_Enemy;
        } else {
            continue;
        }
        DrColor color = DrOpenGL::objectDebugColor(damage, true, false);

        // Load Object Position
        DrPointF center = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
        QPointF  text_coord = mapFromScene(center.x, center.y);

        if (rect().contains( text_coord.toPoint() )) {
            // Health as a QPainterPath
            QPainterPath health;
            DrAsset    *asset =     m_project->findAssetFromKey(thing->getAssetKey());
                    if (asset == nullptr) continue;
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

















