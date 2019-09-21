//
//      Created by Stephens Nunnally on 8/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "engine_mesh/engine_vertex_data.h"
#include "engine_things/engine_thing_object.h"
#include "engine_things/engine_thing_fire.h"
#include "helper.h"
#include "opengl/opengl.h"
#include "project/project.h"
#include "project/project_asset.h"


QMatrix4x4 billboardSphericalBegin(QVector3D camera, QVector3D object, QMatrix4x4 model_view_projection, bool cylindrical_only = false) {
    QMatrix4x4 mvp = model_view_projection;
    QVector3D  look_at, obj_to_cam_proj, up_aux, obj_to_cam;
    float      angle_cosine;

    // obj_to_cam_proj is the vector in world coordinates from the local origin to the camera projected in the XZ plane
    obj_to_cam_proj.setX( camera.x() - object.x() );
    obj_to_cam_proj.setY( 0 );
    obj_to_cam_proj.setZ( camera.z() - object.z() );

    // This is the original look_at vector for the object in world coordinates
    look_at = QVector3D(0, 0, 1);

    // Normalize both vectors to get the cosine directly afterwards
    obj_to_cam_proj.normalize();

    // Easy fix to determine wether the angle is -/+:
    //      For positive angles up_aux will be a vector pointing in the positive y direction,
    //      Otherwise up_aux will point downwards effectively reversing the rotation
    up_aux = QVector3D::crossProduct(look_at, obj_to_cam_proj);

    // Compute the angle
    angle_cosine = QVector3D::dotProduct(look_at, obj_to_cam_proj);

    // Perform the rotation
    if ((angle_cosine < 0.9999f) && (angle_cosine > -0.9999f)) {
        mvp.rotate( acos(angle_cosine)*180.f/3.14f, up_aux.x(), up_aux.y(), up_aux.z());
    }

    // So far it is just a cylindrical billboard, now the second part tilts the object so that it faces the camera
    // "obj_to_cam" is the vector in world coordinates from the local origin to the camera
    if (cylindrical_only) return mvp;
    obj_to_cam.setX( camera.x() - object.x() );
    obj_to_cam.setY( camera.y() - object.y() );
    obj_to_cam.setZ( camera.z() - object.z() );

    // Normalize to get the cosine afterwards
    obj_to_cam.normalize();

    // Compute the angle between obj_to_cam_proj and obj_to_cam, i.e. compute the required angle for the lookup vector
    angle_cosine = QVector3D::dotProduct(obj_to_cam_proj, obj_to_cam);

    // Tilt the object
    if ((angle_cosine < 0.9999f) && (angle_cosine > -0.9999f)) {
        if (obj_to_cam.y() < 0) mvp.rotate( acos(angle_cosine)*180.f/3.14f,  1, 0, 0 );
        else                    mvp.rotate( acos(angle_cosine)*180.f/3.14f, -1, 0, 0 );
    }

    return mvp;
}



