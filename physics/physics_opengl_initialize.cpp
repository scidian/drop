//
//      Created by Stephens Nunnally on 5/5/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPainter>

#include "forms/form_engine.h"
#include "physics/physics_opengl.h"


//####################################################################################
//##        Initialize OpenGL Resources
//####################################################################################
void PhysicsOpenGL::initializeGL() {
    // Connect to aboutToBeDestroyed signal to make sure we clean up our resources
    // context() and QOpenGLContext::currentContext() are equivalent when called from initializeGL or paintGL.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &PhysicsOpenGL::cleanUp);

    // Set up the rendering context, load shaders and other resources, etc.:
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 1.0f);

    // Load resources
    m_parent->t_ball =        initTexture(":/assets/test_images/ball_1.png");
    m_parent->t_metal_block = initTexture(":/assets/test_images/metal_block.png");
    m_parent->t_moon_plant =  initTexture(":/assets/test_images/moon_plant_6.png");
    m_parent->t_rover_body =  initTexture(":/assets/test_images/rover_body.png");
    m_parent->t_rover_wheel = initTexture(":/assets/test_images/rover_wheel.png");
    m_parent->t_spare_wheel = initTexture(":/assets/test_images/spare_wheel.png");

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
QOpenGLTexture* PhysicsOpenGL::initTexture(QString image_name) {

    // Load image mirrored
    QImage image = QImage(image_name).mirrored();
    if ( image.format() != QImage::Format::Format_ARGB32 )
        image = image.convertToFormat( QImage::Format_ARGB32 );

    // Add a "c_texture_border" pixel border to reduce artifacts during multi sampling
    QPixmap one_pixel_border( image.width() + (c_texture_border * 2), image.height() + (c_texture_border * 2));
    one_pixel_border.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&one_pixel_border);
    painter.drawPixmap( c_texture_border, c_texture_border, image.width(), image.height(), QPixmap::fromImage(image), 0, 0, image.width(), image.height());
    painter.end();

    // Create new texture
    QOpenGLTexture* texture = new QOpenGLTexture( one_pixel_border.toImage() );

    // These mip map filters allow for nice alpha channels
    texture->setMinificationFilter( QOpenGLTexture::Filter::Nearest);
    texture->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);

    return texture;
}












