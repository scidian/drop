//
//      Created by Stephens Nunnally on 8/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_thing_object.h"
#include "engine/engine_thing_fire.h"
#include "engine/engine_texture.h"
#include "engine/engine_vertex_data.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Draws a DrEngineObject effect type with default shader
//####################################################################################
void DrOpenGL::drawObject(DrEngineThing *thing, DrThingType &last_thing) {
    if (thing->getThingType() != DrThingType::Object) return;
    DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
    if (!object) return;

    // ***** Don't draw Segments (lines)
    ///bool skip_object = false;
    ///for (auto shape : object->shapes) {
    ///    if (object->shape_type[shape] == Shape_Type::Segment)
    ///        skip_object = true;
    ///}
    ///if (skip_object) return;
    if (object->shapes.count() > 0) {
        if (object->shape_type[object->shapes[0]] == Shape_Type::Segment) return;
    }

    // ***** Enable shader program
    if (last_thing != DrThingType::Object) {
        if (!m_default_shader.bind()) return;

        // Blend function
        glEnable(GL_BLEND);
        ///glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);           // Standard non-premultiplied alpha blend
        ///glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);                     // Additive blending
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);                    // Premultiplied alpha blend

        // Fancy Seperate RGB/Alpha Blend Functions
        ///glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);        // Premultiplied alpha blend

        // Set Matrix for Shader, calculates current matrix
        m_default_shader.setUniformValue( u_default_matrix, (m_projection * m_model_view) );

        // Set Texture Coordinates for Shader
        m_default_shader.setAttributeArray(    a_default_texture_coord, m_whole_texture_coordinates.data(), 2 );
        m_default_shader.enableAttributeArray( a_default_texture_coord );
    }

    // ***** Get texture to render with, set texture coordinates
    DrEngineTexture *texture = m_engine->getTexture(object->getTextureNumber());
    if (!texture->texture()->isBound()) {
        texture->texture()->bind();
    }
    float texture_width =  texture->width();
    float texture_height = texture->height();

    // ***** Load vertices for this object
    QVector<GLfloat> vertices;
    getThingVertices(vertices, object, static_cast<float>(object->getAngle()));
    m_default_shader.setAttributeArray(    a_default_vertex, vertices.data(), 3 );
    m_default_shader.enableAttributeArray( a_default_vertex );

    // ***** Set Shader Variables
    m_default_shader.setUniformValue( u_default_texture, 0 );                           // Use texture unit 0

    // Fade away dying object
    float alpha = object->getOpacity();                                                 // Start with object alpha
    if (!object->isAlive() && object->getFadeOnDeath()) {
        double fade_percent = 1.0 - (static_cast<double>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<double>(object->getFadeDelay()));
        alpha *= static_cast<float>(fade_percent);
    }
    m_default_shader.setUniformValue( u_default_alpha,          alpha );
    m_default_shader.setUniformValue( u_default_tint,           0.0f, 0.0f, 0.0f );
    m_default_shader.setUniformValue( u_default_width,          texture_width );
    m_default_shader.setUniformValue( u_default_height,         texture_height );
    m_default_shader.setUniformValue( u_default_time,           static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 1000.0) );
    m_default_shader.setUniformValue( u_default_pre,            true );

    m_default_shader.setUniformValue( u_default_bitrate,        object->bitrate );
    m_default_shader.setUniformValue( u_default_pixel_x,        object->pixel_x );
    m_default_shader.setUniformValue( u_default_pixel_y,        object->pixel_y );
    m_default_shader.setUniformValue( u_default_pixel_offset,   0.0f, 0.0f );
    m_default_shader.setUniformValue( u_default_negative,       object->negative );
    m_default_shader.setUniformValue( u_default_grayscale,      object->grayscale );
    m_default_shader.setUniformValue( u_default_hue,            object->hue );
    m_default_shader.setUniformValue( u_default_saturation,     object->saturation );
    m_default_shader.setUniformValue( u_default_contrast,       object->contrast );
    m_default_shader.setUniformValue( u_default_brightness,     object->brightness );

    m_default_shader.setUniformValue( u_default_cartoon,        false );
    m_default_shader.setUniformValue( u_default_wavy,           false );

    // ***** Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Release bound items
    m_default_shader.disableAttributeArray( a_default_vertex );
    ///m_default_shader.disableAttributeArray( a_default_texture_coord );
    ///m_default_shader.release();

    last_thing = DrThingType::Object;
}





