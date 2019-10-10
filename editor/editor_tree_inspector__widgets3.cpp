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
#include "editor_tree_inspector.h"
#include "forms/form_popup.h"
#include "helper.h"
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
    image_frame->setSizePolicy(size_policy);
    image_frame->setMinimumHeight(frame_height);
    image_frame->setObjectName("assetFrame");

    long property_key = property->getPropertyKey();
    image_frame->setProperty(User_Property::Mouse_Over, false);                                     // Initialize mouse user data, event filter updates this info
    image_frame->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));                               // Used to track when the mouse
    image_frame->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    //image_frame->installEventFilter(new DrFilterAssetMouseHandler(single_asset, m_editor_relay));
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