//####################################################################################
//##    Draws a DrEngineObject effect type with Default Shader
//####################################################################################
void DrOpenGL::drawObject(DrEngineThing *thing, DrThingType &last_thing, bool draw2D) {

    // ***** Initial type checks
    if (thing->getThingType() != DrThingType::Object) return;
    DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
    if (!object) return;

    // Don't draw Segments (lines)
    ///bool skip_object = false;
    ///for (auto shape : object->shapes) {
    ///    if (object->shape_type[shape] == Shape_Type::Segment)
    ///        skip_object = true;
    ///}
    ///if (skip_object) return;
    if (object->shapes.count() > 0) {
        if (object->shape_type[object->shapes[0]] == Shape_Type::Segment) return;
    }

    // Enable shader program
    if (last_thing != DrThingType::Object)
        if (!m_default_shader.bind()) return;


    // ***** Blend function
    glEnable(GL_BLEND);
    ///glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);           // Standard non-premultiplied alpha blend
    ///glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);                     // Additive blending
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);                    // Premultiplied alpha blend

    // Fancy Seperate RGB/Alpha Blend Functions
    ///glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);        // Premultiplied alpha blend


    // ***** Set Matrix for Shader, calculates current matrix, adds in object location
    float x =   static_cast<float>(thing->getPosition().x());
    float y =   static_cast<float>(thing->getPosition().y());
    float z =   static_cast<float>(thing->z_order + m_add_z);
    double now = Dr::MillisecondsSinceStartOfDay() / 10.0;

    // Translate
    QMatrix4x4 model;
    model.translate(x, y, z);

    // Rotate
    if (!object->getBillboard()) {
        if (qFuzzyCompare(object->getAngleX(), 0.0) == false || qFuzzyCompare(object->getRotateSpeedX(), 0.0) == false)
            model.rotate(static_cast<float>(object->getAngleX() + (now * object->getRotateSpeedX())), 1.f, 0.f, 0.f);
        if (qFuzzyCompare(object->getAngleY(), 0.0) == false || qFuzzyCompare(object->getRotateSpeedY(), 0.0) == false)
            model.rotate(static_cast<float>(object->getAngleY() + (now * object->getRotateSpeedY())), 0.f, 1.f, 0.f);
        model.rotate(static_cast<float>(object->getAngle()), 0.f, 0.f, 1.f);

    // Rotate Billboards
    } else {
        ///QMatrix4x4 pmv = (m_projection * m_view * model);
        ///// Get length of first row as scale factor
        ///float       scale_factor = pmv.row(0).toVector3D().length();
        ///// Get upper left 3x3 sub-matrix and unscale
        ///QMatrix3x3  parent_rotation = pmv.toGenericMatrix<3,3>();
        ///            parent_rotation *= 1.0f / scale_factor;
        ///QQuaternion inverse_parent_rotation = QQuaternion::fromRotationMatrix(parent_rotation).inverted();
        ///// Set as rotation
        ///model.rotate(inverse_parent_rotation);
        model = billboardSphericalBegin( QVector3D(m_eye.x(), m_eye.y(), m_eye.z()),
                                         QVector3D(x * m_scale, y * m_scale, z), model, false);
    }

    // Scale
    if (draw2D || object->get3DType() == Convert_3D_Type::Cube)
        model.scale(static_cast<float>(object->getSize().x()), static_cast<float>(object->getSize().y()), 1.0f);
    model.scale( object->getScaleX(), object->getScaleY(), static_cast<float>(object->getDepth()) );

    m_default_shader.setUniformValue( u_default_matrix,         m_projection * m_view * model );
    m_default_shader.setUniformValue( u_default_matrix_object,  model );

    // Remove scaling from camera position for shading calculations
    QMatrix4x4 matrix_eye;
    matrix_eye.scale(1.f/m_scale);
    QVector3D eye_move = matrix_eye * m_eye;


    // ***** Get texture to render with, set texture coordinates
    DrEngineTexture *texture = m_engine->getTexture(object->getTextureNumber());
    if (!texture->texture()->isBound()) {
        texture->texture()->bind();
    }
    float texture_width =  texture->width();
    float texture_height = texture->height();


    // ***** Set Shader Variables
    m_default_shader.setUniformValue( u_default_texture, 0 );                           // Use texture unit 0
    m_default_shader.setUniformValue( u_default_average_color,
                                        static_cast<float>(texture->averageColor().redF()),
                                        static_cast<float>(texture->averageColor().greenF()),
                                        static_cast<float>(texture->averageColor().blueF()) );
    m_default_shader.setUniformValue( u_default_tint,           0.0f, 0.0f, 0.0f );
    m_default_shader.setUniformValue( u_default_width,          texture_width );
    m_default_shader.setUniformValue( u_default_height,         texture_height );
    m_default_shader.setUniformValue( u_default_time,           static_cast<float>(Dr::MillisecondsSinceStartOfDay() / 1000.0) );
    m_default_shader.setUniformValue( u_default_pre,            true );

    m_default_shader.setUniformValue( u_default_bitrate,        object->bitrate );
    m_default_shader.setUniformValue( u_default_pixel_x,        object->pixel_x );
    m_default_shader.setUniformValue( u_default_pixel_y,        object->pixel_y );
    m_default_shader.setUniformValue( u_default_pixel_offset,   0.5f, 0.5f );
    m_default_shader.setUniformValue( u_default_negative,       object->negative );
    m_default_shader.setUniformValue( u_default_grayscale,      object->grayscale );
    m_default_shader.setUniformValue( u_default_hue,            object->hue );
    m_default_shader.setUniformValue( u_default_saturation,     object->saturation );
    m_default_shader.setUniformValue( u_default_contrast,       object->contrast );
    m_default_shader.setUniformValue( u_default_brightness,     object->brightness );

    m_default_shader.setUniformValue( u_default_shade_away,     !draw2D );
    m_default_shader.setUniformValue( u_default_camera_pos,     eye_move.x(), eye_move.y(), eye_move.z() );
    m_default_shader.setUniformValue( u_default_cartoon,        false );
    m_default_shader.setUniformValue( u_default_wavy,           false );
    m_default_shader.setUniformValue( u_default_wireframe,      (m_engine->getCurrentWorld()->wireframe || object->wireframe) );

    // Fade away dying object
    float alpha = object->getOpacity();                                                 // Start with object alpha
    if (!object->isAlive() && object->getFadeOnDeath()) {
        double fade_percent = 1.0 - (static_cast<double>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<double>(object->getFadeDelay()));
        alpha *= static_cast<float>(fade_percent);
    }
    m_default_shader.setUniformValue( u_default_alpha,          alpha );


    // ***** Draw triangles using shader program
    if (draw2D) {
        // Simple Quad Render
        m_default_shader.enableAttributeArray( a_default_vertex );
        m_default_shader.enableAttributeArray( a_default_texture_coord );
        m_default_shader.enableAttributeArray( a_default_barycentric );
        m_default_shader.setAttributeArray(    a_default_vertex,        m_quad_vertices.data(),             3 );
        m_default_shader.setAttributeArray(    a_default_texture_coord, m_quad_texture_coordinates.data(),  2 );
        m_default_shader.setAttributeArray(    a_default_barycentric,   m_quad_barycentric.data(),          3 );
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
        m_default_shader.disableAttributeArray( a_default_vertex );
        m_default_shader.disableAttributeArray( a_default_texture_coord );
        m_default_shader.disableAttributeArray( a_default_barycentric );
        // VBO Render
        ///setDefaultAttributeBuffer(m_quad_vbo);
        ///int quad_vertices =  m_quad_vbo->size() / (c_vertex_length * c_float_size);
        ///glDrawArrays(GL_TRIANGLES, 0, quad_vertices );
        ///releaseDefaultAttributeBuffer();
        ///m_quad_vbo->release();
        addTriangles( 2 );

    } else if (object->get3DType() == Convert_3D_Type::Cube) {
        setDefaultAttributeBuffer(m_cube_vbo);
        int cube_vertices =  m_cube_vbo->size() / (c_vertex_length * c_float_size);
        glDrawArrays(GL_TRIANGLES, 0, cube_vertices );
        releaseDefaultAttributeBuffer();
        m_cube_vbo->release();
        addTriangles( 12 );      // aka 'cube_vertices / 3'

    } else {
        setDefaultAttributeBuffer(m_texture_vbos[object->getTextureNumber()]);
        glDrawArrays(GL_TRIANGLES, 0, m_texture_data[object->getTextureNumber()]->vertexCount() );
        releaseDefaultAttributeBuffer();
        m_texture_vbos[object->getTextureNumber()]->release();
        addTriangles( m_texture_data[object->getTextureNumber()]->triangleCount() );
    }

    ///m_default_shader.release();
    last_thing = DrThingType::Object;
}