//####################################################################################
//##        Draws a DrEngineObject effect type with default shader
//####################################################################################
void DrOpenGL::drawObjectExtrude(DrEngineThing *thing, DrThingType &last_thing) {
    if (thing->getThingType() != DrThingType::Object) return;
    DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
    if (!object) return;

    // ***** Don't draw Segments (lines)
    if (object->shapes.count() > 0) {
        if (object->shape_type[object->shapes[0]] == Shape_Type::Segment) return;
    }

    // ***** Enable shader program
    if (!m_default_shader.bind()) return;

    // Blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);                    // Premultiplied alpha blend

    // Set Matrix for Shader, calculates current matrix, adds in object location
    QPointF center = thing->getPosition();
    float   x, y, z;
    x = static_cast<float>(center.x());
    y = static_cast<float>(center.y());
    z = static_cast<float>(thing->z_order);
    float now = 0;//static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 30.f);
    float angle = static_cast<float>(object->getAngle());
    QMatrix4x4 matrix;
    matrix.translate(x, y, z);
    matrix.rotate(now, 0.f, 1.f, 0.f);
    matrix.rotate(angle, 0.0, 0.0, 1.0);
    matrix.scale(object->getScaleX(), object->getScaleY(), 1.0);
    m_default_shader.setUniformValue( u_default_matrix, (m_projection * m_model_view * matrix) );


    m_texture_vbos[object->getTextureNumber()]->bind();

    m_default_shader.enableAttributeArray(  PROGRAM_VERTEX_ATTRIBUTE);
    m_default_shader.setAttributeBuffer(    PROGRAM_VERTEX_ATTRIBUTE,   GL_FLOAT, 0                  , 3, c_vertex_length * sizeof(GLfloat));

    m_default_shader.enableAttributeArray(  PROGRAM_TEXCOORD_ATTRIBUTE);
    m_default_shader.setAttributeBuffer(    PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 6 * sizeof(GLfloat), 2, c_vertex_length * sizeof(GLfloat));



    // ***** Get texture to render with, set texture coordinates
    DrEngineTexture *texture = m_engine->getTexture(object->getTextureNumber());
    if (!texture->texture()->isBound()) {
        texture->texture()->bind();
    }
    float texture_width =  texture->width();
    float texture_height = texture->height();

    // ***** Load vertices for this object
    //QVector<GLfloat> vertices;
    //getThingVertices(vertices, object, static_cast<float>(object->getAngle()));
    //m_default_shader.setAttributeArray(    a_default_vertex, vertices.data(), 3 );
    //m_default_shader.enableAttributeArray( a_default_vertex );

    // ***** Set Shader Variables
    m_default_shader.setUniformValue( u_default_texture, 0 );                           // Use texture unit 0

    // Fade away dying object
    float alpha = object->getOpacity();                                                 // Start with object alpha
    if (!object->isAlive() && object->getFadeOnDeath()) {
        double fade_percent = 1.0 - (static_cast<double>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<double>(object->getFadeDelay()));
        alpha *= static_cast<float>(fade_percent);
    }
    m_default_shader.setUniformValue( u_default_alpha,          alpha );
    m_default_shader.setUniformValue( u_default_tint,           0.0f, 0.0f, 0.0f );
    m_default_shader.setUniformValue( u_default_width,          texture_width );
    m_default_shader.setUniformValue( u_default_height,         texture_height );
    m_default_shader.setUniformValue( u_default_time,           static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 1000.0) );
    m_default_shader.setUniformValue( u_default_pre,            true );

    m_default_shader.setUniformValue( u_default_bitrate,        object->bitrate );
    m_default_shader.setUniformValue( u_default_pixel_x,        object->pixel_x );
    m_default_shader.setUniformValue( u_default_pixel_y,        object->pixel_y );
    m_default_shader.setUniformValue( u_default_pixel_offset,   0.0f, 0.0f );
    m_default_shader.setUniformValue( u_default_negative,       object->negative );
    m_default_shader.setUniformValue( u_default_grayscale,      object->grayscale );
    m_default_shader.setUniformValue( u_default_hue,            object->hue );
    m_default_shader.setUniformValue( u_default_saturation,     object->saturation );
    m_default_shader.setUniformValue( u_default_contrast,       object->contrast );
    m_default_shader.setUniformValue( u_default_brightness,     object->brightness );

    m_default_shader.setUniformValue( u_default_cartoon,        false );
    m_default_shader.setUniformValue( u_default_wavy,           false );


    // ***** Draw triangles using shader program

    glDrawArrays(GL_TRIANGLES, 0, m_texture_data[object->getTextureNumber()]->vertexCount() );
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, m_texture_data[object->getTextureNumber()]->vertexCount() );

    m_texture_vbos[object->getTextureNumber()]->release();


    // Release bound items
    m_default_shader.disableAttributeArray( PROGRAM_VERTEX_ATTRIBUTE );
    m_default_shader.disableAttributeArray( PROGRAM_TEXCOORD_ATTRIBUTE );
    m_default_shader.release();

    last_thing = DrThingType::None;
}






