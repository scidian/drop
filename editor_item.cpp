//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrObject Class Definitions
//
//
#include <QTime>
#include <QStyleOptionGraphicsItem>

#include "colors.h"
#include "debug.h"

#include "editor_item.h"
#include "editor_scene.h"

#include "interface_relay.h"
#include "library.h"

#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

//####################################################################################
//##        Constructor & destructor
//####################################################################################
DrItem::DrItem(DrProject *project, InterfaceRelay *relay, DrObject *object, bool is_temp_only)
{
    // Store relevant project / object data for use later
    m_relay      = relay;

    m_project    = project;
    m_object     = object;
    m_object_key = object->getKey();
    m_asset_key  = m_object->getAssetKey();
    m_asset      = project->getAsset(m_asset_key);

    m_temp_only  = is_temp_only;

    // Load image from asset
    setPixmap(m_asset->getComponentProperty(Asset_Components::animation, Asset_Properties::animation_default)->getValue().value<QPixmap>());

    // Dimensions of associated asset, used for boundingRect
    m_asset_width =  m_asset->getWidth();
    m_asset_height = m_asset->getHeight();

    // Store some initial user data
    setData(User_Roles::Name, m_asset->getAssetName() );
    setData(User_Roles::Type, Dr::StringFromType( m_object->getType() ));

    double  angle =   m_object->getComponentProperty(Object_Components::transform, Object_Properties::rotation)->getValue().toDouble();
    QPointF scale =   m_object->getComponentProperty(Object_Components::transform, Object_Properties::scale)->getValue().toPointF();
    double  z_order = m_object->getComponentProperty(Object_Components::layering,  Object_Properties::z_order)->getValue().toDouble();
    double  opacity = m_object->getComponentProperty(Object_Components::layering,  Object_Properties::opacity)->getValue().toDouble();
    setData(User_Roles::Rotation, angle);
    setData(User_Roles::Scale,    scale);
    setData(User_Roles::Z_Order,  z_order);
    setData(User_Roles::Opacity,  opacity);
    setZValue(  z_order );

    // Adjust item to proper transform
    QPointF center = boundingRect().center();
    QTransform t = QTransform().translate(center.x(), center.y()).rotate(angle).scale(scale.x(), scale.y()).translate(-center.x(), -center.y());
    setTransform(t);

    // Load starting position
    QPointF start_pos = m_object->getComponentProperty(Object_Components::transform, Object_Properties::position)->getValue().toPointF();
    m_start_x = start_pos.x();
    m_start_y = start_pos.y();

    if (Dr::CheckDebugFlag(Debug_Flags::Turn_Off_Antialiasing) == false)
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
QRectF DrItem::boundingRect() const
{
    QRectF my_rect = QRectF(0, 0, m_asset_width, m_asset_height);
    return my_rect;
}

// Seems to define mouseOver events, and intersection events for Rubber Band Box
QPainterPath DrItem::shape() const
{
    ///QPainterPath path;
    ///path.addRect(0, 0, m_width, m_height);
    ///return path;
    return QGraphicsPixmapItem::shape();
}

// Enable the use of qgraphicsitem_cast with this item
int DrItem::type() const
{
    return User_Types::Object;
}



//####################################################################################
//##        Item Change Event - Allows for auto updating on property changes
//####################################################################################
QVariant DrItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    // If this is a temporary object, or not attached to a scene, do not process change
    if (m_temp_only || !m_relay) return QGraphicsPixmapItem::itemChange(change, value);

    // Load any new data stored in item
    double  angle = data(User_Roles::Rotation).toDouble();
    QPointF scale = data(User_Roles::Scale).toPointF();


    // ***** If item position has changed, update it
    if (change == ItemPositionHasChanged) {
        // Value is new scene position (of upper left corner)
        QPointF new_pos =    value.toPointF();

        // Create a transform so we can find new center position of item
        QTransform t = QTransform().rotate(angle).scale(scale.x(), scale.y());
        double new_x = new_pos.x() + t.map( boundingRect().center() ).x();
        double new_y = new_pos.y() + t.map( boundingRect().center() ).y();

        m_object->setComponentPropertyValue(Object_Components::transform, Object_Properties::position, QPointF(new_x, new_y));

        m_relay->updateObjectInspectorAfterItemChange(m_object, { Object_Properties::position });
        return new_pos;
    }

    // ***** If item transform has changed, update it
    if (change == ItemTransformHasChanged) {
        // Value is new item QTransform
        QTransform new_transform = value.value<QTransform>();

        double size_x = m_asset_width *  scale.x();
        double size_y = m_asset_height * scale.y();

        m_object->setComponentPropertyValue(Object_Components::transform, Object_Properties::rotation, angle);
        m_object->setComponentPropertyValue(Object_Components::transform, Object_Properties::scale, scale );
        m_object->setComponentPropertyValue(Object_Components::transform, Object_Properties::size, QPointF(size_x, size_y));

        m_relay->updateObjectInspectorAfterItemChange(m_object, { Object_Properties::size, Object_Properties::scale, Object_Properties::rotation });
        return new_transform;
    }

    // ***** If item z value has changed, update it
    if (change == ItemZValueHasChanged) {
        // Value is new double z value
        double new_z = value.toDouble();
        m_object->setComponentPropertyValue(Object_Components::layering, Object_Properties::z_order, new_z);

        m_relay->updateObjectInspectorAfterItemChange(m_object, { Object_Properties::z_order });
        return new_z;
    }


    // ---------- Following changes are emitted when GraphicsItemFlag::ItemSendsGeometryChanges is set to true

    // ItemPositionChange           // Position is about to change              Value is the new position, PointF           Return a different PointF to change it
    // ItemPositionHasChanged       // Position has changed                                                                 Return does nothing

    // ItemTransformChange          // Transform is about to change             Value is the new transform, QTransform      Return a different QTransform to change it
    // ItemTransformHasChanged      // Transform has changed                                                                Return does nothing

    // ----------

    // ItemEnabledChange            // Enabled state is about to change         Value is the new state, bool                Return a different bool to stop it
    // ItemEnabledHasChanged        // Enabled state has changed                                                            Return does nothing

    // ItemSelectedChange           // Selected state is about to change        Value is if selected or not, bool           Return a different bool to stop it
    // ItemSelectedHasChanged       // Selected state has changed                                                           Return does nothing

    // ItemVisibleChange            // Visible state about to change            Value is Visible or not, bool               Return a different bool to stop it
    // ItemVisibleHasChanged        // Visible state has changed                                                            Return does nothing

    // ItemParentChange             // Parent is about to change                Value is new parent, QGraphicsItem*         Return a different pointer to change it
    // ItemParentHasChanged         // Parent has changed                                                                   Return does nothing

    // ItemChildAddedChange         // Child is added to this item              Value is new child, QGraphicsItem*              Return value is unused
    // ItemChildRemovedChange       // Child is removed from this item          Value is child to be removed, QGraphicsItem*    Return value is unused

    // ItemSceneChange              // Parent scene is about to change          Value is the new scene, QGraphicsScene*
    // ItemSceneHasChanged

    // ItemFlagsChange              // quint_32
    // ItemFlagsHaveChanged

    // ItemZValueChange             // double
    // ItemZValueHasChanged

    // ItemOpacityChange            // double
    // ItemOpacityHasChanged

    // ItemScenePositionHasChanged

    return QGraphicsPixmapItem::itemChange(change, value);
}


