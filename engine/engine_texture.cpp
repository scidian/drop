//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QPainter>
#include <QPixmap>

#include "engine.h"
#include "engine_texture.h"

//####################################################################################
//##        Constructor
//####################################################################################
DrEngineTexture::DrEngineTexture(QString from_asset_string) {
    loadTexture(from_asset_string);
}

DrEngineTexture::DrEngineTexture(QPixmap &from_pixmap) {
    loadTexture(from_pixmap);
}

//####################################################################################
//##        Loads a texture
//####################################################################################
void DrEngineTexture::loadTexture(QString from_asset_string) {
    QImage image = QImage(from_asset_string);
    if (image.format() != QImage::Format::Format_ARGB32 )
        image = image.convertToFormat( QImage::Format_ARGB32 );

    QPixmap pix = QPixmap::fromImage( image );
    loadTexture( pix );
}

void DrEngineTexture::loadTexture(QPixmap &from_pixmap) {

    // Load image, NOTE: QImage is mirrored vertically to account for the fact that OpenGL and QImage use opposite directions for the y axis
    QImage image = from_pixmap.toImage().mirrored();

    // Add a "c_texture_border" pixel border to reduce artifacts during multi sampling
    QPixmap one_pixel_border( image.width() + (c_texture_border*2), image.height() + (c_texture_border*2));
    one_pixel_border.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&one_pixel_border);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawPixmap( c_texture_border, c_texture_border, image.width(), image.height(), QPixmap::fromImage(image), 0, 0, image.width(), image.height());
    painter.end();

    // Create new texture
    m_texture = new QOpenGLTexture( one_pixel_border.toImage() );

    ///QOpenGLTexture* m_texture = new QOpenGLTexture(QOpenGLTexture::Target2DArray);
    ///m_texture->setSize(one_pixel_border.width(), one_pixel_border.height());
    ///m_texture->setFormat(QOpenGLTexture::RGBAFormat); ;//)  RGBA8_UNorm);
    ///// you can manage the number of mimap you desire...
    ///// by default 256x256 => 9 mipmap levels will be allocated: 256, 128, 64, 32, 16, 8, 4, 2 and 1px to modify this use tile->setMipLevels(n);
    ///m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    ///m_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    ///m_texture->setData(one_pixel_border.toImage(), QOpenGLTexture::GenerateMipMaps);

    //m_texture->setMinificationFilter( QOpenGLTexture::Filter::Nearest);                  // no anti aliasing
    //m_texture->setMinificationFilter( QOpenGLTexture::Filter::NearestMipMapLinear);      // small is edgy
    //m_texture->setMinificationFilter( QOpenGLTexture::Filter::NearestMipMapNearest);     // small is edgy
    m_texture->setMinificationFilter( QOpenGLTexture::Filter::Linear);                   // good
    //m_texture->setMinificationFilter( QOpenGLTexture::Filter::LinearMipMapLinear);       // a little fuzzy
    //m_texture->setMinificationFilter( QOpenGLTexture::Filter::LinearMipMapNearest);        // good

    //m_texture->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);                  // no anti aliasing
    //m_texture->setMagnificationFilter(QOpenGLTexture::Filter::NearestMipMapLinear);      //
    //m_texture->setMagnificationFilter(QOpenGLTexture::Filter::NearestMipMapNearest);     //
    m_texture->setMagnificationFilter(QOpenGLTexture::Filter::Linear);                   //
    //m_texture->setMagnificationFilter(QOpenGLTexture::Filter::LinearMipMapLinear);       // good
    //m_texture->setMagnificationFilter(QOpenGLTexture::Filter::LinearMipMapNearest);        // good

    m_texture->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);             // !!! May need to fixed border artifacts?

    m_width =  m_texture->width();
    m_height = m_texture->height();
    m_texture_loaded = true;
}











