//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor_item.h"
#include "editor_scene.h"
#include "enums_engine.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_font.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##        Called after some change has been made to items on another widget besides
//##        the QGraphicsView, updates the items in the scene
//####################################################################################
void DrScene::updateChangesInScene(QList<DrSettings*> changed_items, QList<long> property_keys) {
    if (changed_items.isEmpty()) return;

    for (auto settings_item : changed_items) {
        DrType my_type = settings_item->getType();

        if (my_type == DrType::Thing) {
            updateItemInScene(settings_item, property_keys);

        } else if (my_type == DrType::Stage || my_type == DrType::StartStage || my_type == DrType::World) {
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


// Updates the item in the scene based on the new property_keys
void DrScene::updateItemInScene(DrSettings* changed_item, QList<long> property_keys) {
    DrThing *thing = dynamic_cast<DrThing*>(changed_item);
    DrItem  *item =  thing->getDrItem();

    // ***** Make sure this object is currently being shown in the DrScene before we try to update it
    if (thing->getParentStage()->getKey() != m_current_stage_key) return;

    // ***** Some local item variables, prepping for update
    QTransform transform;
    QPointF position  = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
    QPointF scale     = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Scale).toPointF();
    QPointF size      = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Size).toPointF();
    double  angle     = thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Rotation).toDouble();
    double  pre_angle = item->data(User_Roles::Rotation).toDouble();
    double  transform_scale_x, transform_scale_y;
    QString text;

    // ***** Turn off itemChange() signals to stop recursive calling
    item->disableItemChangeFlags();

    // ***** Go through each property that we have been notified has changed and update as appropriately
    for (auto one_property : property_keys) {
        Properties property = static_cast<Properties>(one_property);
        QVariant new_value  = thing->findPropertyFromPropertyKey(one_property)->getValue();

        Body_Type type;
        bool pretest, test;
        switch (property) {
            case Properties::Thing_Physics_Type:
                pretest = thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Velocity_X)->isEditable();
                type = static_cast<Body_Type>(thing->getComponentPropertyValue(Components::Thing_Settings_Object, Properties::Thing_Physics_Type).toInt());
                test = (type == Body_Type::Kinematic || type == Body_Type::Dynamic) ? true : false;
                if (test != pretest) {
                    thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Velocity_X)->setEditable(test);
                    thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Velocity_Y)->setEditable(test);
                    thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Spin_Velocity)->setEditable(test);
                    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Scene_View, { thing } ,
                                    { Properties::Thing_Velocity_X, Properties::Thing_Velocity_Y, Properties::Thing_Spin_Velocity });
                }

                pretest = thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Angle_Velocity)->isEditable();
                test = (type == Body_Type::Kinematic) ? true : false;
                if (test != pretest) {
                    thing->getComponentProperty(Components::Thing_Movement, Properties::Thing_Angle_Velocity)->setEditable(test);
                    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Scene_View, { thing } , { Properties::Thing_Angle_Velocity });
                }
                break;

            case Properties::Thing_Position:
                setPositionByOrigin(item, Position_Flags::Center, position.x(), position.y());
                break;

            case Properties::Thing_Size:
            case Properties::Thing_Scale:
            case Properties::Thing_Rotation:

                // If property that changed was size, calculate the proper scale based on size
                if (property == Properties::Thing_Size) {
                        scale.setX( size.x() / item->getAssetWidth()  );
                        scale.setY( size.y() / item->getAssetHeight() );
                // Otherwise calculate the size based on the scale
                } else {
                        size.setX(  scale.x() * item->getAssetWidth() );
                        size.setY(  scale.y() * item->getAssetHeight() );
                }

                // Store the item transform data, one of which will have been new. Then recalculate the transform and move the object

                item->setData(User_Roles::Scale, scale );
                item->setData(User_Roles::Rotation, angle );
                transform_scale_x = Dr::CheckScaleNotZero(scale.x());
                transform_scale_y = Dr::CheckScaleNotZero(scale.y());
                transform = QTransform().rotate(angle).scale(transform_scale_x, transform_scale_y);
                item->setTransform(transform);
                setPositionByOrigin(item, Position_Flags::Center, position.x(), position.y());

                // If size or scale was changed, update the other and update the widgets in the Inspector
                if (property == Properties::Thing_Size) {
                    thing->setComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Scale, scale);
                    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Scene_View, { thing } , { Properties::Thing_Scale });
                }
                if (property == Properties::Thing_Scale) {
                    thing->setComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Size, size);
                    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Scene_View, { thing } , { Properties::Thing_Size });
                }

                break;

            case Properties::Thing_Z_Order:
                item->setZValue(new_value.toDouble());
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

            case Properties::Thing_Text_User_Text:
                text = item->getThing()->getComponentPropertyValue(Components::Thing_Settings_Text, Properties::Thing_Text_User_Text).toString();
                if (text == "") text = " ";
                item->setPixmap( m_editor_relay->currentProject()->getDrFont( item->getAsset()->getSourceKey() )->createText( text ));
                item->setAssetWidth(  item->pixmap().width() );
                item->setAssetHeight( item->pixmap().height() );
                setPositionByOrigin(item, Position_Flags::Center, position.x(), position.y());
                break;

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
    item->enableItemChangeFlags();
}






