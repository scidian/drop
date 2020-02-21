//
//      Created by Stephens Nunnally on 8/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "engine/mesh/engine_vertex_data.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/thing/engine_thing_fire.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"


//####################################################################################
//##    Keeps Object Looking at Camera
//####################################################################################
QMatrix4x4 billboard(glm::vec3 pos, glm::vec3 camera_pos, glm::vec3 camera_up) {
    glm::vec3 look =    glm::vec3(camera_pos - pos);
              look =    glm::normalize(look);
    glm::vec3 right =   glm::cross(camera_up,  look);
    glm::vec3 up2 =     glm::cross(look,       right);
    QMatrix4x4 transform = QMatrix4x4(  right.x,  right.y,  right.z,  0,
                                        up2.x,    up2.y,    up2.z,    0,
                                        look.x,   look.y,   look.z,   0,
                                        pos.x,    pos.y,    pos.z,    1);
    return transform;
}

QMatrix4x4 billboardSphericalBegin(glm::vec3 camera, glm::vec3 object, glm::vec3 up, glm::vec3 look_at,
                                   QMatrix4x4 model_view_projection, bool cylindrical_only = false) {
    QMatrix4x4 mvp = model_view_projection;
    glm::vec3  obj_to_cam_proj, up_aux, obj_to_cam;
    float      angle_cosine;

    Up_Vector uv = (glm::distance(c_up_vector_y, up) < 0.5f) ? Up_Vector::Y : Up_Vector::Z;

    // obj_to_cam_proj is the vector in world coordinates from the local origin to the camera projected in the XZ plane    
    obj_to_cam_proj.x = camera.x - object.x;
    obj_to_cam_proj.y = camera.y - object.y;
    obj_to_cam_proj.z = camera.z - object.z;

    if (uv == Up_Vector::Y) {
        obj_to_cam_proj.y = 0;
        look_at = glm::vec3(0, 0, 1);           // This is the original look_at vector for the object in world coordinates
    } else {
        obj_to_cam_proj.z = 0;
        look_at = glm::vec3(0, 0, 1);           // This is the original look_at vector for the object in world coordinates
    }

    // Normalize both vectors to get the cosine directly afterwards
    obj_to_cam_proj = glm::normalize(obj_to_cam_proj);

    // Easy fix to determine wether the angle is -/+:
    //      For positive angles up_aux will be a vector pointing in the positive y direction,
    //      Otherwise up_aux will point downwards effectively reversing the rotation
    up_aux =        glm::cross(look_at, obj_to_cam_proj);

    // Compute the angle
    angle_cosine =  glm::dot(look_at, obj_to_cam_proj);

    // Perform the rotation
    if ((angle_cosine < 0.9999f) && (angle_cosine > -0.9999f)) {
        mvp.rotate( std::acos(angle_cosine)*180.f/3.14f, up_aux.x, up_aux.y, up_aux.z);
    }

    // So far it is just a cylindrical billboard, now the second part tilts the object so that it faces the camera
    // "obj_to_cam" is the vector in world coordinates from the local origin to the camera
    if (cylindrical_only) return mvp;
    obj_to_cam.x = camera.x - object.x;
    obj_to_cam.y = camera.y - object.y;
    obj_to_cam.z = camera.z - object.z;

    // Normalize to get the cosine afterwards
    obj_to_cam = glm::normalize(obj_to_cam);

    // Compute the angle between obj_to_cam_proj and obj_to_cam, i.e. compute the required angle for the lookup vector
    angle_cosine = glm::dot(obj_to_cam_proj, obj_to_cam);

    // Tilt the object
    if ((angle_cosine < 0.9999f) && (angle_cosine > -0.9999f)) {
        if (obj_to_cam.y < 0) mvp.rotate( acos(angle_cosine)*180.f/3.14f,  1, 0, 0 );
        else                  mvp.rotate( acos(angle_cosine)*180.f/3.14f, -1, 0, 0 );
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
    if (object == nullptr) return;

    // ***** Load possible 3D Info
    ThingComp3D *comp_3d = object->comp3D();
    Convert_3D_Type comp_3d_type = Convert_3D_Type::None;           // Type of 3D extrusion
    double comp_3d_angle_x { 0.0 };                                 // X axis rotation
    double comp_3d_angle_y { 0.0 };                                 // Y axis rotation
    double comp_3d_rotate_x_speed { 0.0 };                          // X axis rotation speed
    double comp_3d_rotate_y_speed { 0.0 };                          // Y axis rotation speed
    bool   comp_3d_billboard { false };                             // Make Object face camera?
    double comp_3d_depth { 0.0 };                                   // Desired 3D Depth of 2D Objects
    if (comp_3d != nullptr) {
        comp_3d_type =           object->comp3D()->get3DType();
        comp_3d_angle_x =        object->comp3D()->getAngleX();
        comp_3d_angle_y =        object->comp3D()->getAngleY();
        comp_3d_rotate_x_speed = object->comp3D()->getRotateSpeedX();
        comp_3d_rotate_y_speed = object->comp3D()->getRotateSpeedY();
        comp_3d_billboard =      object->comp3D()->getBillboard();
        comp_3d_depth =          object->comp3D()->getDepth();
    } else {
        draw2D = true;
    }

    // ***** Don't draw Segments (lines)
    ///bool skip_object = false;
    ///for (auto shape : object->shapes) {
    ///    if (object->shape_type[shape] == Shape_Type::Segment)
    ///        skip_object = true;
    ///}
    ///if (skip_object) return;
    if (object->shapes.size() > 0) {
        if (object->shape_type[object->shapes[0]] == Shape_Type::Segment) return;
    }

    // ***** Enable shader program
    if (last_thing != DrThingType::Object) {
        if (!m_default_shader.bind()) return;
    }


    // ***** Get texture to render with, set texture coordinates
    DrEngineTexture *texture;
    long texture_number = object->getTextureNumber();
    if (object->animation_idle_keys.size() > 0) {
        texture_number = object->animation_idle_keys[object->animation_idle_frame - 1];
    }
    texture = m_engine->getTexture(texture_number);
    if (texture == nullptr) return;


    // ***** Blend function
    glEnable(GL_BLEND);
    switch (object->blend_type) {
        case Blend_Object::Standard:
            ///glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                   // Standard non-premultiplied alpha blend
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);                            // Premultiplied alpha blend
            break;
        case Blend_Object::Additive:
            glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
            break;
        case Blend_Object::Subtractive:
            glBlendFuncSeparate(GL_SRC_COLOR, GL_DST_COLOR, GL_ZERO, GL_ONE);       // Subtract color only, not alpha
            glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);                              // Reverse Subtract is Dest - Source = new Color
            break;
    }
    // More Specific Method of Specifying Functions (Seperate RGB/Alpha Blend Functions):
    ///glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);        // Premultiplied alpha blend


    // ***** Pixelation
    if (object->pixel_texture != Pixel_Texture::None) {
        glEnable(GL_TEXTURE_2D);
        GLint texture_object = glGetUniformLocation(m_default_shader.programId(), "u_texture");
        GLint texture_pixel =  glGetUniformLocation(m_default_shader.programId(), "u_texture_pixel");
        glUseProgram(m_default_shader.programId());
        glUniform1i(texture_object,    0);
        glUniform1i(texture_pixel,     1);
        // Bind textures - !!!!! #NOTE: Must be called in descending order and end on 0
        glActiveTexture(GL_TEXTURE1);                           // Texture unit 1
        long texture_number = 0;
        if      (object->pixel_texture == Pixel_Texture::Ascii_Text)    texture_number = Asset_Textures::Pixel_Ascii_1;
        else if (object->pixel_texture == Pixel_Texture::Brick_Wall)    texture_number = Asset_Textures::Pixel_Brick_1;
        else if (object->pixel_texture == Pixel_Texture::Ceramic_Tile)  texture_number = Asset_Textures::Pixel_Tile_1;
        else if (object->pixel_texture == Pixel_Texture::Cross_Stitch)  texture_number = Asset_Textures::Pixel_Cross_1;
        else if (object->pixel_texture == Pixel_Texture::Knit_Stitch)   texture_number = Asset_Textures::Pixel_Sitch_1;
        else if (object->pixel_texture == Pixel_Texture::Woven_Cloth)   texture_number = Asset_Textures::Pixel_Woven_1;
        else if (object->pixel_texture == Pixel_Texture::Wood_Blocks)   texture_number = Asset_Textures::Pixel_Wood_1;
        if (texture_number != 0) {
            glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(texture_number)->texture()->textureId());
            m_engine->getTexture(texture_number)->texture()->setWrapMode(QOpenGLTexture::WrapMode::Repeat);
        }
        glActiveTexture(GL_TEXTURE0);                           // Texture unit 0
        glBindTexture(GL_TEXTURE_2D, texture->texture()->textureId());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER  // GL_MIRRORED_REPEAT
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER  // GL_MIRRORED_REPEAT
    } else if (!texture->texture()->isBound()) {
        texture->texture()->bind();
    }
    float texture_width =  texture->width();
    float texture_height = texture->height();


    // ***** Set Matrix for Shader, calculates current matrix, adds in object location
    float x =   static_cast<float>(thing->getPosition().x + m_add_z);
    float y =   static_cast<float>(thing->getPosition().y + m_add_z);
    float z =   static_cast<float>(thing->getZOrder()     + m_add_z);
    double now = Dr::MillisecondsSinceStartOfDay() / 10.0;

    // Translate
    QMatrix4x4 model;
    model.translate(x, y, z);

    // Rotate
    model.rotate(static_cast<float>(object->getAngle()), 0.f, 0.f, 1.f);
    model.rotate(static_cast<float>(comp_3d_angle_x + (now * comp_3d_rotate_x_speed)), 1.f, 0.f, 0.f);
    model.rotate(static_cast<float>(comp_3d_angle_y + (now * comp_3d_rotate_y_speed)), 0.f, 1.f, 0.f);

    // Rotate Billboards
    if (comp_3d_billboard) {
        // Kinda works
        ///model = billboardSphericalBegin( m_eye, QVector3D(x * combinedZoomScale(), y * combinedZoomScale(), z), m_up, m_look_at, model, false);

        // Works for look at
        // #NO_QT:  As original Qt calls
        ///QVector3D obj = QVector3D(x, y, z);
        ///QVector3D eye = m_eye / combinedZoomScale();
        ///model.setToIdentity();
        ///model.lookAt(obj, eye, m_up);
        ///model = model.inverted();
        //          No Qt Calls
        glm::vec3 obj{x, y, z};
        glm::vec3 eye = glm::vec3(m_eye.x(), m_eye.y(), m_eye.z()) / combinedZoomScale();
        glm::mat4 billboard = glm::lookAt(obj, eye, glm::vec3(m_up.x(), m_up.y(), m_up.z()));
        billboard = glm::inverse(billboard);
        model = QMatrix4x4(glm::value_ptr(billboard)).transposed();
    }

    // Scale
    bool  scale_2d = (draw2D || comp_3d_type == Convert_3D_Type::Cube || comp_3d_type == Convert_3D_Type::Cone);
    float add_pixel_x = 0.0;
    float add_pixel_y = 0.0;
    if (scale_2d) {
        model.scale(static_cast<float>(texture_width), static_cast<float>(texture_height), 1.0f);
    } else {
        // Add an extra pixel on 3D objects to reduce blockiness of world
        float pixels_to_add = 1.5f;
        add_pixel_x = (pixels_to_add * object->getScaleX()) / (static_cast<float>(texture_width));
        add_pixel_y = (pixels_to_add * object->getScaleY()) / (static_cast<float>(texture_height));
    }
    float flip_x = (object->isFlippedX()) ? -1.0 : 1.0;
    float flip_y = (object->isFlippedY()) ? -1.0 : 1.0;
    float final_x_scale = (object->getScaleX() + add_pixel_x) * flip_x;
    float final_y_scale = (object->getScaleY() + add_pixel_y) * flip_y;
    model.scale( final_x_scale, final_y_scale, static_cast<float>(comp_3d_depth) );


    // ***** Fade Away / Shrink Dying Object (Death Animation
    float alpha = object->getOpacity();                                                     // Start with object alpha
    if (!object->isAlive() && object->getDeathAnimation() != Death_Animation::None) {
        float fade_percent = 1.0f - (static_cast<float>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<float>(object->getDeathDuration()));
        if (object->getDeathAnimation() == Death_Animation::Fade) {
            alpha *= static_cast<float>(fade_percent);
        } else if (object->getDeathAnimation() == Death_Animation::Shrink) {
            model.scale( fade_percent, fade_percent );
        }
    }


    // ***** Some 3D Use Cases
    if (!draw2D) {
        // Transparent 3D Objects Don't Affect Depth Buffer
        if (alpha < 1.0f) glDepthMask(GL_FALSE);

        // Reverse Culling for Flipped Objects
        if ((final_x_scale < 0 && final_y_scale > 0) || (final_x_scale > 0 && final_y_scale < 0)) cullingOn(true);
    }


    // ***** Set Model Shader Values
    QMatrix4x4 mvp = m_projection * m_view * model;
    m_default_shader.setUniformValue( u_default_matrix,                 mvp );
    m_default_shader.setUniformValue( u_default_matrix_object,          model );

    // ***** Remove scaling from camera position for shading calculations
    QMatrix4x4 matrix_eye;
    matrix_eye.scale(1.f / combinedZoomScale());
    QVector3D eye_move = matrix_eye * m_eye;


    // ***** Set Shader Variables
    m_default_shader.setUniformValue( u_default_texture,                0 );
    m_default_shader.setUniformValue( u_default_texture_pixel,          1 );
    m_default_shader.setUniformValue( u_default_alpha,                  alpha );
    m_default_shader.setUniformValue( u_default_average_color,
                                        static_cast<float>(texture->averageColor().redF()),
                                        static_cast<float>(texture->averageColor().greenF()),
                                        static_cast<float>(texture->averageColor().blueF()) );
    m_default_shader.setUniformValue( u_default_tint,                   0.0f, 0.0f, 0.0f );
    m_default_shader.setUniformValue( u_default_zoom,                   combinedZoomScale() );
    m_default_shader.setUniformValue( u_default_width,                  texture_width );
    m_default_shader.setUniformValue( u_default_height,                 texture_height );
    m_default_shader.setUniformValue( u_default_time,                   static_cast<float>(Dr::MillisecondsSinceStartOfDay() / 1000.0) );
    m_default_shader.setUniformValue( u_default_pre,                    true );

    m_default_shader.setUniformValue( u_default_bitrate,                object->bitrate );
    m_default_shader.setUniformValue( u_default_pixel_x,                object->pixel_x );
    m_default_shader.setUniformValue( u_default_pixel_y,                object->pixel_y );
    m_default_shader.setUniformValue( u_default_pixel_offset,           0.0f, 0.0f );
    m_default_shader.setUniformValue( u_default_pixel_type,             static_cast<float>(object->pixel_texture) );
    m_default_shader.setUniformValue( u_default_negative,               object->negative );
    m_default_shader.setUniformValue( u_default_grayscale,              object->grayscale );
    m_default_shader.setUniformValue( u_default_hue,                    object->hue );
    m_default_shader.setUniformValue( u_default_saturation,             object->saturation );
    m_default_shader.setUniformValue( u_default_contrast,               object->contrast );
    m_default_shader.setUniformValue( u_default_brightness,             object->brightness );

    m_default_shader.setUniformValue( u_default_shade_away,             !draw2D );
    m_default_shader.setUniformValue( u_default_camera_pos,             eye_move.x(), eye_move.y(), eye_move.z() );

    m_default_shader.setUniformValue( u_default_cartoon,                object->cartoon );
    m_default_shader.setUniformValue( u_default_cartoon_width,          object->cartoon_width );
    m_default_shader.setUniformValue( u_default_cross_hatch,            object->cross_hatch );
    m_default_shader.setUniformValue( u_default_cross_hatch_width,      object->cross_hatch_width );
    m_default_shader.setUniformValue( u_default_wavy,                   false );
    if (m_engine->getCurrentWorld()->wireframe || object->wireframe) {
        m_default_shader.setUniformValue( u_default_wireframe,          true );
        if (object->wireframe)  m_default_shader.setUniformValue( u_default_wireframe_width,    object->wireframe_width );
        else                    m_default_shader.setUniformValue( u_default_wireframe_width,    m_engine->getCurrentWorld()->wireframe_width );
    } else {
        m_default_shader.setUniformValue( u_default_wireframe,          false );
        m_default_shader.setUniformValue( u_default_wireframe_width,    1.0f );
    }


    // ***** Draw triangles using shader program
    // Soft Body Render
    if ((object->body_style != Body_Style::Rigid_Body) && (object->compSoftBody() != nullptr)) {
        m_default_shader.enableAttributeArray( a_default_vertex );
        m_default_shader.enableAttributeArray( a_default_texture_coord );
        m_default_shader.enableAttributeArray( a_default_barycentric );
        m_default_shader.setAttributeArray(    a_default_vertex,        object->compSoftBody()->m_soft_vertices.data(),             3 );
        m_default_shader.setAttributeArray(    a_default_texture_coord, object->compSoftBody()->m_soft_texture_coordinates.data(),  2 );
        m_default_shader.setAttributeArray(    a_default_barycentric,   object->compSoftBody()->m_soft_barycentric.data(),          3 );
        glDrawArrays( GL_TRIANGLES, 0, object->compSoftBody()->m_soft_triangles*3 );
        m_default_shader.disableAttributeArray( a_default_vertex );
        m_default_shader.disableAttributeArray( a_default_texture_coord );
        m_default_shader.disableAttributeArray( a_default_barycentric );
        addTriangles( object->compSoftBody()->m_soft_triangles );

    // Simple Quad Render
    } else if (draw2D) {
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

    // 3D Cube Render
    } else if (comp_3d_type == Convert_3D_Type::Cube) {
        setDefaultAttributeBuffer(m_cube_vbo);
        int cube_vertices =  m_cube_vbo->size() / (c_vertex_length * c_float_size);
        glDrawArrays(GL_TRIANGLES, 0, cube_vertices );
        releaseDefaultAttributeBuffer();
        m_cube_vbo->release();
        addTriangles( 12 );      // aka 'cube_vertices / 3'

    // 3D Cone Render
    } else if (comp_3d_type == Convert_3D_Type::Cone) {
        setDefaultAttributeBuffer(m_cone_vbo);
        int cone_vertices =  m_cone_vbo->size() / (c_vertex_length * c_float_size);
        glDrawArrays(GL_TRIANGLES, 0, cone_vertices );
        releaseDefaultAttributeBuffer();
        m_cone_vbo->release();
        addTriangles( 6 );      // aka 'cone_vertices / 3'

    // 3D Extrude Render
    } else {
        setDefaultAttributeBuffer(m_texture_vbos[texture_number]);
        glDrawArrays(GL_TRIANGLES, 0, m_texture_data[texture_number]->vertexCount() );
        releaseDefaultAttributeBuffer();
        m_texture_vbos[texture_number]->release();
        addTriangles( m_texture_data[texture_number]->triangleCount() );
    }

    // Reset BlendEquation
    glBlendEquation(GL_FUNC_ADD);

    // Set Last Thing Type
    ///m_default_shader.release();
    last_thing = DrThingType::Object;
}

