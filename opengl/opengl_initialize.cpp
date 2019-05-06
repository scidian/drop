//
//      Created by Stephens Nunnally on 5/5/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPainter>

#include "engine/engine.h"
#include "opengl/opengl.h"

//####################################################################################
//##        Initialize OpenGL Resources
//####################################################################################
void OpenGL::initializeGL() {
    // Connect to aboutToBeDestroyed signal to make sure we clean up our resources
    // context() and QOpenGLContext::currentContext() are equivalent when called from initializeGL or paintGL.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &OpenGL::cleanUp);

    // Set up the rendering context, load shaders and other resources, etc.:
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 1.0f);

    // Load resources
    m_engine->t_ball =        initTexture(":/assets/test_images/ball_1.png");
    m_engine->t_metal_block = initTexture(":/assets/test_images/metal_block.png");
    m_engine->t_moon_plant =  initTexture(":/assets/test_images/moon_plant_6.png");
    m_engine->t_rover_body =  initTexture(":/assets/test_images/rover_body.png");
    m_engine->t_rover_wheel = initTexture(":/assets/test_images/rover_wheel.png");
    m_engine->t_spare_wheel = initTexture(":/assets/test_images/spare_wheel.png");

    // Initialize our basic shader, shaders have 2 parts, a Vertex shader followed by a Fragment shader
    QOpenGLShader vShader( QOpenGLShader::Vertex );     vShader.compileSourceFile( ":/shaders/vShader.glsl" );
    QOpenGLShader fShader( QOpenGLShader::Fragment );   fShader.compileSourceFile( ":/shaders/fShader.glsl" );
    m_program.addShader( &vShader );
    m_program.addShader( &fShader );
    m_program.link();

    m_vertexAttr =      m_program.attributeLocation( "vertex" );
    m_texCoordAttr =    m_program.attributeLocation( "texCoord" );
    m_matrixUniform =   m_program.uniformLocation(   "matrix" );
    m_texUniform =      m_program.uniformLocation(   "tex" );
}


//####################################################################################
//##        Loads a texture
//####################################################################################
QOpenGLTexture* OpenGL::initTexture(QString image_name) {

    // Load image, NOTE: QImage is mirrored vertically to account for the fact that OpenGL and QImage use opposite directions for the y axis
    QImage image = QImage(image_name).mirrored();
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
    QOpenGLTexture *texture = new QOpenGLTexture( one_pixel_border.toImage() );

    ///QOpenGLTexture* tile = new QOpenGLTexture(QOpenGLTexture::Target2D);
    ///tile->setSize(256, 256);
    ///tile->setFormat(QOpenGLTexture::RGBA8_UNorm);
    ///// you can manage the number of mimap you desire...
    ///// by default 256x256 => 9 mipmap levels will be allocated: 256, 128, 64, 32, 16, 8, 4, 2 and 1px to modify this use tile->setMipLevels(n);
    ///tile->setMinificationFilter(QOpenGLTexture::Nearest);
    ///tile->setMagnificationFilter(QOpenGLTexture::Nearest);
    ///tile->setData(image, QOpenGLTexture::GenerateMipMaps);

    // These mip map filters allow for nice alpha channels
    texture->setMinificationFilter( QOpenGLTexture::Filter::Nearest);
    texture->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);
    ///texture->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);             // !!! May need to fixed border artifacts?

    return texture;
}












