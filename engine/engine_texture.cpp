//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QPixmap>
#include <QOpenGLTexture>

#include "editor/pixmap/pixmap.h"
#include "engine/engine.h"
#include "engine/engine_texture.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrEngineTexture::DrEngineTexture(QPixmap &from_pixmap) {
    loadTexture(from_pixmap);
}


//####################################################################################
//##    Loads a texture
//####################################################################################
void DrEngineTexture::loadTexture(QPixmap &from_pixmap) {

    // Calculate and save average color of texture
    m_average_color = Dr::AverageColor(from_pixmap);

    // Load image - #NOTE: QImage is mirrored vertically to account for the fact that OpenGL and QImage use opposite directions for the y axis
    QImage image = from_pixmap.toImage().mirrored();
    image = image.convertToFormat( QImage::Format_ARGB32 );                                             // Convert image to a standard format
    ///image = image.convertToFormat( QImage::Format_ARGB32_Premultiplied );                            // Doesn't work right for our textures, causes white outlines
    image = Dr::ApplySinglePixelFilter( Image_Filter_Type::Premultiplied_Alpha, image, 0 );             // Create alpha premultiplied texture

    // Create new texture
    m_texture = new QOpenGLTexture( image );

    ///QOpenGLTexture* m_texture = new QOpenGLTexture(QOpenGLTexture::Target2DArray);
    ///m_texture->setSize(one_pixel_border.width(), one_pixel_border.height());
    ///m_texture->setFormat(QOpenGLTexture::RGBAFormat); ;//)  RGBA8_UNorm);
    ///// you can manage the number of mimap you desire...
    ///// by default 256x256 => 9 mipmap levels will be allocated: 256, 128, 64, 32, 16, 8, 4, 2 and 1px to modify this use tile->setMipLevels(n);
    ///m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    ///m_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    ///m_texture->setData(one_pixel_border.toImage(), QOpenGLTexture::GenerateMipMaps);

    // There are six options for Minification filter
//    m_texture->setMinificationFilter( QOpenGLTexture::Filter::Nearest);                 // no anti aliasing
    ///m_texture->setMinificationFilter( QOpenGLTexture::Filter::NearestMipMapLinear);     // small is edgy
    ///m_texture->setMinificationFilter( QOpenGLTexture::Filter::NearestMipMapNearest);    // small is edgy
    m_texture->setMinificationFilter( QOpenGLTexture::Filter::Linear);                     // good
    ///m_texture->setMinificationFilter( QOpenGLTexture::Filter::LinearMipMapLinear);      // messes up pixelize filter
    ///m_texture->setMinificationFilter( QOpenGLTexture::Filter::LinearMipMapNearest);     // messes up pixelize filter

    // There are only two options for Magnification filter
//    m_texture->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);
    m_texture->setMagnificationFilter(QOpenGLTexture::Filter::Linear);

    m_texture->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);                          // !!! Need this to fix border artifacts

    m_width =  m_texture->width();
    m_height = m_texture->height();
    m_texture_loaded = true;
}











