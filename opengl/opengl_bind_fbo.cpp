//
//      Created by Stephens Nunnally on 7/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_world.h"
#include "engine_things/engine_thing_light.h"
#include "opengl.h"


//####################################################################################
//##        Allocate / Bind Off Screen FBO
//##            Used for offscreen rendering, allows for entire scene to have post
//##            processing shaders applied at one time
//####################################################################################
void DrOpenGL::bindOffscreenBuffer(bool clear) {
    // Check that off screen buffers are initialized
    if (!m_render_fbo || !m_texture_fbo ||
         m_render_fbo->width()  != width()*devicePixelRatio() ||
         m_render_fbo->height() != height()*devicePixelRatio()) {
        delete m_render_fbo;
        delete m_texture_fbo;
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::Attachment::CombinedDepthStencil);
        format.setSamples(4);
        ///format.setTextureTarget(GL_TEXTURE_2D);                      // This is set automatically, cannot be gl_texture_2d if multisampling is enabled
        ///format.setInternalTextureFormat(GL_RGBA32F_ARB);             // This is set automatically depending on the system
        ///format.setMipmap(true);                                      // Don't need
        m_render_fbo =  new QOpenGLFramebufferObject(width() * devicePixelRatio(), height() * devicePixelRatio(), format);

        QOpenGLFramebufferObjectFormat format2;
        format2.setAttachment(QOpenGLFramebufferObject::Attachment::NoAttachment);
        m_texture_fbo = new QOpenGLFramebufferObject(width() * devicePixelRatio(), height() * devicePixelRatio(), format2);
    }
    m_render_fbo->bind();

    // Clear the buffers
    if (clear) {
        float background_red =   static_cast<float>(m_engine->getCurrentWorld()->getBackgroundColor().redF());
        float background_green = static_cast<float>(m_engine->getCurrentWorld()->getBackgroundColor().greenF());
        float background_blue =  static_cast<float>(m_engine->getCurrentWorld()->getBackgroundColor().blueF());
        glClearColor(background_red, background_green, background_blue, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    // Enable anti aliasing if not on mobile
#if not defined(Q_OS_ANDROID) && not defined(Q_OS_IOS)
    glEnable( GL_MULTISAMPLE );
#endif

}

//####################################################################################
//##        Relase Off Screen Frame Buffer Object
//####################################################################################
void DrOpenGL::releaseOffscreenBuffer() {
    m_render_fbo->release();

    // Turn off anti aliasing if not on mobile
#if not defined(Q_OS_ANDROID) && not defined(Q_OS_IOS)
    glDisable( GL_MULTISAMPLE );
#endif
}


//####################################################################################
//##        Allocate / Bind Glow Light FBO
//##            Glow Light FBO is used to render all glow lights at once
//####################################################################################
void DrOpenGL::bindGlowLightsBuffer(float ambient_light) {
    // Check that off screen buffers are initialized
    if (!m_glow_fbo || (m_glow_fbo->width() != width()*devicePixelRatio() || m_glow_fbo->height() != height()*devicePixelRatio())) {
        delete m_glow_fbo;
        QOpenGLFramebufferObjectFormat format2;
        format2.setAttachment(QOpenGLFramebufferObject::Attachment::NoAttachment);
        m_glow_fbo = new QOpenGLFramebufferObject(width() * devicePixelRatio(), height() * devicePixelRatio(), format2);
    }
    m_glow_fbo->bind();

    // Clear the buffer
    glClearColor(ambient_light, ambient_light, ambient_light, 0.0f);
    ///glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

//####################################################################################
//##        Allocate / Bind Occluder Map FBO
//##            Occluder Map FBO is used to draw objects onto a buffer that lights can copy to cast shadows onto.
//##            The objects are drawn using pixels encoded with their respective z order.
//####################################################################################
void DrOpenGL::bindOccluderMapBuffer() {
    int desired_x, desired_y;

    // Orthographic we can use a larger buffer size
    if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) {
        desired_x = g_max_occluder_fbo_size;
        desired_y = g_max_occluder_fbo_size;

    // Perspective uses same size as off screen buffer to match view angles
    } else {
        desired_x = width()* devicePixelRatio();
        desired_y = height()*devicePixelRatio();
    }

    if (!m_occluder_fbo || (m_occluder_fbo->width() != desired_x || m_occluder_fbo->height() != desired_y)) {
        delete m_occluder_fbo;
        m_occluder_fbo =  new QOpenGLFramebufferObject(desired_x, desired_y);
    }
    m_occluder_fbo->bind();

    // Clear the buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


//####################################################################################
//##        Allocate / Bind Light Occluder FBO (For Each Light)
//##            Light Occluder FBO's are the size of the light to be drawn, they have their location part of the
//##            Occluder Map FBO copied on to them to be processed with the Shadow Map Frag shader to produce a 1D shadow map.
//####################################################################################
void DrOpenGL::bindLightOcculderBuffer(DrEngineLight *light) {
    // Check Frame Buffer Object is initialized
    bool need_to_create_new = false;
    if (!(m_occluders[light->getKey()]))
        need_to_create_new = true;
    else if (m_occluders[light->getKey()]->width()  != light->getLightDiameterFitted() ||
             m_occluders[light->getKey()]->height() != light->getLightDiameterFitted())
        need_to_create_new = true;

    // Initialize light fbo
    if (need_to_create_new) {
        delete m_occluders[light->getKey()];
        m_occluders[light->getKey()] = new QOpenGLFramebufferObject(light->getLightDiameterFitted(), light->getLightDiameterFitted());
    }

    // Bind and clear buffer
    m_occluders[light->getKey()]->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


//####################################################################################
//##        Allocate / Bind Shadow FBO (For Each Light)
//##            A 1D shadow map fbo for each light. The pixels are encoded with the distance from the light center to the nearest shadow
//##            caster extending out at the respective angle to the shadow maps width (0 to width == 0 to 360 degrees)
//####################################################################################
void DrOpenGL::bindLightShadowBuffer(DrEngineLight *light) {
    // Shadow map size is the smallest of c_angles, light_radius_fitted, and width()
    int shadow_size = (light->getLightDiameterFitted() < g_max_rays) ? light->getLightDiameterFitted() : g_max_rays;
        shadow_size = (width()*devicePixelRatio() < shadow_size) ? width()*devicePixelRatio() : shadow_size;

    // Check Frame Buffer Object is initialized
    bool need_to_create_new = false;
    if (m_shadows[light->getKey()] == nullptr)
        need_to_create_new = true;
    else if (m_shadows[light->getKey()]->width() != shadow_size)
        need_to_create_new = true;

    // Initialize shadow fbo
    if (need_to_create_new) {
        delete m_shadows[light->getKey()];
        m_shadows[light->getKey()] = new QOpenGLFramebufferObject(shadow_size, 1);
    }

    // Bind and clear buffer
    m_shadows[light->getKey()]->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}











