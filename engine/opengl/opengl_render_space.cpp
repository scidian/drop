//
//      Created by Stephens Nunnally on 6/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QOpenGLFramebufferObject>

#include <vector>

#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_texture.h"
#include "engine/enums_engine.h"
#include "engine/form_engine.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/engine_thing_fire.h"
#include "engine/thing/engine_thing_fisheye.h"
#include "engine/thing/engine_thing_light.h"
#include "engine/thing/engine_thing_mirror.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/thing/engine_thing_swirl.h"
#include "engine/thing/engine_thing_water.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Triangle Culling On / Off
//##        Before rendering 3D objects, enable face culling for triangles facing away from view
//##        Turn off culling before drawing 2D quads
//##        !!!!! #NOTE: Must turn OFF culling for QPainter to work
//####################################################################################
void DrOpenGL::cullingOn(bool reversed) {
    glEnable( GL_CULL_FACE );
    glCullFace(  GL_BACK );
    if (reversed)
        glFrontFace( GL_CW );
    else
        glFrontFace( GL_CCW );
}
void DrOpenGL::cullingOff() {
    glDisable( GL_CULL_FACE );
}


//####################################################################################
//##    Render all Scene Objects
//####################################################################################
void DrOpenGL::drawSpace() {

    // ***** Reset Frame Variables
    bool   has_rendered_glow_lights = false;                                            // Keeps track of if we have rendered the lights yet
    long   thing_count =  0;
    long  &effect_count = m_engine->getCurrentWorld()->effect_count;
    effect_count = 0;
    m_triangles =  0;                                                                   // Reset frame triangle count

    // Used to stop z fighting (a.k.a z-fighting, stitching)
    double z_divisor = (combinedZoomScale() < 0.001f) ? 0.001 : static_cast<double>(combinedZoomScale());
    double last_z;
    m_add_z = 0.0;

    // This variable was put in so that multiple Water things drawn next to each other will use the same copy of the render fbo as it currently was,
    //      this saves lots of blit calls, and stops some vertical fragments from appearing as they would try to refract each other
    DrThingType last_thing = DrThingType::None;


    // ***** Get Things
    EngineThings &things = m_engine->getCurrentWorld()->getThings();
    // Sort by camera distance
    /**
    std::sort(things.begin(), things.end(), [this] (DrEngineThing *a, DrEngineThing *b) {
        glm::vec3 cam_position =     glm::vec3(this->m_eye.x(), this->m_eye.y(), this->m_eye.z());
        glm::vec3 thing_position_a = glm::vec3(a->getPosition().x, a->getPosition().y, a->getZOrder());
        glm::vec3 thing_position_b = glm::vec3(b->getPosition().x, b->getPosition().y, b->getZOrder());
        double cam_distance_a = abs( static_cast<double>(glm::distance(thing_position_a, cam_position)) );
        double cam_distance_b = abs( static_cast<double>(glm::distance(thing_position_b, cam_position)) );
        return cam_distance_b < cam_distance_a;
    }); */


    // ********** Render 2D Objects
    for (auto thing : things) {

        // ***** When we have gone past glow z_order, draw the lights to the scene
        if (!has_rendered_glow_lights && (thing->getZOrder() > m_engine->getCurrentWorld()->getGlowZOrder())) {
            has_rendered_glow_lights = drawGlowBuffer();
            last_thing = DrThingType::None;
        }      

        // ***** Draw Thing with appropriate Shader
        switch (thing->getThingType()) {
            case DrThingType::Character:
            case DrThingType::Object: {
                DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
                if (object == nullptr) continue;
                if (object->isPhysicsChild()) continue;

                // ***** If in 2D Mode (Object has no Depth, World is in 2D, etc) just draw quad
                bool draw2D = m_engine->getCurrentWorld()->render_mode == Render_Mode::Mode_2D;
                if (thing->comp3D() == nullptr)
                    draw2D = true;
                else if (thing->comp3D()->get3DType() == Convert_3D_Type::None || Dr::FuzzyCompare(thing->comp3D()->getDepth(), 0.0))
                    draw2D = true;
                else if (thing->extrude_3d)
                    draw2D = false;

                // ***** Handle Soft Body
                if (object->body_style == Body_Style::Circular_Blob || object->body_style == Body_Style::Square_Blob) {
                    if (object->compSoftBody() == nullptr) continue;
                    if (object->compSoftBody()->calculateSoftBodyMesh(object->body_style, Soft_Mesh_Style::Radial) == false) continue;
                } else if (object->body_style == Body_Style::Mesh_Blob) {
                    if (object->compSoftBody() == nullptr) continue;
                    if (object->compSoftBody()->calculateSoftBodyMesh(object->body_style, Soft_Mesh_Style::Grid_Square) == false) continue;
                }
                if (object->body_style != Body_Style::Rigid_Body) {
                    cullingOn(false);
                    draw2D = true;
                }

                // ***** Trying to stop z-fighting, move objects apart with same z_order
                //       ...The further away the more they have to move since the precision of the z buffer is scaled away from the camera
                if (draw2D == false) {
                    // Calculate distance from thing to camera
                    glm::vec3 thing_position = glm::vec3(thing->getPosition().x, thing->getPosition().y, thing->getZOrder());
                    glm::vec3 cam_position =   glm::vec3(m_eye.x(), m_eye.y(), m_eye.z());
                    double cam_distance = 1.0 + (static_cast<double>(glm::distance(thing_position, cam_position)) / 400.0);

                    // Adjust offset we're going to add onto things location to stop z-fighting
                    double z_spacing = (0.0005 / z_divisor) * cam_distance;
                    if (thing_count == 0) last_z = thing->getZOrder() - 1000.0;
                    if (Dr::IsCloseTo(last_z, thing->getZOrder(), 0.01)) m_add_z += z_spacing; else m_add_z = 0.0;
                    last_z = thing->getZOrder();
                }

                // ***** Draw Object / Character
                if (draw2D) {
                    // Standard Shader
                    drawObject(thing, last_thing, draw2D);
                    // Simple Shader
                    ///glEnable(GL_DEPTH_TEST);
                    ///drawObjectSimple(thing);
                    ///glDisable(GL_DEPTH_TEST);
                } else {
                    cullingOn(false);
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LEQUAL);
                    drawObject(thing, last_thing, draw2D);
                    glDepthMask(GL_TRUE);
                    glDisable(GL_DEPTH_TEST);
                }
                cullingOff();
                break;
            }
            case DrThingType::Camera:
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
    if (m_glow_lights.size() <= 0 && Dr::IsCloseTo(0.5, ambient_light, .001) && mode == Blend_Mode::Standard) return true;

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