// Bind vertex array
void DrOpenGL::setDefaultAttributeBuffer(QOpenGLBuffer *buffer) {
    // #NO_QT:  As original QOpenGlWidget calls
    ///buffer->bind();
    ///m_default_shader.enableAttributeArray(  PROGRAM_VERTEX_ATTRIBUTE);
    ///m_default_shader.enableAttributeArray(  PROGRAM_NORMAL_ATTRIBUTE);
    ///m_default_shader.enableAttributeArray(  PROGRAM_TEXCOORD_ATTRIBUTE);
    ///m_default_shader.enableAttributeArray(  PROGRAM_BARYCENTRIC_ATTRIBUTE);
    ///m_default_shader.setAttributeBuffer(    PROGRAM_VERTEX_ATTRIBUTE,      GL_FLOAT, 0               , 3, c_vertex_length * c_float_size);
    ///m_default_shader.setAttributeBuffer(    PROGRAM_NORMAL_ATTRIBUTE,      GL_FLOAT, 3 * c_float_size, 3, c_vertex_length * c_float_size);
    ///m_default_shader.setAttributeBuffer(    PROGRAM_TEXCOORD_ATTRIBUTE,    GL_FLOAT, 6 * c_float_size, 2, c_vertex_length * c_float_size);
    ///m_default_shader.setAttributeBuffer(    PROGRAM_BARYCENTRIC_ATTRIBUTE, GL_FLOAT, 8 * c_float_size, 3, c_vertex_length * c_float_size);
    //          Standard OpenGL Calls
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
    if (object == nullptr) return;

    // Load possible 3D Info
    ThingComp3D *comp_3d = object->comp3D();
    double comp_3d_angle_x { 0.0 };                                 // X axis rotation
    double comp_3d_angle_y { 0.0 };                                 // Y axis rotation
    double comp_3d_rotate_x_speed { 0.0 };                          // X axis rotation speed
    double comp_3d_rotate_y_speed { 0.0 };                          // Y axis rotation speed
    double comp_3d_depth { 0.0 };                                   // Desired 3D Depth of 2D Objects
    if (comp_3d != nullptr) {
        comp_3d_angle_x =        object->comp3D()->getAngleX();
        comp_3d_angle_y =        object->comp3D()->getAngleY();
        comp_3d_rotate_x_speed = object->comp3D()->getRotateSpeedX();
        comp_3d_rotate_y_speed = object->comp3D()->getRotateSpeedY();
        comp_3d_depth =          object->comp3D()->getDepth();
    }

    // Don't draw Segments (lines)
    if (object->shapes.size() > 0) {
        if (object->shape_type[object->shapes[0]] == Shape_Type::Segment) return;
    }

    // Enable shader program
    if (!m_simple_shader.bind()) return;

    // ***** Blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);                    // Premultiplied alpha blend

    // ***** Get texture to render with, set texture coordinates
    DrEngineTexture *texture;
    long texture_number = object->getTextureNumber();
    if (object->animation_idle_keys.size() > 0) {
        texture_number = object->animation_idle_keys[object->animation_idle_frame - 1];
    }
    texture = m_engine->getTexture(texture_number);
    if (texture == nullptr) return;
    if (!texture->texture()->isBound()) texture->texture()->bind();

    // ***** Set Matrix for Shader, calculates current matrix, adds in object location
    float x =   static_cast<float>(thing->getPosition().x + m_add_z);
    float y =   static_cast<float>(thing->getPosition().y + m_add_z);
    float z =   static_cast<float>(thing->getZOrder()     + m_add_z);
    double now = Dr::MillisecondsSinceStartOfDay() / 10.0;

    // Translate
    QMatrix4x4 model;
    model.translate(x, y, z);

    // Rotate
    model.rotate(static_cast<float>(object->getAngle()), 0.f, 0.f, 1.f);
    model.rotate(static_cast<float>(comp_3d_angle_x + (now * comp_3d_rotate_x_speed)), 1.f, 0.f, 0.f);
    model.rotate(static_cast<float>(comp_3d_angle_y + (now * comp_3d_rotate_y_speed)), 0.f, 1.f, 0.f);

    // Scale
    float flip_x = (object->isFlippedX()) ? -1.0 : 1.0;
    float flip_y = (object->isFlippedY()) ? -1.0 : 1.0;
    model.scale(static_cast<float>(texture->width()) * flip_x, static_cast<float>(texture->height()) * flip_y, 1.0f);
    model.scale( object->getScaleX(), object->getScaleY(), static_cast<float>(comp_3d_depth) );

    // ***** Fade Away / Shrink Dying Object (Death Animation
    float alpha = object->getOpacity();                                                 // Start with object alpha
    if (!object->isAlive() && object->getDeathAnimation() != Death_Animation::None) {
        float fade_percent = 1.0f - (static_cast<float>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<float>(object->getDeathDuration()));
        if (object->getDeathAnimation() == Death_Animation::Fade) {
            alpha *= static_cast<float>(fade_percent);
        } else if (object->getDeathAnimation() == Death_Animation::Shrink) {
            model.scale( fade_percent, fade_percent );
        }
    }

    m_simple_shader.setUniformValue( u_simple_matrix,         m_projection * m_view * model );

    // ***** Set Shader Variables
    m_simple_shader.setUniformValue( u_simple_texture, 0 );                           // Use texture unit 0
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
    if (thing->getThingType() != DrThingType::Object) return false;
    DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
    if (object == nullptr) return false;

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
        QMatrix4x4 view_matrix, proj_matrix;
        occluderMatrix(m_engine->getCurrentWorld()->render_type, view_matrix, proj_matrix);
        m_occluder_shader.setUniformValue(      u_occluder_matrix, (proj_matrix * view_matrix) );

        // Set Texture Coordinates for Shader
        m_occluder_shader.setAttributeArray(    a_occluder_texture_coord, m_quad_texture_coordinates.data(), 2 );
        m_occluder_shader.enableAttributeArray( a_occluder_texture_coord );
    }

    // ***** Get texture to render with, set texture coordinates
    DrEngineTexture *texture;
    long texture_number = object->getTextureNumber();
    if (object->animation_idle_keys.size() > 0) {
        texture_number = object->animation_idle_keys[object->animation_idle_frame - 1];
    }
    texture = m_engine->getTexture(texture_number);
    if (texture == nullptr) return true;
    if (!texture->texture()->isBound()) texture->texture()->bind();
    double texture_width =  texture->width();
    double texture_height = texture->height();

    // ***** Fade Away / Shrink Dying Object (Death Animation
    float alpha = object->getOpacity();                                                     // Start with object alpha
    float animation_scale = 1.0;
    if (!object->isAlive() && object->getDeathAnimation() != Death_Animation::None) {
        float fade_percent = 1.0f - (static_cast<float>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<float>(object->getDeathDuration()));
        if (object->getDeathAnimation() == Death_Animation::Fade) {
            alpha *= static_cast<float>(fade_percent);
        } else if (object->getDeathAnimation() == Death_Animation::Shrink) {
            animation_scale = fade_percent;
        }
    }

    // ***** Load vertices for this object
    float flip_x = (object->isFlippedX()) ? -animation_scale : animation_scale;
    float flip_y = (object->isFlippedY()) ? -animation_scale : animation_scale;
    std::vector<GLfloat> vertices;
    getThingVertices(vertices, object, texture_width, texture_height, flip_x, flip_y);
    m_occluder_shader.setAttributeArray(    a_occluder_vertex, vertices.data(), 3 );
    m_occluder_shader.enableAttributeArray( a_occluder_vertex );

    // ***** Set Shader Variables
    m_occluder_shader.setUniformValue( u_occluder_texture,      0 );                        // Texture unit 0
    m_occluder_shader.setUniformValue( u_occluder_alpha,        alpha );
    m_occluder_shader.setUniformValue( u_occluder_depth,        static_cast<float>(object->getZOrder()) );
    m_occluder_shader.setUniformValue( u_occluder_near_plane,   c_near_plane );
    m_occluder_shader.setUniformValue( u_occluder_far_plane,    c_far_plane );

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
    if (fire == nullptr) return false;

    // ***** Enable shader program
    if (!m_fire_shader.bind()) return false;

    // Load possible 3D Info
    ThingComp3D *comp_3d = fire->comp3D();
    double comp_3d_angle_x { 0.0 };                                 // X axis rotation
    double comp_3d_angle_y { 0.0 };                                 // Y axis rotation
    double comp_3d_rotate_x_speed { 0.0 };                          // X axis rotation speed
    double comp_3d_rotate_y_speed { 0.0 };                          // Y axis rotation speed
    bool   comp_3d_billboard { false };                             // Make Object face camera?
    double comp_3d_depth { 0.0 };                                   // Desired 3D Depth of 2D Objects
    if (comp_3d != nullptr) {
        comp_3d_angle_x =        fire->comp3D()->getAngleX();
        comp_3d_angle_y =        fire->comp3D()->getAngleY();
        comp_3d_rotate_x_speed = fire->comp3D()->getRotateSpeedX();
        comp_3d_rotate_y_speed = fire->comp3D()->getRotateSpeedY();
        comp_3d_billboard =      fire->comp3D()->getBillboard();
        comp_3d_depth =          fire->comp3D()->getDepth();
    }

    // ***** Blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);           // Standard non-premultiplied alpha blend

    // ***** Set Matrix for Shader, calculates current matrix, adds in object location
    float x =   static_cast<float>(thing->getPosition().x + m_add_z);
    float y =   static_cast<float>(thing->getPosition().y + m_add_z);
    float z =   static_cast<float>(thing->getZOrder()     + m_add_z);
    double now = Dr::MillisecondsSinceStartOfDay() / 10.0;

    // Translate
    QMatrix4x4 model;
    model.translate(x, y, z);

    // Rotate
    model.rotate(static_cast<float>(fire->getAngle()), 0.f, 0.f, 1.f);
    model.rotate(static_cast<float>(comp_3d_angle_x + (now * comp_3d_rotate_x_speed)), 1.f, 0.f, 0.f);
    model.rotate(static_cast<float>(comp_3d_angle_y + (now * comp_3d_rotate_y_speed)), 0.f, 1.f, 0.f);

    // Rotate Billboards
    if (comp_3d_billboard) {
        glm::vec3 obj{x, y, z};
        glm::vec3 eye = glm::vec3(m_eye.x(), m_eye.y(), m_eye.z()) / combinedZoomScale();
        glm::mat4 billboard = glm::lookAt(obj, eye, glm::vec3(m_up.x(), m_up.y(), m_up.z()));
        billboard = glm::inverse(billboard);
        model = QMatrix4x4(glm::value_ptr(billboard)).transposed();
    }

    // Scale
    model.scale( static_cast<float>(fire->getSize().x), static_cast<float>(fire->getSize().y), 1.0f );
    float final_x_scale = (fire->getScaleX());
    float final_y_scale = (fire->getScaleY());
    model.scale( final_x_scale, final_y_scale, static_cast<float>(comp_3d_depth) );

    // Reverse Culling for Flipped Objects
    if ((final_x_scale < 0 && final_y_scale > 0) || (final_x_scale > 0 && final_y_scale < 0)) cullingOn(true);


    // ***** Set Matrix for Shader, calculates current matrix
    QMatrix4x4 mvp = m_projection * m_view * model;
    m_fire_shader.setUniformValue( u_fire_matrix,         mvp );

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
    m_fire_shader.setAttributeArray(    a_fire_vertex, m_quad_vertices.data(), 3 );
    m_fire_shader.enableAttributeArray( a_fire_vertex );

    // ***** Set Shader Variables
    now = Dr::MillisecondsSinceStartOfDay() / 1000.0;
    m_fire_shader.setUniformValue( u_fire_alpha,    fire->getOpacity() );
    m_fire_shader.setUniformValue( u_fire_time,     static_cast<float>(now) );
    m_fire_shader.setUniformValue( u_fire_position, static_cast<float>(thing->getPosition().x), static_cast<float>(thing->getPosition().y) );
    m_fire_shader.setUniformValue( u_fire_width,    static_cast<float>(fire->getSize().x) * fire->getScaleX() );
    m_fire_shader.setUniformValue( u_fire_height,   static_cast<float>(fire->getSize().y) * fire->getScaleY() );

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















