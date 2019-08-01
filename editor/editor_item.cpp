//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrItem Class Definitions
//
//
#include "colors/colors.h"
#include "debug.h"
#include "editor_item.h"
#include "enums.h"
#include "globals.h"
#include "image_filter.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_effect.h"
#include "project/project_font.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##        Constructor & destructor
//####################################################################################
DrItem::DrItem(DrProject *project, IEditorRelay *editor_relay, DrThing *thing, bool is_temp_only) : m_editor_relay(editor_relay) {
    // Store relevant project / thing data for use later
    m_project    = project;
    m_thing      = thing;
    m_thing_key  = thing->getKey();
    m_asset_key  = m_thing->getAssetKey();
    m_asset      = project->getAsset(m_asset_key);

    m_temp_only  = is_temp_only;

    // Load image from asset
    switch (m_asset->getAssetType()) {
        case DrAssetType::Object:
        case DrAssetType::Character:
            m_pixmap = m_asset->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
            applyFilters();                                 // Apply filters and set pixmap
            m_asset_width =  m_asset->getWidth();           // Dimensions of associated asset, used for boundingRect
            m_asset_height = m_asset->getHeight();
            break;
        case DrAssetType::Effect:
            switch (m_editor_relay->currentProject()->getDrEffect( m_asset->getSourceKey() )->getEffectType()) {
                case DrEffectType::Light: {
                    uint light_color =  m_thing->getComponentProperty(Components::Thing_Settings_Light, Properties::Thing_Light_Color)->getValue().toUInt();
                    float cone_start =  m_thing->getComponentProperty(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_Start)->getValue().toFloat();
                    float cone_end =    m_thing->getComponentProperty(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_End)->getValue().toFloat();
                    float intensity =   m_thing->getComponentProperty(Components::Thing_Settings_Light, Properties::Thing_Light_Intensity)->getValue().toFloat();
                    float blur =        m_thing->getComponentProperty(Components::Thing_Settings_Light, Properties::Thing_Light_Blur)->getValue().toFloat();
                    m_pixmap = DrImaging::drawLight( QColor::fromRgba( light_color ), 400, cone_start, cone_end, intensity, blur);
                    break;
                }
                case DrEffectType::Water: {
                    uint start_color =  m_thing->getComponentProperty(Components::Thing_Settings_Water, Properties::Thing_Water_Start_Color)->getValue().toUInt();
                    uint end_color =    m_thing->getComponentProperty(Components::Thing_Settings_Water, Properties::Thing_Water_End_Color)->getValue().toUInt();
                    m_pixmap = DrImaging::drawWater( QColor::fromRgba(start_color), QColor::fromRgba(end_color) );
                    break;
                }
            }
            setPixmap(m_pixmap);
            m_asset_width =  m_pixmap.width();
            m_asset_height = m_pixmap.height();
            break;
        case DrAssetType::Text: {
            QString text = m_thing->getComponentPropertyValue(Components::Thing_Settings_Text, Properties::Thing_Text_User_Text).toString();
            m_pixmap = m_editor_relay->currentProject()->getDrFont( m_asset->getSourceKey() )->createText( text );
            setPixmap(m_pixmap);
            m_asset_width =  m_pixmap.width();
            m_asset_height = m_pixmap.height();
            break;
        }
    }


    // Store some initial user data
    setData(User_Roles::Name, m_asset->getName() );
    setData(User_Roles::Type, Dr::StringFromThingType( m_thing->getThingType() ));
    setData(User_Roles::Key, QVariant::fromValue(m_thing_key));

    double  angle =   m_thing->getComponentProperty(Components::Thing_Transform, Properties::Thing_Rotation)->getValue().toDouble();
    QPointF scale =   m_thing->getComponentProperty(Components::Thing_Transform, Properties::Thing_Scale)->getValue().toPointF();
    double  z_order = m_thing->getComponentProperty(Components::Thing_Layering,  Properties::Thing_Z_Order)->getValue().toDouble();
    double  opacity = m_thing->getComponentProperty(Components::Thing_Layering,  Properties::Thing_Opacity)->getValue().toDouble();
    setData(User_Roles::Rotation, angle);
    setData(User_Roles::Scale,    scale);
    setData(User_Roles::Z_Order,  z_order);
    setData(User_Roles::Opacity,  opacity);
    setZValue(  z_order );

    // Adjust item to proper transform
    QPointF center = boundingRect().center();
    double transform_scale_x = Dr::CheckScaleNotZero(scale.x());
    double transform_scale_y = Dr::CheckScaleNotZero(scale.y());
    QTransform t = QTransform()
            .translate(center.x(), center.y())
            .rotate(angle)
            .scale(transform_scale_x, transform_scale_y)
            .translate(-center.x(), -center.y());
    setTransform(t);

    // Load starting position
    QPointF start_pos = m_thing->getComponentProperty(Components::Thing_Transform, Properties::Thing_Position)->getValue().toPointF();
    m_start_x = start_pos.x();
    m_start_y = start_pos.y();

    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing))
        setTransformationMode(Qt::SmoothTransformation);                    // Turn on anti aliasing

    // Set up initial item settings
    if (!m_temp_only) {
        setAcceptHoverEvents(true);                                         // Item tracks mouse movement
        setShapeMode(QGraphicsPixmapItem::MaskShape);                       // Allows for selecting while ignoring transparent pixels
        setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
        setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
        enableItemChangeFlags();
    }

}                                     

