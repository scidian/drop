//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor_item.h"
#include "editor_scene.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_font.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_object.h"
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

        if (my_type == DrType::Object) {
            updateItemInScene(settings_item, property_keys);

        } else if (my_type == DrType::Stage || my_type == DrType::StartStage) {
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
    DrObject *object = dynamic_cast<DrObject*>(changed_item);
    DrItem   *item =   object->getDrItem();

    // ***** Make sure this object is currently being shown in the DrScene before we try to update it
    if (object->getParentStage()->getKey() != m_current_stage_key) return;

    // ***** Some local item variables, prepping for update
    QTransform transform;
    QPointF position  = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Position).toPointF();
    QPointF scale     = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Scale).toPointF();
    QPointF size      = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Size).toPointF();
    double  angle     = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Rotation).toDouble();
    double  pre_angle = item->data(User_Roles::Rotation).toDouble();
    double  transform_scale_x, transform_scale_y;
    QString text;

    // ***** Turn off itemChange() signals to stop recursive calling
    item->disableItemChangeFlags();

    // ***** Go through each property that we have been notified has changed and update as appropriately
    for (auto one_property : property_keys) {
        Properties property = static_cast<Properties>(one_property);

        QVariant new_value  = object->findPropertyFromPropertyKey(one_property)->getValue();

        switch (property) {
            case Properties::Object_Position:
                setPositionByOrigin(item, Position_Flags::Center, position.x(), position.y());
                break;

            case Properties::Object_Size:
            case Properties::Object_Scale:
            case Properties::Object_Rotation:

                // If property that changed was size, calculate the proper scale based on size
                if (property == Properties::Object_Size) {
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

                // If size or scale was changed, update the other and update the widgets in the object inspector
                if (property == Properties::Object_Size) {
                    object->setComponentPropertyValue(Components::Object_Transform, Properties::Object_Scale, scale);
                    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Scene_View, { object } , { Properties::Object_Scale });
                }
                if (property == Properties::Object_Scale) {
                    object->setComponentPropertyValue(Components::Object_Transform, Properties::Object_Size, size);
                    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Scene_View, { object } , { Properties::Object_Size });
                }

                break;

            case Properties::Object_Z_Order:
                item->setZValue(new_value.toDouble());
                break;


            case Properties::Object_Filter_Brightness:
            case Properties::Object_Filter_Contrast:
            case Properties::Object_Filter_Saturation:
            case Properties::Object_Filter_Hue:
            case Properties::Object_Filter_Grayscale:
            case Properties::Object_Filter_Negative:
                item->applyFilters();
                break;

            case Properties::Object_Text_User_Text:
                text = item->getObject()->getComponentPropertyValue(Components::Object_Settings_Text, Properties::Object_Text_User_Text).toString();
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






