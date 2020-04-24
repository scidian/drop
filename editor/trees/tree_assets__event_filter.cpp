//
//      Created by Stephens Nunnally on 3/22/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QGuiApplication>
#include <QEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QScreen>
#include <QTimer>

#include "core/colors/colors.h"
#include "editor/constants_advisor_info.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/trees/tree_assets.h"
#include "project/enums_entity_types.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_effect.h"
#include "project/entities/dr_font.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    DrFilterAssetMouseHandler Constructor
//####################################################################################
DrFilterAssetMouseHandler::DrFilterAssetMouseHandler(QObject *parent, IEditorRelay *editor_relay) : QObject(parent), m_editor_relay(editor_relay) {
    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), SLOT(startScroll()));
}


//####################################################################################
//##    eventFilter - handles mouse click on asset,
//##                  loads Inspector for clicked asset,
//##                  starts drag and drop event
//####################################################################################
bool DrFilterAssetMouseHandler::eventFilter(QObject *object, QEvent *event) {
    // Grab the frame and get some properties from it
    QWidget *asset_frame =  dynamic_cast<QWidget*>(object);
    if (asset_frame == nullptr)     return QObject::eventFilter(object, event);
    long     asset_key =    asset_frame->property(User_Property::Key).toLongLong();
    if (asset_key <= 0)             return QObject::eventFilter(object, event);

    QLabel  *label_name =   asset_frame->findChild<QLabel*>("assetName");
    QLabel  *label_pixmap = asset_frame->findChild<QLabel*>("assetPixmap");
    if (label_name == nullptr)      return QObject::eventFilter(object, event);
    if (label_pixmap == nullptr)    return QObject::eventFilter(object, event);

    TreeAssets *asset_tree = m_editor_relay->getAssetTree();
    if (asset_tree == nullptr)      return QObject::eventFilter(object, event);

    // On mouse down, update the Inspector, prepare for drag and drop
    if (event->type() == QEvent::MouseButtonPress) {
        asset_tree->setSelectedKey( asset_key );
        m_editor_relay->setActiveWidget(Editor_Widgets::Asset_Tree);

        m_editor_relay->buildInspector( { asset_key } );
        m_editor_relay->updateItemSelection(Editor_Widgets::Asset_Tree, { });

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
                startDrag(label_pixmap, asset_key);
            }
        }

    // Start scrolling name if name is too wide to be shown
    } else if (event->type() == QEvent::HoverEnter) {
        DrSettings *entity = m_editor_relay->currentProject()->findSettingsFromKey(asset_key);
        if (entity == nullptr) return QObject::eventFilter(object, event);
        QString asset_name = QString::fromStdString(entity->getName());

        if (asset_name != label_name->text()) {
            m_flag_scrolling =  true;
            m_position =        0;
            m_scroll_text =     asset_name + "     ";
            m_scroll_width =    Dr::CheckFontWidth(label_name->font(), m_scroll_text);
            m_starting_rect =   label_name->geometry();
            m_starting_text =   label_name->text();
            m_label_to_scroll = label_name;
            m_timer->start();
        }

    // Highlights selected Asset Item
    } else if (event->type() == QEvent::Paint) {
        if (asset_key == asset_tree->getSelectedKey()) {
            QPainter painter(asset_frame);
            painter.setRenderHint(QPainter::Antialiasing, true);
            int border_size = (m_editor_relay->getActiveWidget() == Editor_Widgets::Asset_Tree) ? 2 : 1;
            painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Dark)), border_size) );
            painter.setBrush(Qt::NoBrush);
            QRect box = asset_frame->rect();
            box.setX( box.x() + 6);     box.setWidth(  box.width() -  1 );
            box.setY( box.y() + 2);     box.setHeight( box.height() - 3 );
            painter.drawRoundedRect(box, 5, 5, Qt::SizeMode::AbsoluteSize);
        }

    // Reset asset name if it was scrolling
    } else if (event->type() == QEvent::HoverLeave) {
        m_flag_scrolling = false;
        m_timer->stop();
        label_name->setGeometry( m_starting_rect );
        label_name->setText( m_starting_text );
    }


    // ***** Event Debugging
    // Paint == 12
    // ToolTip = 110
    // DynamicPropertyChange = 170
    if (event->type() != QEvent::MouseMove &&
        event->type() != QEvent::MouseButtonPress &&
        event->type() != QEvent::MouseButtonRelease &&
        event->type() != QEvent::HoverMove) {
        ///Dr::SetLabelText(Label_Names::Label_1, "Event: " + QString::number(event->type()) + ", " + Dr::CurrentTimeAsString());
    }

    return QObject::eventFilter(object, event);
}


//####################################################################################
//##    Asset Name Label Scrolling for long names
//####################################################################################
void DrFilterAssetMouseHandler::startScroll() { this->handleScroll(m_label_to_scroll, true); }

void DrFilterAssetMouseHandler::handleScroll(QLabel *label, bool first_time) {
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






















