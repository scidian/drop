//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrObject Class Definitions
//
//
#include "colors/colors.h"
#include "debug.h"
#include "editor_item.h"
#include "enums.h"
#include "globals.h"
#include "image_filter_color.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_font.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_object.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##        Constructor & destructor
//####################################################################################
DrItem::DrItem(DrProject *project, IEditorRelay *editor_relay, DrObject *object, bool is_temp_only) : m_editor_relay(editor_relay) {
    // Store relevant project / object data for use later
    m_project    = project;
    m_object     = object;
    m_object_key = object->getKey();
    m_asset_key  = m_object->getAssetKey();
    m_asset      = project->getAsset(m_asset_key);

    m_temp_only  = is_temp_only;

    // Load image from asset
    QString text;

    switch (m_asset->getAssetType()) {
        case DrAssetType::Object:
        case DrAssetType::Character:
            m_pixmap = m_asset->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
            applyFilters();                                 // Apply filters and set pixmap
            m_asset_width =  m_asset->getWidth();           // Dimensions of associated asset, used for boundingRect
            m_asset_height = m_asset->getHeight();
            break;
        case DrAssetType::Text:
            text =     m_object->getComponentPropertyValue(Components::Object_Settings_Text, Properties::Object_Text_User_Text).toString();
            m_pixmap = m_editor_relay->currentProject()->getDrFont( m_asset->getSourceKey() )->createText( text );
            setPixmap(m_pixmap);
            m_asset_width =  m_pixmap.width();
            m_asset_height = m_pixmap.height();
            break;
    }


    // Store some initial user data
    setData(User_Roles::Name, m_asset->getName() );
    setData(User_Roles::Type, Dr::StringFromObjectType( m_object->getObjectType() ));
    setData(User_Roles::Key, QVariant::fromValue(m_object_key));

    double  angle =   m_object->getComponentProperty(Components::Object_Transform, Properties::Object_Rotation)->getValue().toDouble();
    QPointF scale =   m_object->getComponentProperty(Components::Object_Transform, Properties::Object_Scale)->getValue().toPointF();
    double  z_order = m_object->getComponentProperty(Components::Object_Layering,  Properties::Object_Z_Order)->getValue().toDouble();
    double  opacity = m_object->getComponentProperty(Components::Object_Layering,  Properties::Object_Opacity)->getValue().toDouble();
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
    QPointF start_pos = m_object->getComponentProperty(Components::Object_Transform, Properties::Object_Position)->getValue().toPointF();
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
// Outline of entire object
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

    int  brightness = m_object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Brightness).toInt();
    int  contrast   = m_object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Contrast).toInt();
    int  saturation = m_object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Saturation).toInt();
    int  hue        = m_object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Hue).toInt();
    bool grayscale  = m_object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Grayscale).toBool();
    bool negative   = m_object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Negative).toBool();

    if ( negative )        new_pixmap = DrImaging::changeToNegative(new_pixmap);
    if ( grayscale )       new_pixmap = DrImaging::changeToGrayscale(new_pixmap);
    if ( hue        != 0 ) new_pixmap = DrImaging::changeHue(new_pixmap, hue);
    if ( saturation != 0 ) new_pixmap = DrImaging::changeSaturation(new_pixmap, saturation);
    if ( contrast   != 0 ) new_pixmap = DrImaging::changeContrast(new_pixmap, contrast);
    if ( brightness != 0 ) new_pixmap = DrImaging::changeBrightness(new_pixmap, brightness);

    setPixmap(new_pixmap);
}



//####################################################################################
//##        Input overrides
//####################################################################################
void DrItem::mousePressEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mousePressEvent(event); }
void DrItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseMoveEvent(event); }
void DrItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseReleaseEvent(event); }
void DrItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverLeaveEvent(event);}