// Bind vertex array
void DrOpenGL::setDefaultAttributeBuffer(QOpenGLBuffer *buffer) {
    // As QOpenGlWidget calls
    ///buffer->bind();
    ///m_default_shader.enableAttributeArray(  PROGRAM_VERTEX_ATTRIBUTE);
    ///m_default_shader.enableAttributeArray(  PROGRAM_NORMAL_ATTRIBUTE);
    ///m_default_shader.enableAttributeArray(  PROGRAM_TEXCOORD_ATTRIBUTE);
    ///m_default_shader.enableAttributeArray(  PROGRAM_BARYCENTRIC_ATTRIBUTE);
    ///m_default_shader.setAttributeBuffer(    PROGRAM_VERTEX_ATTRIBUTE,      GL_FLOAT, 0               , 3, c_vertex_length * c_float_size);
    ///m_default_shader.setAttributeBuffer(    PROGRAM_NORMAL_ATTRIBUTE,      GL_FLOAT, 3 * c_float_size, 3, c_vertex_length * c_float_size);
    ///m_default_shader.setAttributeBuffer(    PROGRAM_TEXCOORD_ATTRIBUTE,    GL_FLOAT, 6 * c_float_size, 2, c_vertex_length * c_float_size);
    ///m_default_shader.setAttributeBuffer(    PROGRAM_BARYCENTRIC_ATTRIBUTE, GL_FLOAT, 8 * c_float_size, 3, c_vertex_length * c_float_size);

    // Standard OpenGL Calls
    glBindBuffer(GL_ARRAY_BUFFER, buffer->bufferId());
    glEnableVertexAttribArray( PROGRAM_VERTEX_ATTRIBUTE );
    glEnableVertexAttribArray( PROGRAM_NORMAL_ATTRIBUTE );
    glEnableVertexAttribArray( PROGRAM_TEXCOORD_ATTRIBUTE );
    glEnableVertexAttribArray( PROGRAM_BARYCENTRIC_ATTRIBUTE );
    glVertexAttribPointer( PROGRAM_VERTEX_ATTRIBUTE,      3, GL_FLOAT, GL_FALSE, c_vertex_length * c_float_size, nullptr);
    glVertexAttribPointer( PROGRAM_NORMAL_ATTRIBUTE,      3, GL_FLOAT, GL_FALSE, c_vertex_length * c_float_size, reinterpret_cast<void *>(3 * c_float_size));
    glVertexAttribPointer( PROGRAM_TEXCOORD_ATTRIBUTE,    2, GL_FLOAT, GL_FALSE, c_vertex_length * c_float_size, reinterpret_cast<void *>(6 * c_float_size));
    glVertexAttribPointer( PROGRAM_BARYCENTRIC_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, c_vertex_length * c_float_size, reinterpret_cast<void *>(8 * c_float_size));
}

