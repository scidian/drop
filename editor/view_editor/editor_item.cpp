//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      EditorItem Class Definitions
//
//
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

#include "core/colors/colors.h"
#include "core/dr_debug.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/project/project.h"
#include "editor/view_editor/editor_item.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "engine/debug_flags.h"
#include "project/constants_comps_and_props.h"
#include "project/enums_entity_types.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_image.h"
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
EditorItem::EditorItem(DrProject *project, IEditorRelay *editor_relay, DrThing *thing, bool is_temp_only) : m_editor_relay(editor_relay) {
    // ***** Store relevant project / thing data for use later
    m_project    = project;
    m_thing      = thing;
    m_thing_key  = thing->getKey();
    m_asset_key  = m_thing->getAssetKey();
    m_asset      = project->findSettingsFromKey(m_asset_key);

    m_temp_only  = is_temp_only;                            // Used for interactive resizing in QGraphicsView

    // ********** Load image from asset
    switch (m_asset->getType()) {
        case DrType::Asset: {
            DrAsset *asset = dynamic_cast<DrAsset*>(m_asset);
            switch (asset->getAssetType()) {
                case DrAssetType::Character:
                case DrAssetType::Object: {
                    DrImage *image = m_project->findImageFromKey(asset->getIdleAnimationFirstFrameImageKey());
                    if (image != nullptr) m_pixmap = Dr::ToQPixmap(image->getBitmap());
                    applyFilters();                                 // Apply filters and set pixmap
                    m_asset_width =  asset->getWidth();             // Dimensions of associated asset, used for boundingRect
                    m_asset_height = asset->getHeight();
                    break;
                }
            }
            break;
        }

        case DrType::Font: {
            std::string text = m_thing->getComponentPropertyValue(Comps::Thing_Settings_Text, Props::Thing_Text_User_Text).toString();
            m_pixmap = Dr::CreateText(m_editor_relay->currentProject()->findFontFromKey( m_asset->getKey() ), text );
            setPixmap(m_pixmap);
            m_asset_width =  m_pixmap.width();
            m_asset_height = m_pixmap.height();
            break;
        }

        case DrType::Device: {
            DrDevice *device = dynamic_cast<DrDevice*>(m_asset);
            switch (device->getDeviceType()) {
                case DrDeviceType::Camera: {
                    m_pixmap = Dr::DrawCamera();
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
            }
            m_asset_width =  m_pixmap.width();
            m_asset_height = m_pixmap.height();
            break;
        }

        case DrType::Effect: {
            DrEffect *effect = dynamic_cast<DrEffect*>(m_asset);
            switch (effect->getEffectType()) {
                case DrEffectType::Fire: {
                    uint color_1 =      m_thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_1).toUInt();
                    uint color_2 =      m_thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_2).toUInt();
                    uint smoke =        m_thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Color_Smoke).toUInt();
                    int  mask  =        m_thing->getComponentPropertyValue(Comps::Thing_Settings_Fire, Props::Thing_Fire_Shape).toInt();
                    m_pixmap = Dr::DrawFire( QColor::fromRgba(color_1), QColor::fromRgba(color_2), QColor::fromRgba(smoke), static_cast<Fire_Mask>(mask) );
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
                case DrEffectType::Fisheye: {
                    uint color =        m_thing->getComponentProperty(Comps::Thing_Settings_Fisheye, Props::Thing_Fisheye_Color)->getValue().toUInt();
                    m_pixmap = Dr::DrawFisheye( QColor::fromRgba(color) );
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
                case DrEffectType::Light: {
                    uint light_color =  m_thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Color).toUInt();
                    float cone_start =  m_thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Cone_Start).toVector()[0].toFloat();
                    float cone_end =    m_thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Cone_End).toVector()[0].toFloat();
                    float intensity =   m_thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Intensity).toFloat();
                    float blur =        m_thing->getComponentPropertyValue(Comps::Thing_Settings_Light, Props::Thing_Light_Blur).toFloat();
                    m_pixmap = Dr::DrawLight( QColor::fromRgba( light_color ), c_image_size, cone_start, cone_end, intensity, blur);
                    setPixmap(m_pixmap);
                    break;
                }
                case DrEffectType::Mirror: {
                    uint color_1 = m_thing->getComponentPropertyValue(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_Start_Color).toUInt();
                    uint color_2 = m_thing->getComponentPropertyValue(Comps::Thing_Settings_Mirror, Props::Thing_Mirror_End_Color).toUInt();
                    m_pixmap = Dr::DrawMirror( QColor::fromRgba(color_1), QColor::fromRgba(color_2) );
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
                case DrEffectType::Swirl: {
                    uint color_1 = m_thing->getComponentPropertyValue(Comps::Thing_Settings_Swirl, Props::Thing_Swirl_Start_Color).toUInt();
                    float  angle = m_thing->getComponentPropertyValue(Comps::Thing_Settings_Swirl, Props::Thing_Swirl_Angle).toFloat();
                    m_pixmap = Dr::DrawSwirl( QColor::fromRgba(color_1), static_cast<double>(angle) );
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
                case DrEffectType::Water: {
                    uint start_color =  m_thing->getComponentPropertyValue(Comps::Thing_Settings_Water, Props::Thing_Water_Start_Color).toUInt();
                    uint end_color =    m_thing->getComponentPropertyValue(Comps::Thing_Settings_Water, Props::Thing_Water_End_Color).toUInt();
                    m_pixmap = Dr::DrawWater( QColor::fromRgba(start_color), QColor::fromRgba(end_color) );
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
            }
            m_asset_width =  m_pixmap.width();
            m_asset_height = m_pixmap.height();
            break;
        }

        case DrType::Item: {
            DrItem *dr_item = dynamic_cast<DrItem*>(m_asset);
            switch (dr_item->getItemType()) {
                case DrItemType::Tile: {
                    m_pixmap = QPixmap(":/assets/asset_types/tile.png");
                    setPixmap(m_pixmap);
                    applyFilters();
                    break;
                }
            }
            m_asset_width =  m_pixmap.width();
            m_asset_height = m_pixmap.height();
            break;
        }

        default:
            Dr::PrintDebug("Error in EditorItem Constructor, DrType not handled! \n"
                           "Type Not Handled: " + Dr::StringFromType(m_asset->getType()) + " - End Error.....");
    }


    // ***** Store some initial user data
    std::string            description = m_thing->getComponentPropertyValue(Comps::Hidden_Settings, Props::Hidden_Advisor_Description).toString();
    if (description == "") description = Dr::StringFromThingType(m_thing->getThingType());
    setData(User_Roles::Name,       QString::fromStdString(m_asset->getName()) );
    setData(User_Roles::Type,       QString::fromStdString(description) );
    setData(User_Roles::Key,        QVariant::fromValue(m_thing_key));

    double   angle =   m_thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Rotation).toDouble();
    DrPointF scale =   m_thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Scale).toPointF();
    double   z_order = m_thing->getZOrderWithSub();
    double   opacity = m_thing->getComponentPropertyValue(Comps::Thing_Layering,  Props::Thing_Opacity).toDouble();
    setData(User_Roles::Rotation, angle);
    setData(User_Roles::Scale,      QPointF(scale.x, scale.y) );
    setData(User_Roles::Z_Order,    z_order);
    setData(User_Roles::Opacity,    opacity);
    setZValue(  z_order );

    // Adjust item to proper transform
    QPointF center = boundingRect().center();
    double transform_scale_x = Dr::CheckScaleNotZero(scale.x);
    double transform_scale_y = Dr::CheckScaleNotZero(scale.y);
    QTransform t = QTransform()
            .translate(center.x(), center.y())
            .rotate(angle)
            .scale(transform_scale_x, transform_scale_y)
            .translate(-center.x(), -center.y());
    setTransform(t);

    // Load starting position
    DrPointF start_pos = m_thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Position).toPointF();
    m_start_x = start_pos.x;
    m_start_y = start_pos.y;

    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor))
        setTransformationMode(Qt::SmoothTransformation);                            // Turn on anti aliasing

    // ***** Set up initial item settings
    if (!m_temp_only) {
        setAcceptHoverEvents(true);                                                 // Item tracks mouse movement
        setShapeMode(QGraphicsPixmapItem::ShapeMode::MaskShape);                    // Allows for selecting while ignoring transparent pixels
        setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable);
        setFlag(QGraphicsItem::GraphicsItemFlag::ItemIsMovable);
        enableItemChangeFlags();
    }

}                                     

