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
#include "debug.h"
#include "engine_mesh/engine_vertex_data.h"
#include "forms/form_main.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "imaging/imaging.h"
#include "project.h"
#include "project_asset.h"
#include "project_effect.h"
#include "project_font.h"
#include "project_image.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_thing.h"
#include "project/thing_shape_list.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


// Internal Linkage (File Scope) Forward Declarations
DrShapeList autoCollisionShape(QPixmap pixmap);


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrAsset::DrAsset(DrProject *parent_project, long key, DrAssetType new_asset_type, long source_image_key) {
    m_parent_project = parent_project;
    this->setKey(key);

    m_asset_type = new_asset_type;
    m_source_key = source_image_key;

    QPixmap     my_starting_pixmap;
    DrShapeList shape;
    switch (new_asset_type) {
        case DrAssetType::Character:
        case DrAssetType::Object: {
            my_starting_pixmap = m_parent_project->getImage(source_image_key)->getPixmapFromImage();
            shape = autoCollisionShape(my_starting_pixmap);
            if (new_asset_type == DrAssetType::Character) {
                initializeAssetSettingsCharacter(m_parent_project->getImage(source_image_key)->getSimplifiedName(), my_starting_pixmap, shape);
            } else if (new_asset_type == DrAssetType::Object) {
                initializeAssetSettingsObject(m_parent_project->getImage(source_image_key)->getSimplifiedName(), my_starting_pixmap, shape);
            }
            break;
        }
        case DrAssetType::Effect: {
            DrEffect *effect = m_parent_project->getEffect(source_image_key);
            if (effect == nullptr) Dr::ShowErrorMessage("DrProject::addAsset", "Error! Could not find Effect with key: " +
                                                        QString::number(source_image_key), Dr::GetActiveFormMain());
            my_starting_pixmap = effect->getPixmap();
            initializeAssetSettingsEffect(Dr::StringFromEffectType(effect->getEffectType()));
            break;
        }
        case DrAssetType::Text: {
            my_starting_pixmap = m_parent_project->getFont(source_image_key)->getPixmap();
            initializeAssetSettingsFont(m_parent_project->getFont(source_image_key));
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
DrShapeList autoCollisionShape(QPixmap pixmap) {
    DrShapeList shapes;

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
        for (auto asset_pair : m_parent_project->getAssetMap()) {
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
            DrImage *image = m_parent_project->getImageMap()[source_to_delete];
            if (!image) break;
            m_parent_project->getImageMap().erase( source_to_delete );
            delete image;
            break;
        }
        case DrAssetType::Effect: {
            DrEffect *effect = m_parent_project->getEffectMap()[source_to_delete];
            if (!effect) break;
            m_parent_project->getEffectMap().erase( source_to_delete );
            delete effect;
            break;
        }
        case DrAssetType::Text: {
            DrFont *font = m_parent_project->getFontMap()[source_to_delete];
            if (!font) break;
            m_parent_project->getFontMap().erase( source_to_delete );
            delete font;
            break;
        }
    } // end switch
}


//####################################################################################
//##    Updates Default Animation Image / Collsion Shape
//####################################################################################
void DrAsset::updateAnimationProperty(long source_key) {
    if (m_asset_type != DrAssetType::Object && m_asset_type != DrAssetType::Character) return;

    m_source_key = source_key;
    QPixmap     new_pixmap = m_parent_project->getImage(source_key)->getPixmapFromImage();
    setComponentPropertyValue(Components::Asset_Animation, Properties::Asset_Animation_Default, QVariant(new_pixmap));

    DrShapeList shape = autoCollisionShape(new_pixmap);
    setComponentPropertyValue(Components::Asset_Collision, Properties::Asset_Collision_Shape, QVariant::fromValue<DrShapeList>(shape));

    m_width =  new_pixmap.width();
    m_height = new_pixmap.height();
}


//####################################################################################
//##    Property loading - initializeAssetSettings
//####################################################################################
void DrAsset::initializeAssetSettingsCharacter(QString new_name, QPixmap pixmap, DrShapeList &shape) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Character Name");
    property_name->setDescription("Name of the current Character Asset.");
    property_name->setValue(new_name);

    addComponent(Components::Asset_Settings_Character, "Character Settings", "Settings for this Character.", Component_Colors::Mustard_Yellow, true);
    getComponent(Components::Asset_Settings_Character)->setIcon(Component_Icons::Character);

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Max_Speed, Property_Type::PointF, QPointF(1000, 1000),
                           "Max Speed", "Maximum movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Forced_Speed, Property_Type::PointF, QPointF(0, 0),
                           "Forced Speed", "Forced movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Move_Speed, Property_Type::PointF, QPointF(400, 0),
                           "Move Speed", "Button / Joystick movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Force, Property_Type::PointF, QPointF(0, 250),
                           "Jump Force", "Force of jump button in the x and y direction");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Timeout, Property_Type::Positive, 800,
                           "Jump Timeout", "Time, in milliseconds, Character should continue to gain jump force when jump button is held down.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Counter, Property_Type::Int, 1,
                           "Jump Count", "Number of jumps Character can make before having to touch the ground or wall. For unlimited jumps "
                                         "use any negative number (like -1). A Jump Count of 0 disables jumping for this Character.");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Air, Property_Type::Bool, true,
                           "Air Jump?", "Can this Character start jumping while falling in the air? (for example, if the Character fell off a platform)");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Wall, Property_Type::Bool, true,
                           "Wall Jump?", "Can this Character jump off of walls?");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Air_Drag, Property_Type::PositiveDouble, 0.50,
                           "Air Drag", "Affects acceleration and decceleration in air. Usually ranging from 0.0 to 1.0 or higher.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Ground_Drag, Property_Type::PositiveDouble, 0.25,
                           "Ground Drag", "Affects acceleration and decceleration on the ground. Usually ranging from 0.0 to 1.0 or higher.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Rotation_Drag, Property_Type::PositiveDouble, 0.25,
                           "Rotate Drag", "Affects rotation acceleration and decceleration. Usually ranging from 0.0 to 1.0 or higher.");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Can_Rotate, Property_Type::Bool, false,
                           "Can Rotate?", "Can this character rotate (on z axis)? If not, rotation will be fixed.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Feels_Gravity, Property_Type::Bool, true,
                           "Feels Gravity?", "Should this character be affected by gravity?");



    addComponent(Components::Asset_Collision, "Collision Settings", "Collision settings for current Character.", Component_Colors::White_Snow, true);
    getComponent(Components::Asset_Collision)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_Shape, Property_Type::Collision,
                           QVariant::fromValue<DrShapeList>(shape),
                           "Collision Shape", "Shape of the Character as it interacts with other Assets in the world.");

    addComponent(Components::Asset_Animation, "Animation", "Images to show for this Asset.", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Asset_Animation)->setIcon(Component_Icons::Animation);
    addPropertyToComponent(Components::Asset_Animation, Properties::Asset_Animation_Default, Property_Type::Image, QVariant(pixmap),
                           "Default Animation", "Image shown for this Asset.");
}

