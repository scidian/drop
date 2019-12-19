//
//      Created by Stephens Nunnally on 11/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QRgb>
#include <QTime>
#include <QVector2D>
#include <QVector3D>

#include "editor/colors/colors.h"
#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "editor/pixmap/pixmap.h"
#include "editor/style/style.h"

namespace Dr
{


//####################################################################################
//##    Draws a Rounded Square Icon with middle_icon in the middle for displaying
//##    various Devices / Controls in the Editor
//####################################################################################
QPixmap DrawFixedIcon(QPixmap &middle_icon) {
    QColor back_color =     Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Light));
    QColor border_color =   Dr::ToQColor(Dr::GetColor(Window_Colors::Midlight));
    back_color.setAlphaF( 0.75 );

    int width =  c_device_size;
    int height = c_device_size;
    int b2 =     c_device_border / 2;
    QPixmap pix(width, height);

    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(   Qt::NoPen );
    painter.setBrush( QBrush(back_color) );
    painter.drawRoundedRect(pix.rect(), 20, 20, Qt::SizeMode::RelativeSize );

    painter.setPen(   QPen(border_color, c_device_border) );
    painter.setBrush( Qt::NoBrush );
    painter.drawRoundedRect(pix.rect().adjusted(b2, b2, -b2, -b2), 20, 20, Qt::SizeMode::RelativeSize );

    int p =         c_device_size / 8;
    int icon_size = c_device_size - (p * 2);

    QPixmap camera = middle_icon.scaled(icon_size, icon_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    int tx = (width / 2) -  (camera.width() / 2);
    int ty = (height / 2) - (camera.height() / 2);
    painter.drawPixmap( QPoint(tx, ty), camera, camera.rect() );

    return pix;
}


//####################################################################################
//##    Draws a DrDeviceType::Camera as a Pixmap
//####################################################################################
QPixmap DrawCamera() {
    QPixmap icon = QPixmap(":/assets/asset_types/camera.png");
    return DrawFixedIcon(icon);
}



}   // End Dr Namespace



