// Release bound items
void DrOpenGL::releaseDefaultAttributeBuffer() {
    m_default_shader.disableAttributeArray( PROGRAM_VERTEX_ATTRIBUTE );
    m_default_shader.disableAttributeArray( PROGRAM_NORMAL_ATTRIBUTE );
    m_default_shader.disableAttributeArray( PROGRAM_TEXCOORD_ATTRIBUTE );
    m_default_shader.disableAttributeArray( PROGRAM_BARYCENTRIC_ATTRIBUTE );
}


//####################################################################################
//##    Draws a DrEngineObject effect type with Simple Shader
//####################################################################################
void DrOpenGL::drawObjectSimple(DrEngineThing *thing) {

    // ***** Initial type checks
    if (thing->getThingType() != DrThingType::Object) return;
    DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
    if (!object) return;

    // Don't draw Segments (lines)
    if (object->shapes.count() > 0) {
        if (object->shape_type[object->shapes[0]] == Shape_Type::Segment) return;
    }

    // Enable shader program
    if (!m_simple_shader.bind()) return;

    // ***** Blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);                    // Premultiplied alpha blend

    // ***** Set Matrix for Shader, calculates current matrix, adds in object location
    float x =   static_cast<float>(thing->getPosition().x());
    float y =   static_cast<float>(thing->getPosition().y());
    float z =   static_cast<float>(thing->z_order + m_add_z);
    double now = Dr::MillisecondsSinceStartOfDay() / 10.0;

    // Translate
    QMatrix4x4 model;
    model.translate(x, y, z);

    // Rotate
    if (qFuzzyCompare(object->getAngleX(), 0.0) == false || qFuzzyCompare(object->getRotateSpeedX(), 0.0) == false)
        model.rotate(static_cast<float>(object->getAngleX() + (now * object->getRotateSpeedX())), 1.f, 0.f, 0.f);
    if (qFuzzyCompare(object->getAngleY(), 0.0) == false || qFuzzyCompare(object->getRotateSpeedY(), 0.0) == false)
        model.rotate(static_cast<float>(object->getAngleY() + (now * object->getRotateSpeedY())), 0.f, 1.f, 0.f);
    model.rotate(static_cast<float>(object->getAngle()), 0.f, 0.f, 1.f);

    // Scale
    model.scale(static_cast<float>(object->getSize().x()), static_cast<float>(object->getSize().y()), 1.0f);
    model.scale( object->getScaleX(), object->getScaleY(), static_cast<float>(object->getDepth()) );

    m_simple_shader.setUniformValue( u_simple_matrix,         m_projection * m_view * model );

    // ***** Get texture to render with, set texture coordinates
    DrEngineTexture *texture = m_engine->getTexture(object->getTextureNumber());
    if (!texture->texture()->isBound()) texture->texture()->bind();

    // ***** Set Shader Variables
    m_simple_shader.setUniformValue( u_simple_texture, 0 );                           // Use texture unit 0

    // Fade away dying object
    float alpha = object->getOpacity();                                                 // Start with object alpha
    if (!object->isAlive() && object->getFadeOnDeath()) {
        double fade_percent = 1.0 - (static_cast<double>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<double>(object->getFadeDelay()));
        alpha *= static_cast<float>(fade_percent);
    }
    m_simple_shader.setUniformValue( u_simple_alpha,          alpha );

    // ***** Draw triangles using shader program
    m_simple_shader.enableAttributeArray( a_simple_vertex );
    m_simple_shader.enableAttributeArray( a_simple_texture_coord );
    m_simple_shader.setAttributeArray(    a_simple_vertex,        m_quad_vertices.data(),             3 );
    m_simple_shader.setAttributeArray(    a_simple_texture_coord, m_quad_texture_coordinates.data(),  2 );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    m_simple_shader.disableAttributeArray( a_simple_vertex );
    m_simple_shader.disableAttributeArray( a_simple_texture_coord );
    addTriangles( 2 );

    m_simple_shader.release();
}


