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
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"



//####################################################################################
//##        Draws a DrEngineObject effect type with default shader
//####################################################################################
void DrOpenGL::drawObject(DrEngineThing *thing, DrThingType &last_thing) {
    if ( thing->getThingType() != DrThingType::Object) return;
    DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
    if (!object) return;

    // ***** Don't draw Segments (lines)
    bool skip_object = false;
    for (auto shape : object->shapes) {
        if (object->shape_type[shape] == Shape_Type::Segment)
            skip_object = true;
    }
    if (skip_object) return;

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
        m_default_shader.setAttributeArray( a_default_texture_coord, m_whole_texture_coordinates.data(), 2 );
        m_default_shader.enableAttributeArray( a_default_texture_coord );
    }

    // ***** Get texture to render with, set texture coordinates
    DrEngineTexture *texture = m_engine->getTexture(object->getTextureNumber());
    if (!texture->texture()->isBound()) {
        texture->texture()->bind();
    }
    float texture_width =  texture->width();
    float texture_height = texture->height();

    // ***** Get object position data
    QPointF center = object->getPosition();
    float   x, y, z;
    float   half_width, half_height;
    x = static_cast<float>(center.x());
    y = static_cast<float>(center.y());
    z = static_cast<float>(object->z_order);
    half_width =  texture_width *  object->getScaleX() / 2.0f;
    half_height = texture_height * object->getScaleY() / 2.0f;

    // ***** Create rotation matrix, apply rotation to object
    QMatrix4x4 matrix;
    matrix.rotate( static_cast<float>(object->getAngle()), 0.0, 0.0, 1.0 );
    QVector3D top_right = matrix * QVector3D( half_width,  half_height, 0);
    QVector3D top_left =  matrix * QVector3D(-half_width,  half_height, 0);
    QVector3D bot_right = matrix * QVector3D( half_width, -half_height, 0);
    QVector3D bot_left =  matrix * QVector3D(-half_width, -half_height, 0);

    // ***** Load vertices for this object
    QVector<GLfloat> vertices;
    vertices.clear();
    vertices.resize( 12 );                      // in sets of x, y, z
    vertices[ 0] = top_right.x() + x;       vertices[ 1] = top_right.y() + y;       vertices[ 2] = z;       // Top Right
    vertices[ 3] = top_left.x()  + x;       vertices[ 4] = top_left.y()  + y;       vertices[ 5] = z;       // Top Left
    vertices[ 6] = bot_right.x() + x;       vertices[ 7] = bot_right.y() + y;       vertices[ 8] = z;       // Bottom Right
    vertices[ 9] = bot_left.x()  + x;       vertices[10] = bot_left.y()  + y;       vertices[11] = z;       // Bottom Left
    m_default_shader.setAttributeArray( a_default_vertex, vertices.data(), 3 );
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

    // ***** Get object position data
    QPointF center = object->getPosition();

    float x, y, z, half_width, half_height;
    x = static_cast<float>(center.x());
    y = static_cast<float>(center.y());
    z = static_cast<float>(object->z_order);
    half_width =  (static_cast<float>(texture->width()) *  object->getScaleX() * 1.00f) / 2.0f;
    half_height = (static_cast<float>(texture->height()) * object->getScaleY() * 1.00f) / 2.0f;

    // ***** Create rotation matrix, apply rotation to object
    QMatrix4x4 matrix;
    matrix.rotate( static_cast<float>(object->getAngle()), 0.0, 0.0, 1.0 );
    QVector3D top_right = matrix * QVector3D( half_width,  half_height, 0);
    QVector3D top_left =  matrix * QVector3D(-half_width,  half_height, 0);
    QVector3D bot_right = matrix * QVector3D( half_width, -half_height, 0);
    QVector3D bot_left =  matrix * QVector3D(-half_width, -half_height, 0);

    // ***** Load vertices for this object
    QVector<GLfloat> vertices;
    vertices.clear();
    vertices.resize( 12 );                  // in sets of x, y, z
    vertices[ 0] = top_right.x() + x;       vertices[ 1] = top_right.y() + y;       vertices[ 2] = z;           // Top Right
    vertices[ 3] = top_left.x()  + x;       vertices[ 4] = top_left.y()  + y;       vertices[ 5] = z;           // Top Left
    vertices[ 6] = bot_right.x() + x;       vertices[ 7] = bot_right.y() + y;       vertices[ 8] = z;           // Bottom Right
    vertices[ 9] = bot_left.x()  + x;       vertices[10] = bot_left.y()  + y;       vertices[11] = z;           // Bottom Left
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
//####################################################################################
void DrOpenGL::drawObjectFire(DrEngineThing *thing, DrThingType &last_thing) {
    DrEngineFire *fire = dynamic_cast<DrEngineFire*>(thing);
    if (!fire) return;

    // ***** Enable shader program
    if (!m_fire_shader.bind()) return;

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
        case Fire_Mask::Torch:  flame = m_engine->getTexture(Asset_Textures::Fire_Flame_Torch);     break;
        case Fire_Mask::Candle: flame = m_engine->getTexture(Asset_Textures::Fire_Flame_Candle);    break;
    }
    glBindTexture(GL_TEXTURE_2D, flame->texture()->textureId());
    flame->texture()->setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);

    glActiveTexture(GL_TEXTURE0);                           // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Fire_Noise)->texture()->textureId());
    m_engine->getTexture(Asset_Textures::Fire_Noise)->texture()->setWrapMode(QOpenGLTexture::WrapMode::MirroredRepeat);


    // ***** Get object position data
    QPointF center = fire->getPosition();
    float   x, y, z;
    float   half_width, half_height;
    x = static_cast<float>(center.x());
    y = static_cast<float>(center.y());
    z = static_cast<float>(fire->z_order);
    half_width =  static_cast<float>(fire->getSize().x()) * fire->getScaleX() / 2.0f;
    half_height = static_cast<float>(fire->getSize().y()) * fire->getScaleY() / 2.0f;

    // ***** Create rotation matrix, apply rotation to object
    QMatrix4x4 matrix;
    matrix.rotate( static_cast<float>( -fire->getAngle()), 0.0, 0.0, 1.0 );
    QVector3D top_right = matrix * QVector3D( half_width,  half_height, 0);
    QVector3D top_left =  matrix * QVector3D(-half_width,  half_height, 0);
    QVector3D bot_right = matrix * QVector3D( half_width, -half_height, 0);
    QVector3D bot_left =  matrix * QVector3D(-half_width, -half_height, 0);

    // ***** Load vertices for this object
    QVector<GLfloat> vertices;
    vertices.clear();
    vertices.resize( 12 );                      // in sets of x, y, z
    vertices[ 0] = top_right.x() + x;       vertices[ 1] = top_right.y() + y;       vertices[ 2] = z;       // Top Right
    vertices[ 3] = top_left.x()  + x;       vertices[ 4] = top_left.y()  + y;       vertices[ 5] = z;       // Top Left
    vertices[ 6] = bot_right.x() + x;       vertices[ 7] = bot_right.y() + y;       vertices[ 8] = z;       // Bottom Right
    vertices[ 9] = bot_left.x()  + x;       vertices[10] = bot_left.y()  + y;       vertices[11] = z;       // Bottom Left
    m_fire_shader.setAttributeArray( a_fire_vertex, vertices.data(), 3 );
    m_fire_shader.enableAttributeArray( a_fire_vertex );

    // ***** Set Shader Variables
    float now = static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 1000.f);
    m_fire_shader.setUniformValue( u_fire_alpha,    fire->getOpacity() );
    m_fire_shader.setUniformValue( u_fire_time,     now );
    m_fire_shader.setUniformValue( u_fire_position, x, y );
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
}















