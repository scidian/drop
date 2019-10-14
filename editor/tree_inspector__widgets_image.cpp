//
//      Created by Stephens Nunnally on 8/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      More non-numerical property row building functions
//          - Image Frame
//
#include <QApplication>
#include <QHBoxLayout>
#include <QPainter>

#include "colors/colors.h"
#include "editor/tree_inspector.h"
#include "forms/form_popup.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "settings/settings_component_property.h"
#include "style/style.h"
#include "widgets/widgets_event_filters.h"


//####################################################################################
//##    Image Frame
//####################################################################################
QFrame* TreeInspector::createImageFrame(DrProperty *property, QFont &font, QSizePolicy size_policy) {

    int frame_height = 100;
    int frame_width =  100;

    // ***** Store current asset key in widget
    QFrame *image_frame = new QFrame();
    image_frame->setAcceptDrops(true);
    image_frame->setSizePolicy(size_policy);
    image_frame->setMinimumHeight(frame_height);
    image_frame->setObjectName("assetFrame");

    long property_key = property->getPropertyKey();
    image_frame->setProperty(User_Property::Mouse_Over, false);                                     // Initialize mouse user data, event filter updates this info
    image_frame->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));                               // Used to track when the mouse
    image_frame->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    m_filter_hover->attachToHoverHandler(image_frame, property);
    addToWidgetList(image_frame);

    // ***** Create the label that will display the image
    QBoxLayout *vertical_split = new QVBoxLayout(image_frame);
    vertical_split->setSpacing(0);
    vertical_split->setMargin(0);
    vertical_split->setContentsMargins(0, 0, 0, 0);
        QLabel *asset_pix = new QLabel(image_frame);
        asset_pix->setObjectName(QStringLiteral("assetPixmap"));
        asset_pix->setFont(font);
        asset_pix->setSizePolicy(size_policy);
        asset_pix->setAlignment(Qt::AlignmentFlag::AlignCenter);
        asset_pix->setPixmap(property->getValue().value<QPixmap>().scaled(QSize(frame_width, frame_height - 15), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        vertical_split->addWidget( asset_pix );

    return image_frame;
}


//####################################################################################
//##
//##    DrFilterInspectorImage Event Filter
//##
//####################################################################################
DrFilterInspectorImage::DrFilterInspectorImage(QObject *parent, IEditorRelay *editor_relay) : QObject(parent), m_editor_relay(editor_relay) { }

bool DrFilterInspectorImage::eventFilter(QObject *object, QEvent *event) {

    //DragEnter = 60,                         // drag moves into widget
    //DragMove = 61,                          // drag moves in widget
    //DragLeave = 62,                         // drag leaves or is cancelled
    //Drop = 63,                              // actual drop

    if (event->type() == QEvent::DragEnter) {
        Dr::SetLabelText(Label_Names::Label_1, "Drag Enter Event: 60, " + Dr::CurrentTimeAsString());
        event->accept();
    }

    if (event->type() == QEvent::DragMove ||
        event->type() == QEvent::DragLeave ||
        event->type() == QEvent::Drop) {
        Dr::SetLabelText(Label_Names::Label_1, "Event: " + QString::number(event->type()) + ", " + Dr::CurrentTimeAsString());
        event->accept();
    }

    // ***** Event Debugging
//    if (event->type() != QEvent::MouseButtonPress &&        //   2
//        event->type() != QEvent::MouseButtonRelease &&      //   3
//        event->type() != QEvent::MouseMove &&               //   5
//        event->type() != QEvent::Enter &&                   //  10
//        event->type() != QEvent::Leave &&                   //  11
//        event->type() != QEvent::Paint &&                   //  12
//        event->type() != QEvent::Move &&                    //  13
//        event->type() != QEvent::Resize &&                  //  14
//        event->type() != QEvent::Show &&                    //  17
//        event->type() != QEvent::Hide &&                    //  18
//        event->type() != QEvent::WindowActivate &&          //  24
//        event->type() != QEvent::WindowDeactivate &&        //  25
//        event->type() != QEvent::ShowToParent &&            //  26
//        event->type() != QEvent::Wheel &&                   //  31
//        event->type() != QEvent::PaletteChange &&           //  39
//        event->type() != QEvent::ChildPolished &&           //  69
//        event->type() != QEvent::PolishRequest &&           //  74
//        event->type() != QEvent::LayoutRequest &&           //  76
//        event->type() != QEvent::UpdateLater &&             //  78
//        event->type() != QEvent::ToolTip &&                 // 110
//        event->type() != QEvent::HoverEnter &&              // 127
//        event->type() != QEvent::HoverLeave &&              // 128
//        event->type() != QEvent::HoverMove &&               // 129
//        event->type() != QEvent::DynamicPropertyChange      // 170
//        ) {
//        Dr::SetLabelText(Label_Names::Label_1, "Event: " + QString::number(event->type()) + ", " + Dr::CurrentTimeAsString());
//    }

    return QObject::eventFilter(object, event);
}