//####################################################################################
//##        Renders the light to the using the Shadow Map
//####################################################################################
bool DrOpenGL::drawObjectOccluder(DrEngineThing *thing, bool need_init_shader) {
    if ( thing->getThingType() != DrThingType::Object) return false;
    DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
    if (!object) return false;

    // ***** Don't draw Segments (lines)
    bool skip_object = false;
    for (auto shape : object->shapes) {
        if (object->shape_type[shape] == Shape_Type::Segment)
            skip_object = true;
    }
    if (skip_object) return false;
    if (!object->cast_shadows) return false;

    // ***** Enable shader program
    if (need_init_shader) {
        if (!m_occluder_shader.bind()) return false;

        // Standard blend function
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set Matrix for Shader
        m_occluder_shader.setUniformValue( u_occluder_matrix, occluderMatrix(m_engine->getCurrentWorld()->render_type, c_use_cam_offset) );

        // Set Texture Coordinates for Shader
        m_occluder_shader.setAttributeArray( a_occluder_texture_coord, m_whole_texture_coordinates.data(), 2 );
        m_occluder_shader.enableAttributeArray( a_occluder_texture_coord );
    }

    // ***** Get texture to render with, set texture coordinates
    DrEngineTexture *texture = m_engine->getTexture(object->getTextureNumber());
    if (!texture->texture()->isBound())
        texture->texture()->bind();  

    // ***** Load vertices for this object
    QVector<GLfloat> vertices;
    getThingVertices(vertices, object, static_cast<float>(object->getAngle()));
    m_occluder_shader.setAttributeArray( a_occluder_vertex, vertices.data(), 3 );
    m_occluder_shader.enableAttributeArray( a_occluder_vertex );

    // ***** Set Shader Variables
    m_occluder_shader.setUniformValue( u_occluder_texture, 0 );             // Texture unit 0

    // Fade away dying object, start with object alpha
    float alpha = object->getOpacity();
    if (!object->isAlive() && object->getFadeOnDeath()) {
        double fade_percent = 1.0 - (static_cast<double>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<double>(object->getFadeDelay()));
        alpha *= static_cast<float>(fade_percent);
    }
    m_occluder_shader.setUniformValue( u_occluder_alpha,      alpha );
    m_occluder_shader.setUniformValue( u_occluder_depth,      static_cast<float>(object->z_order) );
    m_occluder_shader.setUniformValue( u_occluder_near_plane, c_near_plane );
    m_occluder_shader.setUniformValue( u_occluder_far_plane,  c_far_plane );

    // ***** Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Release bound items
    m_occluder_shader.disableAttributeArray( a_occluder_vertex );
    ///m_occluder_shader.disableAttributeArray( a_occluder_texture_coord );
    ///m_occluder_shader.release();

    return true;            // Returns true when m_occluder_shader has been initialized
}



