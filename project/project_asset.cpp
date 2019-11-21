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
DrAsset::DrAsset(DrProject *parent_project, long key, DrAssetType new_asset_type, long source_image_key) : DrSettings(parent_project) {
    this->setKey(key);
    m_asset_type = new_asset_type;
    m_source_key = source_image_key;

    DrSettings *source = parent_project->findSettingsFromKey(source_image_key);

    DrPropertyCollision shape;
    QString     my_starting_name = "";
    QPixmap     my_starting_pixmap;
    switch (getAssetType()) {
        case DrAssetType::Character:
        case DrAssetType::Object: {

            // Add Animation to Project from Image if passed in image key and not animation key
            DrAnimation *animation = nullptr;
            if (source->getType() == DrType::Image) {
                animation = parent_project->addAnimation({ source_image_key });
                if (animation == nullptr) Dr::ShowErrorMessage("DrAsset::DrAsset", "Could not create animation from image: " + QString::number(source_image_key));
                m_source_key = animation->getKey();
            } else if (source->getType() == DrType::Animation) {
                animation = parent_project->findAnimationFromKey(source_image_key);
                if (animation == nullptr) Dr::ShowErrorMessage("DrAsset::DrAsset", "Could not find animation: " + QString::number(source_image_key));
            }
            my_starting_pixmap = animation->getPixmapFromFirstFrame();
            my_starting_name =   animation->getName();

            // Load / Initialize Entity Settings
            shape = autoCollisionShape(my_starting_pixmap);
            if (new_asset_type == DrAssetType::Character)
                initializeAssetSettingsCharacter(my_starting_name);
            else if (new_asset_type == DrAssetType::Object)
                initializeAssetSettingsObject(my_starting_name);
            initializeAssetSettingsCollision(getAssetType(), shape);
            initializeAssetSettingsAnimation(getAssetType(), my_starting_pixmap);
            initializeAssetSettingsHealth(getAssetType());
            initializeAssetSettingsPhysics(getAssetType());
            initializeAssetSettingsControls(getAssetType());
            break;
        }
        case DrAssetType::Device: {
            DrDevice *device = getParentProject()->getDevice(source_image_key);
            if (device == nullptr) Dr::ShowErrorMessage("DrProject::addAsset", "Error! Could not find Device with key: " +
                                                        QString::number(source_image_key), Dr::GetActiveFormMain());
            my_starting_pixmap = device->getPixmap();
            initializeAssetSettingsDevice(Dr::StringFromDeviceType(device->getDeviceType()));
            break;
        }
        case DrAssetType::Effect: {
            DrEffect *effect = getParentProject()->getEffect(source_image_key);
            if (effect == nullptr) Dr::ShowErrorMessage("DrProject::addAsset", "Error! Could not find Effect with key: " +
                                                        QString::number(source_image_key), Dr::GetActiveFormMain());
            my_starting_pixmap = effect->getPixmap();
            initializeAssetSettingsEffect(Dr::StringFromEffectType(effect->getEffectType()));
            break;
        }
        case DrAssetType::Text: {
            my_starting_pixmap = getParentProject()->getFont(source_image_key)->getPixmap();
            initializeAssetSettingsFont(getParentProject()->getFont(source_image_key));
            break;
        }
    }

    m_width =  my_starting_pixmap.width();
    m_height = my_starting_pixmap.height();
}

DrAsset::~DrAsset() { }


