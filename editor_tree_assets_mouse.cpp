//
//      Created by Stephens Nunnally on 3/22/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QEvent>
#include <QTimer>

#include "constants.h"
#include "editor_tree_assets.h"
#include "enums.h"
#include "globals.h"
#include "interface_editor_relay.h"
#include "library.h"
#include "project.h"
#include "project_asset.h"


//####################################################################################
//##    AssetMouseHandler Class Functions
//##        eventFilter - handles mouse click on asset, loads object inspector for clicked asset
//####################################################################################
AssetMouseHandler::AssetMouseHandler(QObject *parent, IEditorRelay *editor_relay) : QObject(parent), m_editor_relay(editor_relay)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), SLOT(startScroll()));
}

bool AssetMouseHandler::eventFilter(QObject *object, QEvent *event)
{
    if (!object) return false;
    QWidget *asset_frame =  dynamic_cast<QWidget*>(object);
    QLabel  *label =        asset_frame->findChild<QLabel*>("assetName");
    long     asset_key =    asset_frame->property(User_Property::Key).toLongLong();

    if (event->type() == QEvent::MouseButtonPress)
    {
        m_editor_relay->buildObjectInspector( { asset_key } );


    // Start scrolling name if name is too wide to be shown
    } else if (event->type() == QEvent::HoverEnter) {
        DrSettings  *asset = m_editor_relay->currentProject()->findSettingsFromKey(asset_key);
        QString asset_name = asset->getComponentPropertyValue(Components::Asset_Settings, Properties::Asset_Name).toString();

        if (asset_name != label->text()) {
            if (scrolling_mutex.tryLock()) {
                m_flag_scrolling =  true;
                m_position =        0;
                m_scroll_text =     asset_name + "     ";
                m_scroll_width =    Dr::CheckFontWidth(label->font(), m_scroll_text);
                m_starting_rect =   label->geometry();
                m_starting_text =   label->text();
                m_label_to_scroll = label;
                m_timer->start();
            }
        }


    // Reset asset name if it was scrolling
    } else if (event->type() == QEvent::HoverLeave) {

        m_flag_scrolling = false;
        m_timer->stop();
        if (scrolling_mutex.tryLock() == false) {
            label->setGeometry( m_starting_rect );
            label->setText( m_starting_text );
        }
        scrolling_mutex.unlock();
    }


    return QObject::eventFilter(object, event);
}

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







