//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Very Important File! Updates Items in the GraphicsScene based on changes elsewhere
//
//
#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/project/project.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "engine/enums_engine.h"
#include "engine/opengl/opengl.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_effect.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Called after some change has been made to items on another widget besides
//##    the QGraphicsView, updates the items in the scene
//####################################################################################
void DrScene::updateChangesInScene(std::list<DrSettings*> changed_items, std::list<ComponentProperty> component_property_pairs) {
    if (changed_items.empty()) return;

    for (auto settings_item : changed_items) {
        DrType my_type = settings_item->getType();

        if (my_type == DrType::Thing) {
            updateItemInScene(settings_item, component_property_pairs);

        } else if (my_type == DrType::Stage || my_type == DrType::World) {
            emit updateGrid();
        }

        //################ !!!!!!!!!!!!!!!!!!!!!!!
        //
        //      ROOM FOR MORE TYPES
        //
        //################ !!!!!!!!!!!!!!!!!!!!!!!
    }

    // Update selection box in case item position changed
    updateSelectionBox();
}


//####################################################################################
//##    Updates all items Z Values
//##        Needs to update them all since the sub ordering can change them all if something moves to back / front
//####################################################################################
void DrScene::updateItemZValues() {
    for (auto &item : items()) {
        DrItem *dritem = dynamic_cast<DrItem*>(item);
        if (dritem == nullptr) continue;
        if (dritem->isTempOnly()) continue;

        DrThing *thing = dritem->getThing();
        if (thing == nullptr) continue;

        // ***** Turn off itemChange() signals to stop recursive calling
        bool flags_enabled_before = dritem->itemChangeFlagsEnabled();
        dritem->disableItemChangeFlags();

        dritem->setZValue( thing->getZOrderWithSub() );

        // ***** Turn back on itemChange() signals
        if (flags_enabled_before) dritem->enableItemChangeFlags();
    }
}