void DrItem::disableItemChangeFlags() {
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, false);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, false);
}
void DrItem::enableItemChangeFlags() {
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, true);
}

//####################################################################################
//##        Item Property Overrides
//####################################################################################
// Outline of entire item
QRectF DrItem::boundingRect() const {
    QRectF my_rect = QRectF(0, 0, m_asset_width, m_asset_height);
    return my_rect;
}

// Seems to define mouseOver events, and intersection events for Rubber Band Box
QPainterPath DrItem::shape() const {
    ///QPainterPath path;
    ///path.addRect(0, 0, m_width, m_height);
    ///return path;
    return QGraphicsPixmapItem::shape();
}




//####################################################################################
//##        Pixmap Filters
//####################################################################################
void DrItem::applyFilters() {
    QPixmap new_pixmap = m_pixmap.copy();

    int     brightness = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Brightness).toInt();
    int     contrast   = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Contrast).toInt();
    int     hue        = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Hue).toInt();
    int     saturation = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Saturation).toInt();
    bool    grayscale  = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Grayscale).toBool();
    bool    negative   = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Negative).toBool();
    QPointF pixelation = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();

    if (pixelation.x() > 1.0 || pixelation.y() > 1.0)
                           new_pixmap = DrImaging::applyPixelation( new_pixmap, pixelation );
    if ( negative )        new_pixmap = DrImaging::applySinglePixelFilter( Image_Filter_Type::Negative, new_pixmap, 0 );
    if ( grayscale )       new_pixmap = DrImaging::applySinglePixelFilter( Image_Filter_Type::Grayscale, new_pixmap, 0 );

    // Important to do saturation first, then hue
    if ( saturation != 0 ) new_pixmap = DrImaging::applySinglePixelFilter( Image_Filter_Type::Saturation, new_pixmap, saturation );
    if ( hue        != 0 ) new_pixmap = DrImaging::applySinglePixelFilter( Image_Filter_Type::Hue, new_pixmap, hue );

    if ( contrast   != 0 ) new_pixmap = DrImaging::applySinglePixelFilter( Image_Filter_Type::Contrast, new_pixmap, contrast );
    if ( brightness != 0 ) new_pixmap = DrImaging::applySinglePixelFilter( Image_Filter_Type::Brightness, new_pixmap, brightness );

    setPixmap(new_pixmap);
}



//####################################################################################
//##        Input overrides
//####################################################################################
void DrItem::mousePressEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mousePressEvent(event); }
void DrItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseMoveEvent(event); }
void DrItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseReleaseEvent(event); }
void DrItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverLeaveEvent(event);}















