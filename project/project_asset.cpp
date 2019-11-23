//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrAsset Class Definitions
//
//
#include <QDebug>

#include "3rd_party/hull_finder.h"
#include "3rd_party/polyline_simplification.h"
#include "engine_mesh/engine_vertex_data.h"
#include "forms/form_main.h"
#include "imaging/imaging.h"
#include "project/project.h"
#include "project/project_animation.h"
#include "project/project_asset.h"
#include "project/project_device.h"
#include "project/project_effect.h"
#include "project/project_font.h"
#include "project/project_image.h"
#include "project/project_world.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "properties/property_collision.h"
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
    //      DrAssetType::Device         Underlying DrDevice object
    //      DrAssetType::Effect         Underlying DrEffect object
    //      DrAssetType::Text           Underlying DrFont   object

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
        case DrAssetType::Device: {
            DrDevice *device = getParentProject()->findDeviceFromKey(base_key);
            if (device == nullptr) Dr::ShowErrorMessage("DrProject::addAsset", "Error! Could not find Device with key: " +
                                                        QString::number(base_key), Dr::GetActiveFormMain());
            my_starting_pixmap = device->getPixmap();
            initializeAssetSettingsDevice(Dr::StringFromDeviceType(device->getDeviceType()));
            break;
        }
        case DrAssetType::Effect: {
            DrEffect *effect = getParentProject()->findEffectFromKey(base_key);
            if (effect == nullptr) Dr::ShowErrorMessage("DrProject::addAsset", "Error! Could not find Effect with key: " +
                                                        QString::number(base_key), Dr::GetActiveFormMain());
            my_starting_pixmap = effect->getPixmap();
            initializeAssetSettingsEffect(Dr::StringFromEffectType(effect->getEffectType()));
            break;
        }
        case DrAssetType::Text: {
            DrFont *font = getParentProject()->findFontFromKey(base_key);
            if (font == nullptr) Dr::ShowErrorMessage("DrProject::addAsset", "Error! Could not find Font with key: " +
                                                      QString::number(base_key), Dr::GetActiveFormMain());
            my_starting_pixmap = font->getPixmap();
            initializeAssetSettingsFont(font);
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
    std::list<long> animation_keys_used {};
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
//##    Deletes Underlying Asset Source, called when DrAsset is deleted
//##        Verify first if source is used by another DrAsset or if source is built in
//####################################################################################
void DrAsset::deleteSource() {
    long source_to_delete = getBaseKey();
    if ( source_to_delete < c_key_starting_number) return;

    // ***** Delete Animations not used by other Assets
    if (getAssetType() == DrAssetType::Object || getAssetType() == DrAssetType::Character) {

        // Delete animations
        for (auto &key : animationsUsedByAsset()) {
            getParentProject()->deleteAnimation(key);
        }

    // ***** Delete Underlying Source(s)
    } else {
        // Check all Assets for use of the same Source
        for (auto &asset_pair : getParentProject()->getAssetMap()) {
            DrAsset *asset_to_check = asset_pair.second;
            if (asset_to_check == nullptr) continue;
            if ((asset_pair.first != getKey()) && (asset_to_check->getBaseKey() == getBaseKey())) return;
        }

        switch (getAssetType()) {
            case DrAssetType::Character:
            case DrAssetType::Object:
                break;
            case DrAssetType::Device: {
                DrDevice *device = getParentProject()->getDeviceMap()[source_to_delete];
                if (device == nullptr) break;
                getParentProject()->getDeviceMap().erase( source_to_delete );
                delete device;
                break;
            }
            case DrAssetType::Effect: {
                DrEffect *effect = getParentProject()->getEffectMap()[source_to_delete];
                if (effect == nullptr) break;
                getParentProject()->getEffectMap().erase( source_to_delete );
                delete effect;
                break;
            }
            case DrAssetType::Text: {
                DrFont *font = getParentProject()->getFontMap()[source_to_delete];
                if (font == nullptr) break;
                getParentProject()->getFontMap().erase( source_to_delete );
                delete font;
                break;
            }
        }
    }
}


//####################################################################################
//##    Returns Key of Image of first frame of Animation source
//####################################################################################
long DrAsset::getIdleAnimationFirstFrameImageKey() {
    ///DrAnimation *animation = getParentProject()->findAnimationFromKey(getBaseKey());

    long idle_animation_key = getComponentPropertyValue(Components::Asset_Animation, Properties::Asset_Animation_Idle).toLongLong();
    DrAnimation *animation =  getParentProject()->findAnimationFromKey(idle_animation_key);

    if (animation == nullptr) return c_key_asset_empty;

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
    getParentProject()->deleteAnimation(old_animation);
}













