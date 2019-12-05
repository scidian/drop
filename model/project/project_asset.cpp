//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrAsset Class Definitions
//
//
#include <QDebug>

#include "model/properties/property_collision.h"
#include "model/project/project.h"
#include "model/project/project_animation.h"
#include "model/project/project_asset.h"
#include "model/project/project_device.h"
#include "model/project/project_effect.h"
#include "model/project/project_font.h"
#include "model/project/project_image.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"

#include "debug.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"


// Internal Linkage (File Scope) Forward Declarations
DrPropertyCollision autoCollisionShape(QPixmap pixmap);


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrAsset::DrAsset(DrProject *parent_project, long key, DrAssetType new_asset_type, long base_key) : DrSettings(parent_project) {
    this->setKey(key);
    m_asset_type = new_asset_type;
    m_base_key =   base_key;

    // Base Key is Project Key of underyling source:
    //      DrAssetType::Character      Refers to DrAnimation object of Asset_Animation_Idle DrProperty
    //      DrAssetType::Object         Refers to DrAnimation object of Asset_Animation_Idle DrProperty

    DrSettings *source = parent_project->findSettingsFromKey(base_key);

    DrPropertyCollision shape;
    QString     my_starting_name = "Unknown";
    QPixmap     my_starting_pixmap(":/assets/dr_images/empty.png");

    switch (getAssetType()) {
        case DrAssetType::Character:
        case DrAssetType::Object: {

            // Create new Animation in Project from Image Key if passed in an Image Key and not an Animation Key
            DrAnimation *animation = nullptr;
            if (source != nullptr) {
                if (source->getType() == DrType::Image) {
                    animation = parent_project->addAnimation({ base_key });
                    if (animation == nullptr) Dr::ShowErrorMessage("DrAsset::DrAsset", "Could not create animation from image: " + QString::number(base_key));
                    m_base_key = animation->getKey();
                } else if (source->getType() == DrType::Animation) {
                    animation = dynamic_cast<DrAnimation*>(source);
                    if (animation == nullptr) Dr::ShowErrorMessage("DrAsset::DrAsset", "Could not find animation: " + QString::number(base_key));
                }
                my_starting_pixmap = animation->getPixmapFromFirstFrame();
                my_starting_name =   animation->getName();
            } else {
                m_base_key = c_no_key;
            }

            // Load / Initialize Entity Settings
            shape = autoCollisionShape(my_starting_pixmap);
            if (new_asset_type == DrAssetType::Character)
                initializeAssetSettingsCharacter(my_starting_name);
            else if (new_asset_type == DrAssetType::Object)
                initializeAssetSettingsObject(my_starting_name);
            initializeAssetSettingsCollision(getAssetType(), shape);
            initializeAssetSettingsAnimation(getAssetType(), m_base_key );
            initializeAssetSettingsHealth(getAssetType());
            initializeAssetSettingsPhysics(getAssetType());
            initializeAssetSettingsControls(getAssetType());
            break;
        }
    }

    m_width =  my_starting_pixmap.width();
    m_height = my_starting_pixmap.height();
}

DrAsset::~DrAsset() { }


//####################################################################################
//##    List of Animations used by Asset
//####################################################################################
std::list<long> DrAsset::animationsUsedByAsset() {
    std::list<long> animation_keys_used { };
    for (auto &component_pair : getComponentMap()) {
        for (auto &property_pair : component_pair.second->getPropertyMap()) {
            DrProperty *property = property_pair.second;
            if (property->getPropertyType() == Property_Type::Image) {
                if (property->getValue().toLongLong() >= c_key_starting_number)
                    animation_keys_used.push_back(property->getValue().toLongLong());
            }
        }
    }
    return animation_keys_used;
}

//####################################################################################
//##    Deletes Underlying Animations, called when DrAsset is deleted
//##        Verify first if Animations are used by other DrAssets or if Animation is built in
//####################################################################################
void DrAsset::deleteAnimations() {
    for (auto &animation_key : animationsUsedByAsset()) {
        getParentProject()->deleteAnimation(animation_key, this->getKey());
    }
}


//####################################################################################
//##    Returns Key of Idle Animation
//####################################################################################
long DrAsset::getIdleAnimationKey() {
    long idle_animation_key = getComponentPropertyValue(Components::Asset_Animation, Properties::Asset_Animation_Idle).toLongLong();
    return idle_animation_key;
}

//####################################################################################
//##    Returns Key of Image of first frame of Animation source
//####################################################################################
long DrAsset::getIdleAnimationFirstFrameImageKey() {
    ///DrAnimation *animation = getParentProject()->findAnimationFromKey(getBaseKey());

    long idle_animation_key = getComponentPropertyValue(Components::Asset_Animation, Properties::Asset_Animation_Idle).toLongLong();
    DrAnimation *animation =  getParentProject()->findAnimationFromKey(idle_animation_key);

    if (animation == nullptr) return c_key_image_empty;

    return animation->getFrame(animation->getStartFrameNumber())->getKey();
}


//####################################################################################
//##    Updates Default Animation Images / Collsion Shape
//####################################################################################
void DrAsset::updateAnimationProperty(QList<long> image_keys, Properties animation_property) {
    if (m_asset_type != DrAssetType::Object && m_asset_type != DrAssetType::Character) return;

    // ***** Get existing DrProperty to Replace
    DrProperty *property =   findPropertyFromPropertyKey(animation_property);
    if (property == nullptr) return;
    long old_animation = property->getValue().toLongLong();

    // ***** Create new animation from new image keys
    DrAnimation *animation = getParentProject()->addAnimation(image_keys);    
    property->setValue( QVariant::fromValue(animation->getKey()) );

    if (property->getPropertyKey() == Dr::EnumToInt(Properties::Asset_Animation_Idle)) {
        QPixmap      new_pixmap = animation->getPixmapFromFirstFrame();
        m_width =    new_pixmap.width();
        m_height =   new_pixmap.height();
        m_base_key = animation->getKey();

        // Calculate new image collision shape
        DrPropertyCollision shape = autoCollisionShape(new_pixmap);
        setComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Image_Shape, QVariant::fromValue<DrPropertyCollision>(shape));
    }

    // ***** Delete Old Animation
    getParentProject()->deleteAnimation(old_animation, this->getKey());
}













