//
//      Created by Stephens Nunnally on 3/22/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QGuiApplication>
#include <QDrag>
#include <QEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QTimer>

#include "constants.h"
#include "editor_tree_assets.h"
#include "enums.h"
#include "globals.h"
#include "image_filter_color.h"
#include "interface_editor_relay.h"
#include "library.h"
#include "project.h"
#include "project_asset.h"
#include "project_font.h"
#include "settings.h"
#include "settings_component_property.h"

//####################################################################################
//##    AssetMouseHandler Constructor
//####################################################################################
AssetMouseHandler::AssetMouseHandler(QObject *parent, IEditorRelay *editor_relay) : QObject(parent), m_editor_relay(editor_relay)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), SLOT(startScroll()));
}


//####################################################################################
//##    eventFilter - handles mouse click on asset,
//##                  loads object inspector for clicked asset,
//##                  starts drag and drop event
//####################################################################################
bool AssetMouseHandler::eventFilter(QObject *object, QEvent *event)
{
    // Grab the frame and get some properties from it
    QWidget *asset_frame =  dynamic_cast<QWidget*>(object);
    if (!asset_frame) return false;
    QLabel  *label_name =   asset_frame->findChild<QLabel*>("assetName");
    QLabel  *label_pixmap = asset_frame->findChild<QLabel*>("assetPixmap");
    long     asset_key =    asset_frame->property(User_Property::Key).toLongLong();

    // On mouse down, update the object inspector, prepare for drag and drop
    if (event->type() == QEvent::MouseButtonPress) {
        m_editor_relay->buildObjectInspector( { asset_key } );

        QMouseEvent *mouse_event = dynamic_cast<QMouseEvent*>(event);
        asset_frame->setProperty(User_Property::Mouse_Down,     true);
        asset_frame->setProperty(User_Property::Mouse_Down_Pos, mouse_event->pos());

    } else if (event->type() == QEvent::MouseButtonRelease) {
        asset_frame->setProperty(User_Property::Mouse_Down, false);

    // Start drag and drop if necessary
    } else if (event->type() == QEvent::MouseMove) {

        if (asset_frame->property(User_Property::Mouse_Down).toBool() == true) {
            QMouseEvent *mouse_event = dynamic_cast<QMouseEvent*>(event);

            QPoint distance = mouse_event->pos() - asset_frame->property(User_Property::Mouse_Down_Pos).toPoint();
            if (distance.manhattanLength() > 2) {
                startDragAndDrop(label_pixmap, asset_key);
            }
        }

    // Start scrolling name if name is too wide to be shown
    } else if (event->type() == QEvent::HoverEnter) {
        DrSettings  *asset = m_editor_relay->currentProject()->findSettingsFromKey(asset_key);
        QString asset_name = asset->getComponentPropertyValue(Components::Asset_Settings, Properties::Asset_Name).toString();

        if (asset_name != label_name->text()) {
            if (scrolling_mutex.tryLock()) {
                m_flag_scrolling =  true;
                m_position =        0;
                m_scroll_text =     asset_name + "     ";
                m_scroll_width =    Dr::CheckFontWidth(label_name->font(), m_scroll_text);
                m_starting_rect =   label_name->geometry();
                m_starting_text =   label_name->text();
                m_label_to_scroll = label_name;
                m_timer->start();
            }
        }


    // Reset asset name if it was scrolling
    } else if (event->type() == QEvent::HoverLeave) {

        m_flag_scrolling = false;
        m_timer->stop();
        if (scrolling_mutex.tryLock() == false) {
            label_name->setGeometry( m_starting_rect );
            label_name->setText( m_starting_text );
        }
        scrolling_mutex.unlock();
    }

    return QObject::eventFilter(object, event);
}

//####################################################################################
//##    Asset Name Label Scrolling for long names
//####################################################################################
void AssetMouseHandler::startScroll() { this->handleScroll(m_label_to_scroll, true); }

void AssetMouseHandler::handleScroll(QLabel *label, bool first_time)
{
    // If we should have stopped, exit out
    if (m_flag_scrolling == false) return;

    // Adjust label position
    if (first_time) {
        label->setText(m_scroll_text + m_scroll_text);
        label->setGeometry(m_position, 0, Dr::CheckFontWidth(label->font(), label->text()), label->geometry().height());
    }
    label->setGeometry(m_position, 0, label->geometry().width(), label->geometry().height());

    // Get ready for next position
    if (abs(m_position) <= m_scroll_width)  m_position -= 1;
    else                                    m_position += m_scroll_width - 1;

    // Fire off new call to this function after time has passed
    QTimer::singleShot( 35, this, [this, label] { this->handleScroll(label, false); } );
}


//####################################################################################
//##    Drag and Drop start for Asset
//####################################################################################
void AssetMouseHandler::startDragAndDrop(QLabel *label_pixmap, long asset_key)
{
    // Get asset from project
    DrAsset *asset = m_editor_relay->currentProject()->getAsset(asset_key);

    // Pull pixmap of asset and scale based on current view zoom level
    QPixmap pre_pixmap = *label_pixmap->pixmap();
    QPixmap pixmap;
    QString text;
    switch (asset->getAssetType()) {
    case DrAssetType::Object:
        pixmap = asset->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
        break;
    case DrAssetType::Text:
        //text =   asset->getComponentPropertyValue(Components::Object_Settings_Text, Properties::Object_Text_User_Text).toString();
        pixmap = m_editor_relay->currentProject()->getDrFont( asset->getSourceKey() )->createText( "Text" );
        break;
    }
    pixmap = DrFilter::changeOpacity(pixmap, -64);
    int scaled_x = static_cast<int>( pixmap.width() *  m_editor_relay->currentViewZoom() );
    int scaled_y = static_cast<int>( pixmap.height() * m_editor_relay->currentViewZoom() );
    scaled_x = Dr::Clamp(scaled_x, 5, QGuiApplication::screenAt( QCursor::pos() )->geometry().width()  * 1);
    scaled_y = Dr::Clamp(scaled_y, 5, QGuiApplication::screenAt( QCursor::pos() )->geometry().height() * 1);
    pixmap = pixmap.scaled(scaled_x, scaled_y, Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation);

    // Use center of pixmap as the hot spot
    QPoint hot_spot = pixmap.rect().center();

    // Create item data for QDrag event
    QByteArray item_data;
    QDataStream data_stream(&item_data, QIODevice::WriteOnly);
    QPoint relative_center = pixmap.rect().center() - hot_spot + QPoint(0, 1);          // Helps place asset into scene at mouse position
    data_stream << relative_center << QVariant::fromValue(asset_key);
    QMimeData *mime_data = new QMimeData;
    mime_data->setData("application/x-drop-asset-data", item_data);

    // Create drag object
    QDrag *drag = new QDrag(label_pixmap);
    drag->setMimeData(mime_data);
    drag->setPixmap(pixmap);
    drag->setHotSpot(hot_spot);

    // Execute drag command
    QPixmap temp_pixmap = DrFilter::changeOpacity(pre_pixmap, -128);
    label_pixmap->setPixmap(temp_pixmap);
    drag->exec(Qt::CopyAction, Qt::CopyAction);
    label_pixmap->setPixmap(pre_pixmap);

}




