//####################################################################################
//##        Custom Paint Handling
//####################################################################################
void DrItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ///if (option->state & QStyle::State_Selected)  { fillColor = QColor(Qt::red); } //m_color.dark(150); }              // If selected
    ///if (option->state & QStyle::State_MouseOver) { fillColor = QColor(Qt::gray); } //fillColor.light(125); }          // If mouse is over

    // Turn off anti aliasing if necessary
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_Off_Antialiasing)) {
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
    }

    // Check item is not super duper tiny, this seems to crash paint function
    QPolygonF poly = ( sceneTransform().map( boundingRect() ));
    QRectF bounds = poly.boundingRect();
    if (bounds.width() < .5 || bounds.height() < .5) return;

    // Set paint option to "not selected" or paint routine will draw dotted lines around item
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

    // Check opacity of current item
    double transparency = 100;
    if (m_object) transparency = m_object->getComponentPropertyValue(Object_Components::layering, Object_Properties::opacity).toDouble();

    // Apply the proper opacity to this item and either paint the pixmap, or paint a pattern representation of the item
    if ( transparency >= 1) {
        setOpacity(transparency / 100);
        QGraphicsPixmapItem::paint(painter, &myOption, widget);
    } else {
        setOpacity(1);
        painter->setPen( QPen(Dr::GetColor(Window_Colors::Icon_Dark), 1, Qt::PenStyle::SolidLine, Qt::PenCapStyle::FlatCap, Qt::PenJoinStyle::MiterJoin ) );
        painter->setBrush( QBrush(Dr::GetColor(Window_Colors::Icon_Dark), Qt::BrushStyle::DiagCrossPattern ) );
        painter->drawPath( this->shape() );
    }
}


QColor DrItem::getColorAtPoint(QPointF at_local_point)
{
    QImage image = this->pixmap().toImage();

    if (image.rect().contains(at_local_point.toPoint()))
        return image.pixelColor(at_local_point.toPoint());
    else
        return QColor(0, 0, 0, 0);
}

QColor DrItem::getColorAtPoint(QPointF at_view_point, QGraphicsView *mouse_over_view)
{
    QPointF in_scene = mouse_over_view->mapToScene(at_view_point.toPoint());
    QPointF in_object = this->mapFromScene(in_scene);
    return this->getColorAtPoint(in_object);
}


//####################################################################################
//##        Input overrides
//####################################################################################
void DrItem::mousePressEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mousePressEvent(event); }
void DrItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseMoveEvent(event); }
void DrItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseReleaseEvent(event); }
void DrItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverLeaveEvent(event);}








