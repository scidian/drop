//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      DrAsset Class Definitions
//
//
#include "3rd_party/hull_finder.h"
#include "3rd_party/polyline_simplification.h"
#include "core/dr_debug.h"
#include "project/constants_component_info.h"
#include "project/properties/property_collision.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/entities_physics_2d/dr_device.h"
#include "project/entities_physics_2d/dr_effect.h"
#include "project/entities_physics_2d/dr_item.h"
#include "project/entities_physics_2d/dr_prefab.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrAsset::DrAsset(DrProject *parent_project, long key, DrAssetType new_asset_type, long base_key) : DrSettings(parent_project) {
    this->setKey(key);
    this->setBaseKey(base_key);
    this->setWidth(0);
    this->setHeight(0);

    m_asset_type = new_asset_type;

    // ***** Base Key is Project Key of underyling source:
    //      DrAssetType::Character      Refers to DrAnimation object of Asset_Animation_Idle DrProperty OR a DrImage key
    //      DrAssetType::Object         Refers to DrAnimation object of Asset_Animation_Idle DrProperty OR a DrImage key

    DrSettings *source = parent_project->findSettingsFromKey(base_key);


    // ***** If we're using a built in Image, make sure we've traced image outline
    if (source->getType() == DrType::Image) {
        DrImage *dr_image = dynamic_cast<DrImage*>(source);
        if (dr_image->outlineCanceled() == false && dr_image->outlineProcessed() == false) {
            dr_image->autoOutlinePoints();
        }
    }


    // ***** Initialize Asset
    DrPropertyCollision shape;
    std::string     my_starting_name = "Unknown";
    DrBitmap        my_starting_bitmap(32, 32);

    switch (getAssetType()) {
        case DrAssetType::Character:
        case DrAssetType::Object: {

            // Load temp box shape in case we can't pull from DrAnimation
            shape.addPolygon(my_starting_bitmap.polygon().points());
            setWidth( my_starting_bitmap.width);
            setHeight(my_starting_bitmap.height);

            // Create new Animation in Project from Image Key if passed in an Image Key and not an Animation Key
            DrAnimation *animation = nullptr;
            if (source != nullptr) {
                if (source->getType() == DrType::Image) {
                    animation = parent_project->addAnimation({ base_key });
                    if (animation == nullptr) Dr::PrintDebug("Warning! In DrAsset constructor, could not create animation from image: " + std::to_string(base_key));
                    this->setBaseKey(animation->getKey());
                } else if (source->getType() == DrType::Animation) {
                    animation = dynamic_cast<DrAnimation*>(source);
                    if (animation == nullptr) Dr::PrintDebug("Warning! In DrAsset constructor, could not find animation: " + std::to_string(base_key));
                }
                my_starting_bitmap = animation->getFirstFrameImage()->getBitmap();
                my_starting_name =   animation->getName();
                setWidth( my_starting_bitmap.width);
                setHeight(my_starting_bitmap.height);
                shape = autoCollisionShape(animation->getFirstFrameImage());
            } else {
                this->setBaseKey(c_no_key);
            }

            // Load / Initialize Entity Settings
            if (new_asset_type == DrAssetType::Character)
                initializeAssetSettingsCharacter(my_starting_name);
            else if (new_asset_type == DrAssetType::Object)
                initializeAssetSettingsObject(my_starting_name);
            initializeAssetSettingsCollision(getAssetType(), shape);
            initializeAssetSettingsAnimation(getAssetType(), getBaseKey() );
            initializeAssetSettingsPhysics(getAssetType());
            initializeAssetSettingsHealth(getAssetType());
            initializeAssetSettingsControls(getAssetType());
            break;
        }
    }


}

DrAsset::~DrAsset() { }


//####################################################################################
//##    Determines automatic collision shapes based on image
//####################################################################################
DrPropertyCollision DrAsset::autoCollisionShape(const DrImage *image) {
    // Go through each polygon and reduce points if necessary
    DrPropertyCollision shapes;
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











