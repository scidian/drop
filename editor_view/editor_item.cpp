//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrItem Class Definitions
//
//
#include <QGraphicsSceneMouseEvent>
#include "colors/colors.h"
#include "debug.h"
#include "editor_item.h"
#include "enums.h"
#include "globals.h"
#include "imaging/imaging.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "helper_qt.h"
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
//##    Constructor & destructor
//####################################################################################
DrItem::DrItem(DrProject *project, IEditorRelay *editor_relay, DrThing *thing, bool is_temp_only) : m_editor_relay(editor_relay) {
    // Store relevant project / thing data for use later
    m_project    = project;
    m_thing      = thing;
    m_thing_key  = thing->getKey();
    m_asset_key  = m_thing->getAssetKey();
    m_asset      = project->getAsset(m_asset_key);

    m_temp_only  = is_temp_only;                            // Used for interactive resizing in QGraphicsView

    // Load image from asset
    switch (m_asset->getAssetType()) {
        case DrAssetType::Character:
        case DrAssetType::Object:
            m_pixmap = m_asset->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
            applyFilters();                                 // Apply filters and set pixmap
            m_asset_width =  m_asset->getWidth();           // Dimensions of associated asset, used for boundingRect
            m_asset_height = m_asset->getHeight();
            break;
        case DrAssetType::Text: {
            QString text = m_thing->getComponentPropertyValue(Components::Thing_Settings_Text, Properties::Thing_Text_User_Text).toString();
            m_pixmap = m_editor_relay->currentProject()->getFont( m_asset->getSourceKey() )->createText( text );
            setPixmap(m_pixmap);
            m_asset_width =  m_pixmap.width();
            m_asset_height = m_pixmap.height();
            break;
        }
        case DrAssetType::Effect:
            switch (m_editor_relay->currentProject()->getEffect( m_asset->getSourceKey() )->getEffectType()) {
                case DrEffectType::Fire: {
                    uint color_1 =      m_thing->getComponentProperty(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_1)->getValue().toUInt();
                    uint color_2 =      m_thing->getComponentProperty(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_2)->getValue().toUInt();
                    uint smoke =        m_thing->getComponentProperty(Components::Thing_Settings_Fire, Properties::Thing_Fire_Color_Smoke)->getValue().toUInt();
                    int  mask  =        m_thing->getComponentProperty(Components::Thing_Settings_Fire, Properties::Thing_Fire_Shape)->getValue().toInt();
                    m_pixmap = DrImaging::drawFire( QColor::fromRgba(color_1), QColor::fromRgba(color_2), QColor::fromRgba(smoke), static_cast<Fire_Mask>(mask) );
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
                case DrEffectType::Fisheye: {
                    uint color =        m_thing->getComponentProperty(Components::Thing_Settings_Fisheye, Properties::Thing_Fisheye_Color)->getValue().toUInt();
                    m_pixmap = DrImaging::drawFisheye( QColor::fromRgba(color) );
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
                case DrEffectType::Light: {
                    uint light_color =  m_thing->getComponentProperty(Components::Thing_Settings_Light, Properties::Thing_Light_Color)->getValue().toUInt();
                    float cone_start =  m_thing->getComponentProperty(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_Start)->getValue().toList().first().toFloat();
                    float cone_end =    m_thing->getComponentProperty(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_End)->getValue().toList().first().toFloat();
                    float intensity =   m_thing->getComponentProperty(Components::Thing_Settings_Light, Properties::Thing_Light_Intensity)->getValue().toFloat();
                    float blur =        m_thing->getComponentProperty(Components::Thing_Settings_Light, Properties::Thing_Light_Blur)->getValue().toFloat();
                    m_pixmap = DrImaging::drawLight( QColor::fromRgba( light_color ), c_image_size, cone_start, cone_end, intensity, blur);
                    setPixmap(m_pixmap);
                    break;
                }
                case DrEffectType::Mirror: {
                    uint color_1 = m_thing->getComponentProperty(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_Start_Color)->getValue().toUInt();
                    uint color_2 = m_thing->getComponentProperty(Components::Thing_Settings_Mirror, Properties::Thing_Mirror_End_Color)->getValue().toUInt();
                    m_pixmap = DrImaging::drawMirror( QColor::fromRgba(color_1), QColor::fromRgba(color_2) );
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
                case DrEffectType::Swirl: {
                    uint color_1 = m_thing->getComponentProperty(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Start_Color)->getValue().toUInt();
                    float  angle = m_thing->getComponentProperty(Components::Thing_Settings_Swirl, Properties::Thing_Swirl_Angle)->getValue().toFloat();
                    m_pixmap = DrImaging::drawSwirl( QColor::fromRgba(color_1), static_cast<double>(angle) );
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
                case DrEffectType::Water: {
                    uint start_color =  m_thing->getComponentProperty(Components::Thing_Settings_Water, Properties::Thing_Water_Start_Color)->getValue().toUInt();
                    uint end_color =    m_thing->getComponentProperty(Components::Thing_Settings_Water, Properties::Thing_Water_End_Color)->getValue().toUInt();
                    m_pixmap = DrImaging::drawWater( QColor::fromRgba(start_color), QColor::fromRgba(end_color) );
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
            }
            m_asset_width =  m_pixmap.width();
            m_asset_height = m_pixmap.height();
            break;
    }


    // Store some initial user data
    setData(User_Roles::Name, m_asset->getName() );
    QString description = m_thing->getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Advisor_Description).toString();
    if (description == "") description = Dr::StringFromThingType( m_thing->getThingType() );
    setData(User_Roles::Type, description);
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
    m_item_change_flags_enabled = false;
}
void DrItem::enableItemChangeFlags() {
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, true);
    m_item_change_flags_enabled = true;
}

//####################################################################################
//##    Item Property Overrides
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
//##    Pixmap Filters
//####################################################################################
void DrItem::applyFilters() {
    QImage new_image = m_pixmap.toImage().copy();

    int     brightness = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Brightness).toList().first().toInt();
    int     contrast   = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Contrast).toList().first().toInt();
    int     hue        = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Hue).toList().first().toInt();
    int     saturation = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Saturation).toList().first().toInt();
    bool    grayscale  = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Grayscale).toBool();
    bool    negative   = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Negative).toBool();
    QPointF pixelation = m_thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();

    if (pixelation.x() > 1.0 || pixelation.y() > 1.0)
                           new_image = DrImaging::applyPixelation( new_image, pixelation );
    if ( negative )        new_image = DrImaging::applySinglePixelFilter( Image_Filter_Type::Negative, new_image, 0 );
    if ( grayscale )       new_image = DrImaging::applySinglePixelFilter( Image_Filter_Type::Grayscale, new_image, 0 );

    // Important to do saturation first, then hue
    if ( saturation != 0 ) new_image = DrImaging::applySinglePixelFilter( Image_Filter_Type::Saturation, new_image, saturation );
    if ( hue        != 0 ) new_image = DrImaging::applySinglePixelFilter( Image_Filter_Type::Hue, new_image, hue );

    if ( contrast   != 0 ) new_image = DrImaging::applySinglePixelFilter( Image_Filter_Type::Contrast, new_image, contrast );
    if ( brightness != 0 ) new_image = DrImaging::applySinglePixelFilter( Image_Filter_Type::Brightness, new_image, brightness );

    setPixmap( QPixmap::fromImage(new_image) );
}



//####################################################################################
//##    Input overrides
//####################################################################################
void DrItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    DrSettings *settings = m_project->findSettingsFromKey(getThingKey());
    if (settings == nullptr)  event->ignore();
    if (settings->isLocked()) event->ignore();
    QGraphicsItem::mousePressEvent(event);
}

void DrItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    DrSettings *settings = m_project->findSettingsFromKey(getThingKey());
    if (settings == nullptr)  event->ignore();
    if (settings->isLocked()) event->ignore();
    QGraphicsItem::mouseMoveEvent(event);
}

void DrItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    DrSettings *settings = m_project->findSettingsFromKey(getThingKey());
    if (settings == nullptr)  event->ignore();
    if (settings->isLocked()) event->ignore();
    QGraphicsItem::mouseReleaseEvent(event);
}

void DrItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverLeaveEvent(event);}















