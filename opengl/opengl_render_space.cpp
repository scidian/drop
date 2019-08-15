//
//      Created by Stephens Nunnally on 6/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <vector>

#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_thing_fire.h"
#include "engine/engine_thing_fisheye.h"
#include "engine/engine_thing_light.h"
#include "engine/engine_thing_water.h"
#include "engine/engine_thing_object.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "enums_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Render, Paint the Scene
//####################################################################################
// Before rendering 3D objects, enable face culling for triangles facing away from view
void DrOpenGL::cullingOn() {    glEnable( GL_CULL_FACE );   glCullFace(  GL_BACK );     glFrontFace( GL_CCW ); }
// Turn off culling before drawing 2D quads, #NOTE: Must turn OFF culling for QPainter to work
void DrOpenGL::cullingOff() {   glDisable( GL_CULL_FACE ); }

// Renders All Scene Objects
void DrOpenGL::drawSpace() {

    // Keeps track of if we have rendered the lights yet
    bool has_rendered_glow_lights = false;

    // This variable was put in so that multiple Water things drawn next to each other will use the same copy of the render fbo as it currently was,
    //      this saves lots of blit calls, and stops some vertical fragments from appearing as they would try to refract each other
    DrThingType last_thing = DrThingType::None;

    // ********** Render 2D Objects
    for (auto thing : m_engine->getCurrentWorld()->getThings()) {

        // ***** When we have gone past glow z_order, draw the lights to the scene
        if (!has_rendered_glow_lights && (thing->z_order > m_engine->getCurrentWorld()->getGlowZOrder())) {
            has_rendered_glow_lights = drawGlowBuffer();
            last_thing = DrThingType::None;
        }

        // ***** Draw Thing with appropriate Shader
        switch (thing->getThingType()) {
            case DrThingType::Object:
                drawObject(thing, last_thing);
                break;
            case DrThingType::Fire:
                drawObjectFire(thing, last_thing);
                break;
            case DrThingType::Light:
            case DrThingType::Water:
            case DrThingType::Fisheye:
                drawEffect(thing, last_thing);
                break;
        }
    }

    // ***** If we didn't draw Glow Lights yet, do it now
    if (!has_rendered_glow_lights) drawGlowBuffer();
}



//####################################################################################
//##        Draws a DrEngineThing effect type with proper shader
//####################################################################################
void DrOpenGL::drawEffect(DrEngineThing *thing, DrThingType &last_thing) {

    // ***** If Light, draw with seperate Light Shader, then move to next Thing
    if (thing->getThingType() == DrThingType::Light) {
        DrEngineLight *light = dynamic_cast<DrEngineLight*>(thing);
        if (light) {
            if (!light->isInView()) return;

            if (light->light_type == Light_Type::Opaque) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);          // Standard blend function
                draw2DLight(light);
                last_thing = DrThingType::Light;
            } else if (light->light_type == Light_Type::Glow) {

            }
        }
        return;
    }

    // ***** If Water, draw with seperate Water Shader, then move to next Thing
    if (thing->getThingType() == DrThingType::Water) {
        DrEngineWater *water = dynamic_cast<DrEngineWater*>(thing);
        if (water) {
            if (last_thing != DrThingType::Water) {
                releaseOffscreenBuffer();
                QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_render_fbo);
                bindOffscreenBuffer(false);
            }
            glDisable(GL_BLEND);
            drawFrameBufferUsingWaterShader(m_texture_fbo, water);
            last_thing = DrThingType::Water;
        }
        return;
    }

    // ***** If Fisheye, draw with seperate Fisheye Shader, then move to next Thing
    if (thing->getThingType() == DrThingType::Fisheye) {
        DrEngineFisheye *lens = dynamic_cast<DrEngineFisheye*>(thing);
        if (lens) {
            if (last_thing != DrThingType::Fisheye) {
                releaseOffscreenBuffer();
                QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_render_fbo);
                bindOffscreenBuffer(false);
            }
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);          // Standard blend function
            drawFrameBufferUsingFisheyeShader(m_texture_fbo, lens);
            last_thing = DrThingType::Fisheye;
        }
        return;
    }
}


//####################################################################################
//##        Draws Glow Buffer onto screen buffer
//####################################################################################
bool DrOpenGL::drawGlowBuffer() {

    Blend_Mode mode = m_engine->getCurrentWorld()->getGlowBlendMode();

    // If no lights and ambient light is 50% (normal) exit now
    double ambient_light = m_engine->getCurrentWorld()->getAmbientLight() / 100.0;
    if (m_glow_lights.count() <= 0 && Dr::IsCloseTo(0.5, ambient_light, .001) && mode == Blend_Mode::Standard) return true;

    // Blend functions, in order of best
    if (mode == Blend_Mode::Standard) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);                        // Best Light blend function, a little but of oversaturation, but looks nice
        ///glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);           // Standard blend function
        ///glBlendFunc(GL_DST_COLOR, GL_ZERO);                          // "Screen" (slembcke) light blend function. Good, but doesnt get brighter than texture colors
        ///glBlendFunc(GL_ONE, GL_ONE);                                 // Supposed to add lights together, not really great for lighting though
        drawFrameBufferUsingKernelShader(m_glow_fbo);

    } else {
        // Copy fbo to a GL_TEXTURE_2D (non multi-sampled) FBO for use as a texture during Screen Shader, Draw Glow Light buffer using Screen Shader
        releaseOffscreenBuffer();
        QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_render_fbo);
        bindOffscreenBuffer();
        glDisable(GL_BLEND);
        drawFrameBufferUsingScreenShader(m_glow_fbo, m_texture_fbo, mode);
    }

    return true;
}












