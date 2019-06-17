//
//      Created by Stephens Nunnally on 6/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QOpenGLFramebufferObject>

#include "opengl.h"


//####################################################################################
//##        Allocate shadow Frame Buffer Object
//####################################################################################
void OpenGL::bindShadowBuffer() {
    int lightSize = 256;        // test size

    // Check if exists first
    if (!m_shadow_fbo) {
        delete m_shadow_fbo;

        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::Attachment::NoAttachment);
        format.setTextureTarget(GL_TEXTURE_2D);                      // This is set automatically, cannot be gl_texture_2d if multisampling is enabled
        format.setInternalTextureFormat(GL_RGBA32F_ARB);             // This is set automatically depending on the system
        m_shadow_fbo =  new QOpenGLFramebufferObject(lightSize, lightSize, format);
    }
    m_shadow_fbo->bind();

    // Clear the FBO
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

}