//####################################################################################
//##    Determines automatic collision shapes based on image
//####################################################################################
DrPropertyCollision autoCollisionShape(QPixmap pixmap) {
    DrPropertyCollision shapes;

    // ***** Break pixmap into seperate images for each object in image
    QVector<QImage> images;
    QVector<QRect>  rects;
    DrImaging::findObjectsInImage(pixmap.toImage(), images, rects, 0.9);

    // ***** Creates a box polygon for use with complicated / not complicated enough images
    QVector<DrPointF> box;
    box.clear();
    box.push_back( DrPointF(0,                  0) );
    box.push_back( DrPointF(pixmap.width() - 1, 0) );
    box.push_back( DrPointF(pixmap.width() - 1, pixmap.height() - 1) );
    box.push_back( DrPointF(0,                  pixmap.height() - 1) );

    // ***** Go through each image (object) and Polygon for it
    if (images.count() < 50) {
        for (int image_number = 0; image_number < images.count(); image_number++) {
            QImage &image = images[image_number];
            if (image.width() < 1 || image.height() < 1) continue;

            // Trace edge of image
            QVector<DrPointF> points = DrImaging::traceImageOutline(image);
            double plus_one_pixel_percent_x = 1.0 + (1.50 / image.width());
            double plus_one_pixel_percent_y = 1.0 + (1.50 / image.height());
            for (auto &point : points) {
                point.x = point.x * plus_one_pixel_percent_x;
                point.y = point.y * plus_one_pixel_percent_y;
            }

            // Run Polyline Simplification algorithm
            QVector<DrPointF> simple_points;
            simple_points = QVector<DrPointF>::fromStdVector( PolylineSimplification::RamerDouglasPeucker(points.toStdVector(), 2.0) );

            // If we only have a couple points left, add shape as a box of the original image, otherwise use PolylineSimplification points
            if ((simple_points.count() < 4)) {
                ///points = HullFinder::FindConcaveHull(points, 5.0);
                points.clear();
                points.push_back( DrPointF(rects[image_number].topLeft().x(),        rects[image_number].topLeft().y()) );
                points.push_back( DrPointF(rects[image_number].topRight().x(),       rects[image_number].topRight().y()) );
                points.push_back( DrPointF(rects[image_number].bottomRight().x(),    rects[image_number].bottomRight().y()) );
                points.push_back( DrPointF(rects[image_number].bottomLeft().x(),     rects[image_number].bottomLeft().y()) );
                points.push_back( DrPointF(rects[image_number].topLeft().x(),        rects[image_number].topLeft().y()) );
            } else {
                points = simple_points;
            }

            // Check we still have 3 points, remove duplicate first point
            if (points.count() < 4) continue;
            points.pop_back();

            // Check winding
            HullFinder::EnsureWindingOrientation(points, Winding_Orientation::CounterClockwise);

            // Add polygon to list of polygons in shape
            shapes.addPolygon( points.toStdVector() );
        }
    }

    // If we don't have polygons by this point, just add a simple box
    if (shapes.getPolygons().size() < 1) {
        shapes.addPolygon( box.toStdVector() );
    }

    // Adjust points in Boxes / Polygons so that (0, 0) is the center of the image
    for (auto &shape : shapes.getPolygons()) {
        for (auto &point : shape) {
            point.x = point.x - (pixmap.width() / 2.0);
            point.y = (pixmap.height() - point.y) - (pixmap.height() / 2.0);
        }
    }

    return shapes;
}


//####################################################################################
//##    Check if built in Asset Source, or if used by another Asset,
//##        Otherwise can delete source if desired
//####################################################################################
bool DrAsset::canDeleteSource() {
    bool can_delete = true;
    if (getSourceKey() < c_key_starting_number) {
        can_delete = false;
    } else {
        // Check all Assets for use of the same Source
        for (auto asset_pair : getParentProject()->getAssetMap()) {
            DrAsset *asset_to_check = asset_pair.second;
            if ((asset_pair.first != getKey()) && (asset_to_check->getSourceKey() == getSourceKey())) {
                can_delete = false;
                break;
            }
        }
    }
    return can_delete;
}

// Delete underlying source to Asset
void DrAsset::deleteSource(long source_key) {
    if (canDeleteSource() == false) return;

    long source_to_delete = (source_key == c_no_key) ? getSourceKey() : source_key;

    switch (getAssetType()) {
        case DrAssetType::Character:
        case DrAssetType::Object: {
            // Get animation
            DrAnimation *animation = getParentProject()->getAnimation(source_to_delete);
            if (animation == nullptr) break;

            // Delete all images in animation
            for (auto frame : animation->getFrames()) {
                DrImage *image = getParentProject()->getImageMap()[frame->getKey()];
                if (image == nullptr) continue;
                getParentProject()->getImageMap().erase( source_to_delete );
                delete image;
            }
            // Delete animation
            getParentProject()->deleteAnimation(m_source_key);
            break;
        }
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
    } // end switch
}


//####################################################################################
//##    Returns Key of Image of first frame of Animation source
//####################################################################################
long DrAsset::getAnimationFirstFrameImageKey() {
    DrAnimation *animation = getParentProject()->getAnimation(getSourceKey());
    if (animation == nullptr) return c_no_key;
    return animation->getFrame(animation->getStartFrameNumber())->getKey();
}


//####################################################################################
//##    Updates Default Animation Images / Collsion Shape
//####################################################################################
void DrAsset::updateAnimationProperty(QList<long> image_keys) {
    if (m_asset_type != DrAssetType::Object && m_asset_type != DrAssetType::Character) return;

    // Create new animation from new image keys
    DrAnimation *animation = getParentProject()->addAnimation(image_keys);
    m_source_key = animation->getKey();

    // Get image from new animation for Inspector
    QPixmap     new_pixmap = animation->getPixmapFromFirstFrame();
    setComponentPropertyValue(Components::Asset_Animation, Properties::Asset_Animation_Default, QVariant(new_pixmap));

    // Calculate new image collision shape
    DrPropertyCollision shape = autoCollisionShape(new_pixmap);
    setComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Image_Shape, QVariant::fromValue<DrPropertyCollision>(shape));

    m_width =  new_pixmap.width();
    m_height = new_pixmap.height();
}















