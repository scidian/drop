//
//      Created by Stephens Nunnally on 2/19/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPainter>

#include <cmath>

#include "editor/pixmap/pixmap.h"

namespace Dr
{


//####################################################################################
//##    Applies QGraphicsEffect to QImage
//####################################################################################
QImage ApplyEffectToImage(QImage src, QGraphicsEffect *effect, int extend) {
    if (src.isNull()) return QImage();
    if (!effect) return src;

    QImage image(src.size() + QSize(extend * 2, extend * 2), QImage::Format_ARGB32);
           image.fill(Qt::transparent);

    QGraphicsScene scene;
    QGraphicsPixmapItem item;
        item.setPixmap(QPixmap::fromImage(src));
        item.setGraphicsEffect(effect);
        scene.addItem(&item);
        item.setPos((image.width() - src.width()) / 2, (image.height() - src.height()) / 2);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    scene.render(&painter, QRectF(), QRectF(0, 0, image.width(), image.height()));
    return image;
}


//####################################################################################
//##    Applies Outer Glow Border to QImage
//####################################################################################
QImage ApplyBorderToImage(QImage src, QColor border_color, int border_width, int extend) {
    QImage border = Dr::ColorizeImage(src, border_color).scaled(src.size().width()  + border_width*2,
                                                                src.size().height() + border_width*2,
                                                                Qt::AspectRatioMode::KeepAspectRatio, Qt::SmoothTransformation);

    QImage image(src.size() + QSize(extend * 2, extend * 2), QImage::Format_ARGB32);
           image.fill(Qt::transparent);

    QGraphicsScene scene;
    QGraphicsPixmapItem item_border;
        item_border.setPixmap(QPixmap::fromImage(border));
        scene.addItem(&item_border);
        item_border.setPos((image.width() - border.width()) / 2, (image.height() - border.height()) / 2);

    QGraphicsPixmapItem item_image;
        item_image.setPixmap(QPixmap::fromImage(src));
        scene.addItem(&item_image);
        item_image.setPos((image.width() - src.width()) / 2, (image.height() - src.height()) / 2);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    scene.render(&painter, QRectF(), QRectF(0, 0, image.width(), image.height()));
    return image;
}


//####################################################################################
//##    Overlays add_image onto src image
//####################################################################################
QImage OverlayImage(QImage src, QImage add_image, double add_image_scale) {
    QGraphicsScene scene;
    QGraphicsPixmapItem item_add;
        QSize new_size;
        new_size.setWidth(  static_cast<int>(round(static_cast<double>(add_image.width())  * add_image_scale)) );
        new_size.setHeight( static_cast<int>(round(static_cast<double>(add_image.height()) * add_image_scale)) );
        item_add.setPixmap(QPixmap::fromImage(add_image).scaled(new_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        scene.addItem(&item_add);
        item_add.setPos((src.width() - new_size.width()) / 2, (src.height() - new_size.height()) / 2);

    QPainter painter(&src);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    scene.render(&painter, QRectF(), QRectF(0, 0, src.width(), src.height()));
    return src;
}


}   // End namespace Dr





