void EditorItem::disableItemChangeFlags() {
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, false);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, false);
    m_item_change_flags_enabled = false;
}
void EditorItem::enableItemChangeFlags() {
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, true);
    m_item_change_flags_enabled = true;
}


//####################################################################################
//##    Item Property Overrides
//####################################################################################
// Outline of entire item
QRectF EditorItem::boundingRect() const {
    QRectF my_rect = QRectF(0, 0, m_asset_width, m_asset_height);
    return my_rect;
}

// Defines mouseOver events, and intersection events for Rubber Band Box
QPainterPath EditorItem::shape() const {
    ///QPainterPath path;
    ///path.addRect(0, 0, m_width, m_height);
    ///return path;
    return QGraphicsPixmapItem::shape();
}




//####################################################################################
//##    Pixmap Filters
//####################################################################################
void EditorItem::applyFilters() {
    if (m_thing == nullptr) return;
    if (m_thing->getComponent(Comps::Thing_Appearance, false) == nullptr) return;

    QImage new_image = m_pixmap.toImage().copy();
    int      brightness = m_thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Brightness).toVector()[0].toInt();
    int      contrast   = m_thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Contrast).toVector()[0].toInt();
    int      hue        = m_thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Hue).toVector()[0].toInt();
    int      saturation = m_thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Saturation).toVector()[0].toInt();
    bool     grayscale  = m_thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Grayscale).toBool();
    bool     negative   = m_thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Negative).toBool();
    DrPointF pixelation = m_thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Pixelation).toPointF();
    int      bit_rate =   m_thing->getComponentPropertyValue(Comps::Thing_Appearance, Props::Thing_Filter_Bitrate).toVector()[0].toInt();

    if ( pixelation.x > 1.0 || pixelation.y > 1.0 )
                                       Dr::ApplyPixelation( new_image, QPointF(pixelation.x, pixelation.y) );
    if ( negative )        new_image = Dr::ApplySinglePixelFilter( Image_Filter_Type::Negative, new_image, 0 );
    if ( grayscale )       new_image = Dr::ApplySinglePixelFilter( Image_Filter_Type::Grayscale, new_image, 0 );

    // Important to do saturation first, then hue
    if ( saturation != 0 ) new_image = Dr::ApplySinglePixelFilter( Image_Filter_Type::Saturation, new_image, saturation );
    if ( hue        != 0 ) new_image = Dr::ApplySinglePixelFilter( Image_Filter_Type::Hue, new_image, hue );

    if ( contrast   != 0 ) new_image = Dr::ApplySinglePixelFilter( Image_Filter_Type::Contrast, new_image, contrast );
    if ( brightness != 0 ) new_image = Dr::ApplySinglePixelFilter( Image_Filter_Type::Brightness, new_image, brightness );

    if ( bit_rate < 256 )  new_image = Dr::ApplySinglePixelFilter( Image_Filter_Type::Bitrate, new_image, bit_rate );

    // Make all mostly transparent pixels fully transparent, better QGraphicsItem.shape() mouse select / grab
    new_image = Dr::CheckOpacityTolerance(new_image);

    // Set updated image as Pixmap
    setPixmap( QPixmap::fromImage(new_image) );
}



//####################################################################################
//##    Input overrides
//####################################################################################
void EditorItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    DrSettings *settings = getThing();
    if (settings == nullptr) settings = m_project->findSettingsFromKey(getThingKey());
    if      (settings == nullptr)  event->ignore();
    else if (settings->isLocked()) event->ignore();
    QGraphicsItem::mousePressEvent(event);
}

void EditorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    DrSettings *settings = getThing();
    if (settings == nullptr) settings = m_project->findSettingsFromKey(getThingKey());
    if      (settings == nullptr)  event->ignore();
    else if (settings->isLocked()) event->ignore();
    QGraphicsItem::mouseMoveEvent(event);
}

void EditorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    DrSettings *settings = getThing();
    if (settings == nullptr) settings = m_project->findSettingsFromKey(getThingKey());
    if      (settings == nullptr)  event->ignore();
    else if (settings->isLocked()) event->ignore();
    QGraphicsItem::mouseReleaseEvent(event);
}

void EditorItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverLeaveEvent(event);}















