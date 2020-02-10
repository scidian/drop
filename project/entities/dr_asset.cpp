//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrAsset Class Definitions
//
//
#include "3rd_party/polyline_simplification.h"
#include "core/dr_debug.h"
#include "project/constants_component_info.h"
#include "project/properties/property_collision.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_device.h"
#include "project/entities/dr_effect.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Determines automatic collision shapes based on image
//####################################################################################
DrPropertyCollision autoCollisionShape(const DrImage *image) {
    // Go through each polygon and reduce points if necessary
    DrPropertyCollision     shapes;
    for (int poly_number = 0; poly_number < static_cast<int>(image->m_poly_list.size()); poly_number++) {
        // Run Polyline Simplification algorithm, keep running until collision shape polygon has less than 100 points
        std::vector<DrPointF> simple_points = image->m_poly_list[poly_number];
        double simple_tolerance = 2.0;
        while (simple_points.size() > 100) {
            simple_points = PolylineSimplification::RamerDouglasPeucker(simple_points, simple_tolerance);
            simple_tolerance = std::pow(simple_tolerance, 2.0);
        }

        // Add polygon to list of polygons in shape
        shapes.addPolygon( simple_points );
    }

    // If we don't have polygons by this point, just add a simple box
    if (shapes.getPolygons().size() < 1) shapes.addPolygon( image->getBitmap().polygon().points() );

    // Adjust points in Boxes / Polygons so that (0, 0) is the center of the image
    for (auto &shape : shapes.getPolygons()) {
        for (auto &point : shape) {
            point.x = point.x - (image->getBitmap().width / 2.0);
            point.y = (image->getBitmap().height - point.y) - (image->getBitmap().height / 2.0);
        }
    }
    return shapes;
}


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
    std::string     my_starting_name = "Unknown";
    DrBitmap        my_starting_bitmap(32, 32);

    switch (getAssetType()) {
        case DrAssetType::Character:
        case DrAssetType::Object: {

            // Load temp box shape in case we can't pull from DrAnimation
            shape.addPolygon(my_starting_bitmap.polygon().points());
            m_width =  my_starting_bitmap.width;
            m_height = my_starting_bitmap.height;

            // Create new Animation in Project from Image Key if passed in an Image Key and not an Animation Key
            DrAnimation *animation = nullptr;
            if (source != nullptr) {
                if (source->getType() == DrType::Image) {
                    animation = parent_project->addAnimation({ base_key });
                    if (animation == nullptr) Dr::PrintDebug("Warning! In DrAsset constructor, could not create animation from image: " + std::to_string(base_key));
                    m_base_key = animation->getKey();
                } else if (source->getType() == DrType::Animation) {
                    animation = dynamic_cast<DrAnimation*>(source);
                    if (animation == nullptr) Dr::PrintDebug("Warning! In DrAsset constructor, could not find animation: " + std::to_string(base_key));
                }
                my_starting_bitmap = animation->getFirstFrameImage()->getBitmap();
                my_starting_name =   animation->getName();
                m_width =  my_starting_bitmap.width;
                m_height = my_starting_bitmap.height;
                shape = autoCollisionShape(animation->getFirstFrameImage());
            } else {
                m_base_key = c_no_key;
            }

            // Load / Initialize Entity Settings
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
                if (property->getValue().toLong() >= c_key_starting_number)
                    animation_keys_used.push_back(property->getValue().toLong());
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
    long idle_animation_key = getComponentPropertyValue(Comps::Asset_Animation, Props::Asset_Animation_Idle).toLong();
    return idle_animation_key;
}

//####################################################################################
//##    Returns Key of Image of first frame of Animation source
//####################################################################################
long DrAsset::getIdleAnimationFirstFrameImageKey() {
    ///DrAnimation *animation = getParentProject()->findAnimationFromKey(getBaseKey());

    long idle_animation_key = getComponentPropertyValue(Comps::Asset_Animation, Props::Asset_Animation_Idle).toLong();
    if (idle_animation_key == c_no_key) return c_key_image_empty;

    DrAnimation *animation =  getParentProject()->findAnimationFromKey(idle_animation_key);
    if (animation == nullptr)           return c_key_image_empty;

    return animation->getFrame(animation->getStartFrameNumber())->getKey();
}


//####################################################################################
//##    Updates Default Animation Images / Collsion Shape
//####################################################################################
void DrAsset::updateAnimationProperty(std::list<long> image_keys, ComponentProperty animation_component_property_key) {
    if (m_asset_type != DrAssetType::Object && m_asset_type != DrAssetType::Character) return;

    // ***** Get existing DrProperty to Replace
    DrProperty *property =   getComponentProperty(animation_component_property_key);
    if (property == nullptr) return;
    long old_animation = property->getValue().toLong();

    // ***** Create new animation from new image keys
    DrAnimation *animation = getParentProject()->addAnimation(image_keys);    
    property->setValue( animation->getKey() );

    if (property->getPropertyKey() == Props::Asset_Animation_Idle) {
        DrImage     *new_image = animation->getFirstFrameImage();
        m_width =    new_image->getBitmap().width;
        m_height =   new_image->getBitmap().height;
        m_base_key = animation->getKey();

        // Calculate new image collision shape
        DrPropertyCollision shape = autoCollisionShape(new_image);
        setComponentPropertyValue(Comps::Asset_Collision, Props::Asset_Collision_Image_Shape, shape);
    }

    // ***** Delete Old Animation
    getParentProject()->deleteAnimation(old_animation, this->getKey());
}













