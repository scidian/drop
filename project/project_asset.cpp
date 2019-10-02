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

// Forward Declarations
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
                           "Asset Name", "Name of the current Asset.");

    addComponent(Components::Asset_Object_Settings, "Asset Settings", "Basic settings for current Object Asset.", Component_Colors::White_Snow, true);
    getComponent(Components::Asset_Object_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::Asset_Object_Settings, Properties::Asset_Collision_Shape, Property_Type::Collision,
                           QVariant::fromValue<DrShapeList>(shape),
                           "Collision Shape", "Shape of the Asset as it interacts with other Assets in the world.");

    addComponent(Components::Asset_Animation, "Animation", "Images to show for this Asset.", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Asset_Animation)->setIcon(Component_Icons::Animation);
    addPropertyToComponent(Components::Asset_Animation, Properties::Asset_Animation_Default, Property_Type::Image, QVariant(pixmap),
                           "Default Animation", "Image shown for this Asset.");
}

void DrAsset::initializeAssetSettingsObject(QString new_name, QPixmap pixmap, DrShapeList &shape) {
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, new_name,
                           "Asset Name", "Name of the current Asset.");

    addComponent(Components::Asset_Object_Settings, "Asset Settings", "Basic settings for current Object Asset.", Component_Colors::White_Snow, true);
    getComponent(Components::Asset_Object_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::Asset_Object_Settings, Properties::Asset_Collision_Shape, Property_Type::Collision,
                           QVariant::fromValue<DrShapeList>(shape),
                           "Collision Shape", "Shape of the Asset as it interacts with other Assets in the world.");

    addComponent(Components::Asset_Animation, "Animation", "Images to show for this Asset.", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Asset_Animation)->setIcon(Component_Icons::Animation);
    addPropertyToComponent(Components::Asset_Animation, Properties::Asset_Animation_Default, Property_Type::Image, QVariant(pixmap),
                           "Default Animation", "Image shown for this Asset.");
}

void DrAsset::initializeAssetSettingsEffect(QString new_name) {
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, new_name,
                           "Asset Name", "Name of the current Asset.", false, false);
}


void DrAsset::initializeAssetSettingsFont(DrFont *font) {
    addComponent(Components::Entity_Name, "Name", "Name of selected item.", Component_Colors::Red_Tuscan, true);
    getComponent(Components::Entity_Name)->setIcon(Component_Icons::Name);
    addPropertyToComponent(Components::Entity_Name, Properties::Entity_Name, Property_Type::String, font->getName(),
                           "Asset Name", "Name of the current Asset.");

    addComponent(Components::Asset_Font_Settings, "Font Settings", "Font settings for this Text Asset.", Component_Colors::Orange_Medium, true);
    getComponent(Components::Asset_Font_Settings)->setIcon(Component_Icons::Font);
    addPropertyToComponent(Components::Asset_Font_Settings, Properties::Asset_Font_Family, Property_Type::String, font->getPropertyFontFamily(),
                           "Font Family", "Font used for this text asset.", false, false);
    addPropertyToComponent(Components::Asset_Font_Settings, Properties::Asset_Font_Size, Property_Type::Int, font->getPropertyFontSize(),
                           "Font Size", "Font size of this text asset.", false, false);
}

















