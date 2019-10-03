//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrAsset Class Definitions
//
//
#include <QDebug>

#include "3rd_party/hullfinder.h"
#include "3rd_party/polyline_simplification.h"
#include "debug.h"
#include "engine_mesh/engine_vertex_data.h"
#include "helper.h"
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

// Custom types for QVariant
Q_DECLARE_METATYPE(DrShapeList);

// Internal Linkage (File Scope) Forward Declarations
DrShapeList autoCollisionShape(QPixmap pixmap);


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrAsset::DrAsset(DrProject *parent_project, long new_asset_key, DrAssetType new_asset_type, long source_key) {
    m_parent_project = parent_project;
    setKey(new_asset_key);

    m_asset_type = new_asset_type;
    m_source_key = source_key;

    m_list_order = new_asset_key;
    m_group_number = 0;

    QPixmap     my_starting_pixmap;
    DrShapeList shape;
    switch (new_asset_type) {
        case DrAssetType::Character:
        case DrAssetType::Object:
            my_starting_pixmap = m_parent_project->getDrImage(source_key)->getPixmapFromImage();
            shape = autoCollisionShape(my_starting_pixmap);
            if (new_asset_type == DrAssetType::Character) {
                initializeAssetSettingsCharacter(m_parent_project->getDrImage(source_key)->getSimplifiedName(), my_starting_pixmap, shape);
            } else if (new_asset_type == DrAssetType::Object) {
                initializeAssetSettingsObject(m_parent_project->getDrImage(source_key)->getSimplifiedName(), my_starting_pixmap, shape);
            }
            break;
        case DrAssetType::Effect:
            my_starting_pixmap = m_parent_project->getDrEffect(source_key)->getPixmap();
            initializeAssetSettingsEffect(Dr::StringFromEffectType(m_parent_project->getDrEffect(source_key)->getEffectType()));
            break;
        case DrAssetType::Text:
            my_starting_pixmap = m_parent_project->getDrFont(source_key)->getPixmap();
            initializeAssetSettingsFont(m_parent_project->getDrFont(source_key));
            break;

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

            // Run Polyline Simplification algorithm
            QVector<DrPointF> simple_points;
            simple_points = QVector<DrPointF>::fromStdVector( PolylineSimplification::RamerDouglasPeucker(points.toStdVector(), 2.0) );
            if ((simple_points.count() < 4)) {
                ///points = HullFinder::FindConcaveHull(points, 5.0);
                points.clear();
                points.push_back( DrPointF(rects[image_number].topLeft().x(),        rects[image_number].topLeft().y()) );
                points.push_back( DrPointF(rects[image_number].topRight().x(),       rects[image_number].topRight().y()) );
                points.push_back( DrPointF(rects[image_number].bottomRight().x(),    rects[image_number].bottomRight().y()) );
                points.push_back( DrPointF(rects[image_number].bottomLeft().x(),     rects[image_number].bottomLeft().y()) );
                points.push_back( DrPointF(rects[image_number].topLeft().x(),        rects[image_number].topLeft().y()) );
            } else{
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

    // If we have to polygons by this point, just add a simple box
    if (shapes.getPolygons().size() < 1) {
        shapes.addPolygon( box.toStdVector() );
    }

    // Adjust points in Polygons so that (0, 0) is the center of the image
    for (auto &shape : shapes.getPolygons()) {
        for (auto &point : shape) {
            point.x = point.x - (pixmap.width() / 2.0);
            point.y = (pixmap.height() - point.y) - (pixmap.height() / 2.0);
        }
    }

    return shapes;
}


//####################################################################################
//##    Property loading - initializeAssetSettings
//####################################################################################

void DrAsset::initializeAssetSettingsCharacter(QString new_name, QPixmap pixmap, DrShapeList &shape) {
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, new_name,
                           "Character Name", "Name of the current Character Asset.");
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Description, Property_Type::String, "",
                           "Description", "Custom Advisor description for this item.");

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
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, new_name,
                           "Object Name", "Name of the current Object Asset.");
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Description, Property_Type::String, "",
                           "Description", "Custom Advisor description for this item.");

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
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, new_name,
                           "Effect Name", "Name of the current Effect Asset.", false, false);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Description, Property_Type::String, "",
                           "Description", "Custom Advisor description for this item.");
}


void DrAsset::initializeAssetSettingsFont(DrFont *font) {
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, font->getName(),
                           "Font Name", "Name of the current Font Asset.");
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Description, Property_Type::String, "",
                           "Description", "Custom Advisor description for this item.");

    addComponent(Components::Asset_Settings_Font, "Font Settings", "Font settings for this Text Asset.", Component_Colors::Orange_Medium, true);
    getComponent(Components::Asset_Settings_Font)->setIcon(Component_Icons::Font);
    addPropertyToComponent(Components::Asset_Settings_Font, Properties::Asset_Font_Family, Property_Type::String, font->getPropertyFontFamily(),
                           "Font Family", "Font used for this text asset.", false, false);
    addPropertyToComponent(Components::Asset_Settings_Font, Properties::Asset_Font_Size, Property_Type::Int, font->getPropertyFontSize(),
                           "Font Size", "Font size of this text asset.", false, false);
}

















