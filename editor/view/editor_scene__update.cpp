//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Very Important File! Updates Items in the GraphicsScene based on changes elsewhere
//
//
#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "editor/pixmap/pixmap.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "engine/enums_engine.h"
#include "engine/opengl/opengl.h"
#include "library/dr_math.h"
#include "model/project/project.h"
#include "model/project/project_asset.h"
#include "model/project/project_effect.h"
#include "model/project/project_font.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Called after some change has been made to items on another widget besides
//##    the QGraphicsView, updates the items in the scene
//####################################################################################
void DrScene::updateChangesInScene(std::list<DrSettings*> changed_items, std::list<long> property_keys) {
    if (changed_items.empty()) return;

    for (auto settings_item : changed_items) {
        DrType my_type = settings_item->getType();

        if (my_type == DrType::Thing) {
            updateItemInScene(settings_item, property_keys);

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
void DrScene::updateItemInScene(DrSettings* changed_item, std::list<long> property_keys) {
    DrThing *thing = dynamic_cast<DrThing*>(changed_item);
    if (thing == nullptr) return;
    DrItem  *item =  thing->getDrItem();
    if (item == nullptr)  return;

    // ***** Make sure this object is currently being shown in the DrScene before we try to update it
    if (thing->getParentStage()->getKey() != m_current_stage_key) return;

    // ***** Some local item variables, prepping for update
    QTransform transform;
    DrPointF position  = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
    DrPointF scale     = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Scale).toPointF();
    DrPointF size      = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Size).toPointF();
    double   angle     = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Rotation).toDouble();
    double   pre_angle = item->data(User_Roles::Rotation).toDouble();
    double   transform_scale_x, transform_scale_y;

    // ***** Turn off itemChange() signals to stop recursive calling
    bool flags_enabled_before = item->itemChangeFlagsEnabled();
    item->disableItemChangeFlags();

    // ***** Go through each property that we have been notified has changed and update as appropriately
    for (auto one_property : property_keys) {
        Properties  property    = static_cast<Properties>(one_property);
        DrProperty *dr_property = thing->findPropertyFromPropertyKey(one_property);
        DrVariant   new_value   = dr_property->getValue();


        switch (property) {

            case Properties::Hidden_Hide_From_Trees:
                if (new_value.toBool() == true) {
                    thing->setComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Item_Locked, true);
                }
                m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } , { Properties::Hidden_Item_Locked });
                break;
            case Properties::Hidden_Item_Locked:
                if (new_value.toBool() == false) {
                    if (thing->getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Hide_From_Trees).toBool() == true) {
                        thing->setComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Hide_From_Trees, false);
                    }
                }
                m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } , { Properties::Hidden_Hide_From_Trees });
                break;

            case Properties::Thing_Object_Physics_Type: {
                bool pretest = thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Velocity_X)->isEditable();
                Body_Type type = static_cast<Body_Type>(thing->getComponentPropertyValue(Components::Thing_Settings_Object, Properties::Thing_Object_Physics_Type).toInt());
                bool      test = (type == Body_Type::Kinematic || type == Body_Type::Dynamic) ? true : false;
                if (test != pretest) {
                    thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Velocity_X)->setEditable(test);
                    thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Velocity_Y)->setEditable(test);
                    thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Spin_Velocity)->setEditable(test);
                    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } ,
                                    { Properties::Thing_Velocity_X, Properties::Thing_Velocity_Y, Properties::Thing_Spin_Velocity });
                }

                bool pretest_velocity = thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Angle_Velocity)->isEditable();
                bool pretest_player =   thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Angle_Player)->isEditable();
                test = (type == Body_Type::Kinematic) ? true : false;
                if (test != pretest_velocity) {
                    thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Angle_Velocity)->setEditable(test);
                    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } , { Properties::Thing_Angle_Velocity });
                }
                if (test != pretest_player) {
                    thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Angle_Player)->setEditable(test);
                    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } , { Properties::Thing_Angle_Player });
                }
                break;
            }

            case Properties::Thing_Position:
                setPositionByOrigin(item, Position_Flags::Center, position.x, position.y);
                break;

            case Properties::Thing_Size:
            case Properties::Thing_Scale:
            case Properties::Thing_Rotation: {
                // ***** Keep Thing Square: Size / scale change override for Things that need to be square (light, etc)
                // ***** Also limits max size
                //       #KEYWORD: "keep square", "lock size", "same size", "limit size"
                bool    pretest = false;
                bool    has_max_x = !(Dr::FuzzyCompare(thing->maxSize().x, 0.0)),   has_max_y = !(Dr::FuzzyCompare(thing->maxSize().y, 0.0));
                bool    has_min_x = !(Dr::FuzzyCompare(thing->minSize().x, 0.0)),   has_min_y = !(Dr::FuzzyCompare(thing->minSize().y, 0.0));
                bool    keep_square = thing->keepSquare();

                if (keep_square || has_max_x || has_max_y || has_min_x || has_min_y) {
                    if (property == Properties::Thing_Size) {
                        if (has_max_x && size.x > thing->maxSize().x) size.x = thing->maxSize().x;
                        if (has_min_x && size.x < thing->minSize().x) size.x = thing->minSize().x;
                        if (has_max_y && size.y > thing->maxSize().y) size.y = thing->maxSize().y;
                        if (has_min_y && size.y < thing->minSize().y) size.y = thing->minSize().y;
                        if (keep_square) {
                            if (Dr::IsCloseTo(scale.y, size.y / item->getAssetHeight(), 0.001)) size.y = size.x;
                            else                                                                size.x = size.y;
                        }
                        pretest = true;

                    } else if (property == Properties::Thing_Scale) {
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
                if (property == Properties::Thing_Size) {
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
                if (property == Properties::Thing_Size || pretest) {
                    thing->setComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Scale, scale);
                    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } , { Properties::Thing_Scale });
                }
                if (property == Properties::Thing_Scale || pretest) {
                    thing->setComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Size, size);
                    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { thing } , { Properties::Thing_Size });
                }

                break;
            }

            case Properties::Thing_Z_Order:
                item->setZValue( thing->getZOrderWithSub() );
                break;


            case Properties::Thing_Filter_Brightness:
            case Properties::Thing_Filter_Contrast:
            case Properties::Thing_Filter_Saturation:
            case Properties::Thing_Filter_Hue:
            case Properties::Thing_Filter_Grayscale:
            case Properties::Thing_Filter_Negative:
            case Properties::Thing_Filter_Pixelation:
                item->applyFilters();
                break;

            case Properties::Thing_Fire_Color_1:
            case Properties::Thing_Fire_Color_2:
            case Properties::Thing_Fire_Color_Smoke:
            case Properties::Thing_Fire_Shape: {
                QColor cs = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_1).toUInt());
                QColor ce = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_2).toUInt());
                QColor sm = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_Smoke).toUInt());
                int  mask = item->getThing()->getComponentProperty(Components::Thing_Settings_Fire, Properties::Thing_Fire_Shape)->getValue().toInt();
                item->setPixmap( Dr::DrawFire(cs, ce, sm, static_cast<Fire_Mask>(mask)) );
                item->setBasePixmap(  item->pixmap() );
                item->setAssetWidth(  item->pixmap().width() );
                item->setAssetHeight( item->pixmap().height() );
                item->applyFilters();
                break;
            }

            case Properties::Thing_Fisheye_Color: {
                uint color =        item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Color).toUInt();
                item->setPixmap( Dr::DrawFisheye( QColor::fromRgba(color) ) );
                item->setBasePixmap(  item->pixmap() );
                item->setAssetWidth(  item->pixmap().width() );
                item->setAssetHeight( item->pixmap().height() );
                item->applyFilters();
                break;
            }

            case Properties::Thing_Light_Color:
            case Properties::Thing_Light_Cone_Start:
            case Properties::Thing_Light_Cone_End:
            case Properties::Thing_Light_Intensity:
            case Properties::Thing_Light_Blur: {
                QColor light_color = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Color).toUInt());
                float  cone_start =  item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_Start).toVector()[0].toFloat();
                float  cone_end =    item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_End).toVector()[0].toFloat();
                float  intensity =   item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Intensity).toFloat();
                float  blur =        item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Blur).toFloat();
                item->setPixmap( Dr::DrawLight(light_color, c_image_size, cone_start, cone_end, intensity, blur) );
                item->setAssetWidth(  item->pixmap().width() );
                item->setAssetHeight( item->pixmap().height() );
                break;
            }

            case Properties::Thing_Mirror_Start_Color:
            case Properties::Thing_Mirror_End_Color: {
                QColor cs = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Start_Color).toUInt());
                QColor ce = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_End_Color).toUInt());
                item->setPixmap( Dr::DrawMirror(cs, ce) );
                item->setBasePixmap(  item->pixmap() );
                item->setAssetWidth(  item->pixmap().width() );
                item->setAssetHeight( item->pixmap().height() );
                item->applyFilters();
                break;
            }

            case Properties::Thing_Swirl_Start_Color:
            case Properties::Thing_Swirl_Angle: {
                QColor cs = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Start_Color).toUInt());
                float  a =                   item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Angle).toFloat();
                item->setPixmap( Dr::DrawSwirl(cs, static_cast<double>(a)) );
                item->setBasePixmap(  item->pixmap() );
                item->setAssetWidth(  item->pixmap().width() );
                item->setAssetHeight( item->pixmap().height() );
                item->applyFilters();
                break;
            }

            case Properties::Thing_Water_Start_Color:
            case Properties::Thing_Water_End_Color: {
                QColor cs = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_Start_Color).toUInt());
                QColor ce = QColor::fromRgba(item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_End_Color).toUInt());
                item->setPixmap( Dr::DrawWater(cs, ce) );
                item->setBasePixmap(  item->pixmap() );
                item->setAssetWidth(  item->pixmap().width() );
                item->setAssetHeight( item->pixmap().height() );
                item->applyFilters();
                break;
            }

            case Properties::Thing_Text_User_Text: {
                std::string text = item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Text, Properties::Thing_Text_User_Text).toString();
                if (text == "") text = " ";
                item->setPixmap( m_editor_relay->currentProject()->findFontFromKey( item->getAsset()->getKey() )->createText( text ));
                item->setAssetWidth(  item->pixmap().width() );
                item->setAssetHeight( item->pixmap().height() );
                setPositionByOrigin(item, Position_Flags::Center, position.x, position.y);
                break;
            }

            default: ;
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






