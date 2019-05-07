//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPainter>
#include <QPixmap>

#include "engine.h"
#include "engine_texture.h"

//####################################################################################
//##        Constructor
//####################################################################################
EngineTexture::EngineTexture(QString from_asset_string) {
    loadTexture(from_asset_string);
}


//####################################################################################
//##        Loads a texture
//####################################################################################
void EngineTexture::loadTexture(QString from_asset_string) {

    // Load image, NOTE: QImage is mirrored vertically to account for the fact that OpenGL and QImage use opposite directions for the y axis
    QImage image = QImage(from_asset_string).mirrored();
    if (image.format() != QImage::Format::Format_ARGB32 )
        image = image.convertToFormat( QImage::Format_ARGB32 );

    // Add a "c_texture_border" pixel border to reduce artifacts during multi sampling
    QPixmap one_pixel_border( image.width() + (c_texture_border * 2), image.height() + (c_texture_border * 2));
    one_pixel_border.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&one_pixel_border);
    painter.drawPixmap( c_texture_border, c_texture_border, image.width(), image.height(), QPixmap::fromImage(image), 0, 0, image.width(), image.height());
    painter.end();

    // Create new texture
    m_texture = new QOpenGLTexture( one_pixel_border.toImage() );

    ///QOpenGLTexture* tile = new QOpenGLTexture(QOpenGLTexture::Target2D);
    ///tile->setSize(256, 256);
    ///tile->setFormat(QOpenGLTexture::RGBA8_UNorm);
    ///// you can manage the number of mimap you desire...
    ///// by default 256x256 => 9 mipmap levels will be allocated: 256, 128, 64, 32, 16, 8, 4, 2 and 1px to modify this use tile->setMipLevels(n);
    ///tile->setMinificationFilter(QOpenGLTexture::Nearest);
    ///tile->setMagnificationFilter(QOpenGLTexture::Nearest);
    ///tile->setData(image, QOpenGLTexture::GenerateMipMaps);

    // These mip map filters allow for nice alpha channels
    m_texture->setMinificationFilter( QOpenGLTexture::Filter::Nearest);
    m_texture->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);
    ///m_texture->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);             // !!! May need to fixed border artifacts?

    m_width =  m_texture->width();
    m_height = m_texture->height();
    m_texture_loaded = true;
}