//####################################################################################
//##        Uses Fire Shader to draw Fire
//##            - Returns true if rendered, false if not
//####################################################################################
bool DrOpenGL::drawObjectFire(DrEngineThing *thing, DrThingType &last_thing) {
    DrEngineFire *fire = dynamic_cast<DrEngineFire*>(thing);
    if (!fire) return false;

    // ***** Enable shader program
    if (!m_fire_shader.bind()) return false;

    // ***** Blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);           // Standard non-premultiplied alpha blend

    // ***** Set Matrix for Shader, calculates current matrix
    m_fire_shader.setUniformValue( u_fire_matrix, (m_projection * m_model_view) );

    // ***** Set Texture Coordinates for Shader
    m_fire_shader.setAttributeArray( a_fire_texture_coord, m_whole_texture_coordinates.data(), 2 );
    m_fire_shader.enableAttributeArray( a_fire_texture_coord );

    // ***** Bind textures
    glEnable(GL_TEXTURE_2D);
    GLint fire_noise = glGetUniformLocation(m_fire_shader.programId(), "u_texture_noise");
    GLint fire_flame = glGetUniformLocation(m_fire_shader.programId(), "u_texture_flame");
    glUseProgram(m_fire_shader.programId());
    glUniform1i(fire_noise, 0);
    glUniform1i(fire_flame, 1);

    // !!!!! #NOTE: Must be called in descending order and end on 0
    glActiveTexture(GL_TEXTURE1);                           // Texture unit 1
    DrEngineTexture *flame;
    switch (fire->fire_mask) {
        case Fire_Mask::Torch:      flame = m_engine->getTexture(Asset_Textures::Fire_Flame_Torch);     break;
        case Fire_Mask::Candle:     flame = m_engine->getTexture(Asset_Textures::Fire_Flame_Candle);    break;
        case Fire_Mask::Square:     flame = m_engine->getTexture(Asset_Textures::Fire_Flame_Square);    break;
        case Fire_Mask::Triangle:   flame = m_engine->getTexture(Asset_Textures::Fire_Flame_Triangle);  break;
    }
    glBindTexture(GL_TEXTURE_2D, flame->texture()->textureId());
    flame->texture()->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);

    glActiveTexture(GL_TEXTURE0);                           // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Fire_Noise)->texture()->textureId());
    m_engine->getTexture(Asset_Textures::Fire_Noise)->texture()->setWrapMode(QOpenGLTexture::WrapMode::MirroredRepeat);

    // ***** Load vertices for this object
    QVector<GLfloat> vertices;
    getThingVertices(vertices, fire, static_cast<float>(-fire->getAngle()));
    m_fire_shader.setAttributeArray( a_fire_vertex, vertices.data(), 3 );
    m_fire_shader.enableAttributeArray( a_fire_vertex );

    // ***** Set Shader Variables
    float now = static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 1000.f);
    m_fire_shader.setUniformValue( u_fire_alpha,    fire->getOpacity() );
    m_fire_shader.setUniformValue( u_fire_time,     now );
    m_fire_shader.setUniformValue( u_fire_position, static_cast<float>(thing->getPosition().x()), static_cast<float>(thing->getPosition().y()) );
    m_fire_shader.setUniformValue( u_fire_width,    static_cast<float>(fire->getSize().x()) * fire->getScaleX() );
    m_fire_shader.setUniformValue( u_fire_height,   static_cast<float>(fire->getSize().y()) * fire->getScaleY() );

    m_fire_shader.setUniformValue( u_fire_shape,    static_cast<int>(fire->fire_mask) );
    m_fire_shader.setUniformValue( u_fire_start_color,
                                        static_cast<float>(fire->start_color.redF()),
                                        static_cast<float>(fire->start_color.greenF()),
                                        static_cast<float>(fire->start_color.blueF()) );
    m_fire_shader.setUniformValue( u_fire_end_color,
                                        static_cast<float>(fire->end_color.redF()),
                                        static_cast<float>(fire->end_color.greenF()),
                                        static_cast<float>(fire->end_color.blueF()) );
    m_fire_shader.setUniformValue( u_fire_smoke_color,
                                        static_cast<float>(fire->smoke_color.redF()),
                                        static_cast<float>(fire->smoke_color.greenF()),
                                        static_cast<float>(fire->smoke_color.blueF()) );
    m_fire_shader.setUniformValue( u_fire_intensity,    fire->intensity );
    m_fire_shader.setUniformValue( u_fire_smoothness,   fire->smoothness );
    m_fire_shader.setUniformValue( u_fire_wavy,         fire->wavy );
    m_fire_shader.setUniformValue( u_fire_speed,       (fire->flame_speed / 2.f) );

    m_fire_shader.setUniformValue( u_fire_pixel_x,      fire->pixel_x );
    m_fire_shader.setUniformValue( u_fire_pixel_y,      fire->pixel_y );
    m_fire_shader.setUniformValue( u_fire_bitrate,      fire->bitrate );

    // ***** Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Release bound items
    m_fire_shader.disableAttributeArray( a_fire_vertex );
    m_fire_shader.disableAttributeArray( a_fire_texture_coord );
    m_fire_shader.release();

    last_thing = DrThingType::Fire;
    return true;
}