//####################################################################################
//##    Renders the light to the using the Shadow Map
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
        m_occluder_shader.setAttributeArray(    a_occluder_texture_coord, m_quad_texture_coordinates.data(), 2 );
        m_occluder_shader.enableAttributeArray( a_occluder_texture_coord );
    }

    // ***** Get texture to render with, set texture coordinates
    DrEngineTexture *texture = m_engine->getTexture(object->getTextureNumber());
    if (!texture->texture()->isBound())
        texture->texture()->bind();  

    // ***** Load vertices for this object
    QVector<GLfloat> vertices;
    getThingVertices(vertices, object);
    m_occluder_shader.setAttributeArray(    a_occluder_vertex, vertices.data(), 3 );
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

    addTriangles( 2 );
    return true;            // Returns true when m_occluder_shader has been initialized
}



//####################################################################################
//##    Uses Fire Shader to draw Fire
//##        - Returns true if rendered, false if not
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
    m_fire_shader.setUniformValue( u_fire_matrix, (m_projection * m_view) );

    // ***** Set Texture Coordinates for Shader
    m_fire_shader.setAttributeArray(    a_fire_texture_coord, m_quad_texture_coordinates.data(), 2 );
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
    getThingVertices(vertices, fire);
    m_fire_shader.setAttributeArray(    a_fire_vertex, vertices.data(), 3 );
    m_fire_shader.enableAttributeArray( a_fire_vertex );

    // ***** Set Shader Variables
    float now = static_cast<float>(Dr::MillisecondsSinceStartOfDay() / 1000.0);
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

    addTriangles( 2 );
    last_thing = DrThingType::Fire;
    return true;
}















