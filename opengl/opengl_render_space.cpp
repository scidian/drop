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
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "engine_things/engine_thing_fire.h"
#include "engine_things/engine_thing_fisheye.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_object.h"
#include "engine_things/engine_thing_mirror.h"
#include "engine_things/engine_thing_swirl.h"
#include "engine_things/engine_thing_water.h"
#include "enums_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##    Triangle Culling On / Off
//##        Before rendering 3D objects, enable face culling for triangles facing away from view
//##        Turn off culling before drawing 2D quads
//##        !!!!! #NOTE: Must turn OFF culling for QPainter to work
//####################################################################################
void DrOpenGL::cullingOn() {
    glEnable( GL_CULL_FACE );
    glCullFace(  GL_BACK );
    glFrontFace( GL_CCW );
}
void DrOpenGL::cullingOff() {
    glDisable( GL_CULL_FACE );
}

//####################################################################################
//##    Wireframe Rendering
//####################################################################################
void DrOpenGL::wireframeOn(bool smooth) {
#if not defined (Q_OS_IOS)
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );                        // Wireframe on
    if (smooth) glEnable(GL_LINE_SMOOTH);                               // Enables line anti-aliasing
    glLineWidth(1);                                                     // Only '1' fully supported by OpenGL standard
#else
    Q_UNUSED(smooth)
#endif
}
void DrOpenGL::wireframeOff(bool smooth) {
#if not defined (Q_OS_IOS)
    if (smooth) glDisable(GL_LINE_SMOOTH);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );                        // Wireframe off
#else
    Q_UNUSED(smooth)
#endif
}


//####################################################################################
//##    Render all Scene Objects
//####################################################################################
void DrOpenGL::drawSpace() {

    // ***** Reset Frame Variables
    m_triangles = 0;                                                    // Reset frame triangle count
    bool has_rendered_glow_lights = false;                              // Keeps track of if we have rendered the lights yet

    // This variable was put in so that multiple Water things drawn next to each other will use the same copy of the render fbo as it currently was,
    //      this saves lots of blit calls, and stops some vertical fragments from appearing as they would try to refract each other
    DrThingType last_thing = DrThingType::None;

    // ********** Render 2D Objects
    long  &effect_count = m_engine->getCurrentWorld()->effect_count;
           effect_count = 0;
    long   thing_count = 0;
    double last_z; m_add_z = 0.0;                                       // Used to stop z fighting (a.k.a z-fighting, stitching)
    for (auto thing : m_engine->getCurrentWorld()->getThings()) {

        if (thing_count == 0) last_z = thing->z_order - 1000.0;
        if (Dr::IsCloseTo(last_z, thing->z_order, 0.001)) m_add_z += 0.05; else m_add_z = 0.0;
        last_z = thing->z_order;

        // ***** When we have gone past glow z_order, draw the lights to the scene
        if (!has_rendered_glow_lights && (thing->z_order > m_engine->getCurrentWorld()->getGlowZOrder())) {
            has_rendered_glow_lights = drawGlowBuffer();
            last_thing = DrThingType::None;
        }      

        // ***** Draw Thing with appropriate Shader
        bool draw2D;
        switch (thing->getThingType()) {
            case DrThingType::Character:
            case DrThingType::Object:
                if (m_engine->getCurrentWorld()->wireframe) wireframeOn();

                // If no depth to object, or if in Orthographic mode and object is not rotated on X or Y axis, just draw front face
                draw2D = qFuzzyCompare(thing->getDepth(), 0.0) ||
                         (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic &&
                          qFuzzyCompare(thing->getAngleX(), 0.0)       && qFuzzyCompare(thing->getAngleY(), 0.0) &&
                          qFuzzyCompare(thing->getRotateSpeedX(), 0.0) && qFuzzyCompare(thing->getRotateSpeedY(), 0.0));
                if (draw2D) {
                    drawObject(thing, last_thing, draw2D);
                } else {

                    cullingOn();
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);
                    drawObject(thing, last_thing, draw2D);
                    glDisable(GL_DEPTH_TEST);
                    cullingOff();
                }

                if (m_engine->getCurrentWorld()->wireframe) wireframeOff();
                break;
            case DrThingType::Text:
                // !!!!!
                // !!!!! IMPLEMENT DRAW TEXT HERE !!!!!
                // !!!!!
                break;
            case DrThingType::Fire:
                if (drawObjectFire(thing, last_thing)) ++effect_count;
                break;
            case DrThingType::Fisheye:
            case DrThingType::Light:
            case DrThingType::Mirror:
            case DrThingType::Swirl:
            case DrThingType::Water:
                if (drawEffect(thing, last_thing)) ++effect_count;
                break;
            case DrThingType::None:
                break;
        }

        ++thing_count;
    }

    // ***** If we didn't draw Glow Lights yet, do it now
    if (!has_rendered_glow_lights) drawGlowBuffer();
}


//####################################################################################
//##    Draws Glow Buffer onto screen buffer
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


//####################################################################################
//##    Draws a DrEngineThing effect type with proper shader
//####################################################################################
bool DrOpenGL::drawEffect(DrEngineThing *thing, DrThingType &last_thing) {

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
            if (drawFrameBufferUsingFisheyeShader(m_texture_fbo, lens)) {
                last_thing = DrThingType::Fisheye;
                return true;
            }
        }
    }

    // ***** If Light, draw with seperate Light Shader, then move to next Thing
    if (thing->getThingType() == DrThingType::Light) {
        DrEngineLight *light = dynamic_cast<DrEngineLight*>(thing);
        if (light) {
            if (!light->isInView()) return false;

            if (light->light_type == Light_Type::Opaque) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);          // Standard blend function
                if (draw2DLight(light)) {
                    last_thing = DrThingType::Light;
                    return true;
                }
            } else if (light->light_type == Light_Type::Glow) {

            }
        }
    }

    // ***** If Mirror, draw with seperate Mirror Shader, then move to next Thing
    if (thing->getThingType() == DrThingType::Mirror) {
        DrEngineMirror *mirror = dynamic_cast<DrEngineMirror*>(thing);
        if (mirror) {
            if (last_thing != DrThingType::Mirror) {
                releaseOffscreenBuffer();
                QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_render_fbo);
                bindOffscreenBuffer(false);
            }
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);          // Standard blend function
            if (drawFrameBufferUsingMirrorShader(m_texture_fbo, mirror)) {
                last_thing = DrThingType::Mirror;
                return true;
            }
        }
    }

    // ***** If Swirl, draw with seperate Swirl Shader, then move to next Thing
    if (thing->getThingType() == DrThingType::Swirl) {
        DrEngineSwirl *swirl = dynamic_cast<DrEngineSwirl*>(thing);
        if (swirl) {
            if (last_thing != DrThingType::Swirl) {
                releaseOffscreenBuffer();
                QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_render_fbo);
                bindOffscreenBuffer(false);
            }
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);          // Standard blend function
            if (drawFrameBufferUsingSwirlShader(m_texture_fbo, swirl)) {
                last_thing = DrThingType::Swirl;
                return true;
            }
        }
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
            if (drawFrameBufferUsingWaterShader(m_texture_fbo, water)) {
                last_thing = DrThingType::Water;
                return true;
            }
        }
    }

    return false;
}