//####################################################################################
//##    Updates the item in the scene based on the new property_keys
//####################################################################################
void DrScene::updateItemInScene(DrSettings *changed_item, std::list<ComponentProperty> component_property_pairs) {
    DrThing *thing = dynamic_cast<DrThing*>(changed_item);
    if (thing == nullptr) return;
    DrItem  *item =  thing->getDrItem();
    if (item == nullptr)  return;

    // ***** Make sure this object is currently being shown in the DrScene before we try to update it
    if (thing->getParentStage()->getKey() != m_current_stage_key) return;

    // ***** Some local item variables, prepping for update
    QTransform transform;
    DrPointF position  = thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Position).toPointF();
    DrPointF scale     = thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Scale).toPointF();
    DrPointF size      = thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Size).toPointF();
    double   angle     = thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Rotation).toDouble();
    double   pre_angle = item->data(User_Roles::Rotation).toDouble();
    double   transform_scale_x, transform_scale_y;

    // ***** Turn off itemChange() signals to stop recursive calling
    bool flags_enabled_before = item->itemChangeFlagsEnabled();
    item->disableItemChangeFlags();

    // ***** Go through each property that we have been notified has changed and update as appropriately
    for (auto one_component_property_pair : component_property_pairs) {
        std::string comp = one_component_property_pair.first;
        std::string prop = one_component_property_pair.second;
        DrProperty *dr_property = thing->getComponentProperty(one_component_property_pair);
        DrVariant   new_value   = dr_property->getValue();


        if (comp == Comps::Hidden_Settings && prop == Props::Hidden_Hide_From_Trees) {
            if (new_value.toBool() == true) {
                thing->setComponentPropertyValue(Comps::Hidden_Settings, Props::Hidden_Item_Locked, true);
            }
            m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } ,
                            { std::make_pair(comp, Props::Hidden_Item_Locked) });

        } else if (comp == Comps::Hidden_Settings && prop == Props::Hidden_Item_Locked) {
            if (new_value.toBool() == false) {
                if (thing->getComponentPropertyValue(Comps::Hidden_Settings, Props::Hidden_Hide_From_Trees).toBool() == true) {
                    thing->setComponentPropertyValue(Comps::Hidden_Settings, Props::Hidden_Hide_From_Trees, false);
                }
            }
            m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } ,
                            { std::make_pair(comp, Props::Hidden_Hide_From_Trees) });

        } else if (comp == Comps::Thing_Settings_Object && prop == Props::Thing_Object_Physics_Type) {
            bool pretest = thing->getComponentProperty(Comps::Thing_Movement, Props::Thing_Velocity_X)->isEditable();
            Body_Type type = static_cast<Body_Type>(thing->getComponentPropertyValue(Comps::Thing_Settings_Object, Props::Thing_Object_Physics_Type).toInt());
            bool      test = (type == Body_Type::Kinematic || type == Body_Type::Dynamic) ? true : false;
            if (test != pretest) {
                thing->getComponentProperty(Comps::Thing_Movement, Props::Thing_Velocity_X)->setEditable(test);
                thing->getComponentProperty(Comps::Thing_Movement, Props::Thing_Velocity_Y)->setEditable(test);
                thing->getComponentProperty(Comps::Thing_Movement, Props::Thing_Spin_Velocity)->setEditable(test);
                m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } ,
                                { std::make_pair(Comps::Thing_Movement, Props::Thing_Velocity_X),
                                  std::make_pair(Comps::Thing_Movement, Props::Thing_Velocity_Y),
                                  std::make_pair(Comps::Thing_Movement, Props::Thing_Spin_Velocity) });
            }
            bool pretest_velocity = thing->getComponentProperty(Comps::Thing_Movement, Props::Thing_Angle_Velocity)->isEditable();
            bool pretest_player =   thing->getComponentProperty(Comps::Thing_Movement, Props::Thing_Angle_Player)->isEditable();
            test = (type == Body_Type::Kinematic) ? true : false;
            if (test != pretest_velocity) {
                thing->getComponentProperty(Comps::Thing_Movement, Props::Thing_Angle_Velocity)->setEditable(test);
                m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } ,
                                { std::make_pair(Comps::Thing_Movement, Props::Thing_Angle_Velocity) });
            }
            if (test != pretest_player) {
                thing->getComponentProperty(Comps::Thing_Movement, Props::Thing_Angle_Player)->setEditable(test);
                m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } ,
                                { std::make_pair(Comps::Thing_Movement, Props::Thing_Angle_Player) });
            }

        } else if (comp == Comps::Thing_Transform && prop == Props::Thing_Position) {
            setPositionByOrigin(item, Position_Flags::Center, position.x, position.y);

        } else if ((comp == Comps::Thing_Transform && prop == Props::Thing_Size) ||
                   (comp == Comps::Thing_Transform && prop == Props::Thing_Scale) ||
                   (comp == Comps::Thing_Transform && prop == Props::Thing_Rotation)) {
            // ***** Keep Thing Square: Size / scale change override for Things that need to be square (light, etc)
            // ***** Also limits max size
            //       #KEYWORD: "keep square", "lock size", "same size", "limit size"
            bool    pretest = false;
            bool    has_max_x = !(Dr::FuzzyCompare(thing->maxSize().x, 0.0)),   has_max_y = !(Dr::FuzzyCompare(thing->maxSize().y, 0.0));
            bool    has_min_x = !(Dr::FuzzyCompare(thing->minSize().x, 0.0)),   has_min_y = !(Dr::FuzzyCompare(thing->minSize().y, 0.0));
            bool    keep_square = thing->keepSquare();
            if (keep_square || has_max_x || has_max_y || has_min_x || has_min_y) {
                if (prop == Props::Thing_Size) {
                    if (has_max_x && size.x > thing->maxSize().x) size.x = thing->maxSize().x;
                    if (has_min_x && size.x < thing->minSize().x) size.x = thing->minSize().x;
                    if (has_max_y && size.y > thing->maxSize().y) size.y = thing->maxSize().y;
                    if (has_min_y && size.y < thing->minSize().y) size.y = thing->minSize().y;
                    if (keep_square) {
                        if (Dr::IsCloseTo(scale.y, size.y / item->getAssetHeight(), 0.001)) size.y = size.x;
                        else                                                                size.x = size.y;
                    }
                    pretest = true;
                } else if (prop == Props::Thing_Scale) {
                    if (has_max_x && scale.x * item->getAssetWidth() >  thing->maxSize().x) scale.x = thing->maxSize().x / item->getAssetWidth();
                    if (has_min_x && scale.x * item->getAssetWidth() <  thing->minSize().x) scale.x = thing->minSize().x / item->getAssetWidth();
                    if (has_max_y && scale.y * item->getAssetHeight() > thing->maxSize().y) scale.y = thing->maxSize().y / item->getAssetHeight();
                    if (has_min_y && scale.y * item->getAssetHeight() < thing->minSize().y) scale.y = thing->minSize().y / item->getAssetHeight();
                    if (keep_square) {
                        if (Dr::IsCloseTo(size.y, scale.y * item->getAssetHeight(), 0.001)) scale.y = scale.x;
                        else                                                                scale.x = scale.y;
                    }
                    pretest = true;
                }
            }
            // ***** If property that changed was size, calculate the proper scale based on size
            if (prop == Props::Thing_Size) {
                scale.x = size.x / item->getAssetWidth();
                scale.y = size.y / item->getAssetHeight();
            // Otherwise calculate the size based on the scale
            } else {
                size.x = scale.x * item->getAssetWidth();
                size.y = scale.y * item->getAssetHeight();
            }
            // ***** Store the item transform data, one of which will have been new. Then recalculate the transform and move the object
            item->setData(User_Roles::Scale, QPointF(scale.x, scale.y) );
            item->setData(User_Roles::Rotation, angle );
            transform_scale_x = Dr::CheckScaleNotZero(scale.x);
            transform_scale_y = Dr::CheckScaleNotZero(scale.y);
            transform = QTransform().rotate(angle).scale(transform_scale_x, transform_scale_y);
            item->setTransform(transform);
            setPositionByOrigin(item, Position_Flags::Center, position.x, position.y);
            // ***** If size or scale was changed, update the other and update the widgets in the Inspector
            if (prop == Props::Thing_Size || pretest) {
                thing->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Scale, scale);
                m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } , { std::make_pair(comp, Props::Thing_Scale) });
            }
            if (prop == Props::Thing_Scale || pretest) {
                thing->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Size, size);
                m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } , { std::make_pair(comp, Props::Thing_Size) });
            }

        } else if (comp == Comps::Thing_Layering && prop == Props::Thing_Z_Order) {
            item->setZValue( thing->getZOrderWithSub() );

        } else if ((comp == Comps::Thing_Appearance && prop == Props::Thing_Filter_Brightness) ||
                   (comp == Comps::Thing_Appearance && prop == Props::Thing_Filter_Contrast) ||
                   (comp == Comps::Thing_Appearance && prop == Props::Thing_Filter_Saturation) ||
                   (comp == Comps::Thing_Appearance && prop == Props::Thing_Filter_Hue) ||
                   (comp == Comps::Thing_Appearance && prop == Props::Thing_Filter_Grayscale) ||
                   (comp == Comps::Thing_Appearance && prop == Props::Thing_Filter_Negative) ||
                   (comp == Comps::Thing_Appearance && prop == Props::Thing_Filter_Pixelation) ||
                   (comp == Comps::Thing_Appearance && prop == Props::Thing_Filter_Bitrate)) {
            item->applyFilters();

        } else if ((comp == Comps::Thing_Settings_Fire && prop == Props::Thing_Fire_Color_1) ||
                   (comp == Comps::Thing_Settings_Fire && prop == Props::Thing_Fire_Color_2) ||
                   (comp == Comps::Thing_Settings_Fire && prop == Props::Thing_Fire_Color_Smoke) ||
                   (comp == Comps::Thing_Settings_Fire && prop == Props::Thing_Fire_Shape)) {
            QColor cs = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_1).toUInt());
            QColor ce = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_2).toUInt());
            QColor sm = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_Smoke).toUInt());
            int  mask = item->getThing()->getComponentProperty(Comps::Thing_Settings_Fire, Props::Thing_Fire_Shape)->getValue().toInt();
            item->setPixmap( Dr::DrawFire(cs, ce, sm, static_cast<Fire_Mask>(mask)) );
            item->setBasePixmap(  item->pixmap() );
            item->setAssetWidth(  item->pixmap().width() );
            item->setAssetHeight( item->pixmap().height() );
            item->applyFilters();

        } else if (comp == Comps::Thing_Settings_Fisheye && prop == Props::Thing_Fisheye_Color) {
            uint color =        item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Fisheye, Props::Thing_Fisheye_Color).toUInt();
            item->setPixmap( Dr::DrawFisheye( QColor::fromRgba(color) ) );
            item->setBasePixmap(  item->pixmap() );
            item->setAssetWidth(  item->pixmap().width() );
            item->setAssetHeight( item->pixmap().height() );
            item->applyFilters();

        } else if ((comp == Comps::Thing_Settings_Light && prop == Props::Thing_Light_Color) ||
                   (comp == Comps::Thing_Settings_Light && prop == Props::Thing_Light_Cone_Start) ||
                   (comp == Comps::Thing_Settings_Light && prop == Props::Thing_Light_Cone_End) ||
                   (comp == Comps::Thing_Settings_Light && prop == Props::Thing_Light_Intensity) ||
                   (comp == Comps::Thing_Settings_Light && prop == Props::Thing_Light_Blur)) {
            QColor light_color = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Color).toUInt());
            float  cone_start =  item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Cone_Start).toVector()[0].toFloat();
            float  cone_end =    item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Cone_End).toVector()[0].toFloat();
            float  intensity =   item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Intensity).toFloat();
            float  blur =        item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Blur).toFloat();
            item->setPixmap( Dr::DrawLight(light_color, c_image_size, cone_start, cone_end, intensity, blur) );
            item->setAssetWidth(  item->pixmap().width() );
            item->setAssetHeight( item->pixmap().height() );

        } else if ((comp == Comps::Thing_Settings_Mirror && prop == Props::Thing_Mirror_Start_Color) ||
                   (comp == Comps::Thing_Settings_Mirror && prop == Props::Thing_Mirror_End_Color)) {
            QColor cs = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Start_Color).toUInt());
            QColor ce = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_End_Color).toUInt());
            item->setPixmap( Dr::DrawMirror(cs, ce) );
            item->setBasePixmap(  item->pixmap() );
            item->setAssetWidth(  item->pixmap().width() );
            item->setAssetHeight( item->pixmap().height() );
            item->applyFilters();

        } else if ((comp == Comps::Thing_Settings_Swirl && prop == Props::Thing_Swirl_Start_Color) ||
                   (comp == Comps::Thing_Settings_Swirl && prop == Props::Thing_Swirl_Angle)) {
            QColor cs = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Swirl, Props::Thing_Swirl_Start_Color).toUInt());
            float  a =                   item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Swirl, Props::Thing_Swirl_Angle).toFloat();
            item->setPixmap( Dr::DrawSwirl(cs, static_cast<double>(a)) );
            item->setBasePixmap(  item->pixmap() );
            item->setAssetWidth(  item->pixmap().width() );
            item->setAssetHeight( item->pixmap().height() );
            item->applyFilters();

        } else if ((comp == Comps::Thing_Settings_Water && prop == Props::Thing_Water_Start_Color) ||
                   (comp == Comps::Thing_Settings_Water && prop == Props::Thing_Water_End_Color)) {
            QColor cs = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Water, Props::Thing_Water_Start_Color).toUInt());
            QColor ce = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Water, Props::Thing_Water_End_Color).toUInt());
            item->setPixmap( Dr::DrawWater(cs, ce) );
            item->setBasePixmap(  item->pixmap() );
            item->setAssetWidth(  item->pixmap().width() );
            item->setAssetHeight( item->pixmap().height() );
            item->applyFilters();

        } else if (comp == Comps::Thing_Settings_Text && prop == Props::Thing_Text_User_Text) {
            std::string text = item->getThing()->getComponentPropertyValue(Comps::Thing_Settings_Text, Props::Thing_Text_User_Text).toString();
            if (text == "") text = " ";
            item->setPixmap( Dr::CreateText(m_editor_relay->currentProject()->findFontFromKey( item->getAsset()->getKey() ), text ));
            item->setAssetWidth(  item->pixmap().width() );
            item->setAssetHeight( item->pixmap().height() );
            setPositionByOrigin(item, Position_Flags::Center, position.x, position.y);
        }
    }


    // ***** If angle has changed, check if we should update selection box angle as well
    if (Dr::IsCloseTo(pre_angle, angle, .001) == false) {
        if (checkAllSelectedItemsHaveSameAngle()) {
            setSelectionAngle( angle );
        }
    }

    // ***** Turn back on itemChange() signals
    if (flags_enabled_before) item->enableItemChangeFlags();
}



