void DrAsset::initializeAssetSettingsObject(QString new_name, QPixmap pixmap, DrShapeList &shape) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Object Name");
    property_name->setDescription("Name of the current Object Asset.");
    property_name->setValue(new_name);

    addComponent(Components::Asset_Collision, "Collision Settings", "Collision settings for current Object.", Component_Colors::White_Snow, true);
    getComponent(Components::Asset_Collision)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_Shape, Property_Type::Collision,
                           QVariant::fromValue<DrShapeList>(shape),
                           "Collision Shape", "Shape of the Object as it interacts with other Assets in the world.");

    addComponent(Components::Asset_Animation, "Animation", "Images to show for this Asset.", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Asset_Animation)->setIcon(Component_Icons::Animation);
    addPropertyToComponent(Components::Asset_Animation, Properties::Asset_Animation_Default, Property_Type::Image, QVariant(pixmap),
                           "Default Animation", "Image shown for this Asset.");
}

void DrAsset::initializeAssetSettingsEffect(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of the current Effect Asset.");
    property_name->setValue(new_name);
}


void DrAsset::initializeAssetSettingsFont(DrFont *font) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Font Name");
    property_name->setDescription("Name of the current Font Asset.");
    property_name->setValue(font->getName());

    addComponent(Components::Asset_Settings_Font, "Font Settings", "Font settings for this Text Asset.", Component_Colors::Orange_Medium, true);
    getComponent(Components::Asset_Settings_Font)->setIcon(Component_Icons::Font);
    addPropertyToComponent(Components::Asset_Settings_Font, Properties::Asset_Font_Family, Property_Type::String, font->getPropertyFontFamily(),
                           "Font Family", "Font used for this text asset.", false, false);
    addPropertyToComponent(Components::Asset_Settings_Font, Properties::Asset_Font_Size, Property_Type::Int, font->getPropertyFontSize(),
                           "Font Size", "Font size of this text asset.", false, false);
}

















