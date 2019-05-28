//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QPainter>

#include <vector>

#include "chipmunk/chipmunk.h"
#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_texture.h"
#include "forms/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Render, Paint the Scene
//####################################################################################
void OpenGL::paintGL() {

    // Start timer that calculates how long one render takes to shown onto screen
    m_time_frame = Clock::now();

    // Find OpenGL Version supported on this system
    ///auto ver = glGetString(GL_VERSION);
    ///m_engine->info = QString::fromUtf8(reinterpret_cast<const char*>(ver));

    // ********** Make sure viewport is sized correctly and clear the buffers
    ///glViewport(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
    float background_red =   static_cast<float>(m_engine->getBackgroundColor().redF());
    float background_green = static_cast<float>(m_engine->getBackgroundColor().greenF());
    float background_blue =  static_cast<float>(m_engine->getBackgroundColor().blueF());
    glClearColor(background_red, background_green, background_blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);///) | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
    ///glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    ///glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    // Enable depth / stencil test
    ///glEnable( GL_DEPTH_TEST  );
    ///glEnable( GL_STENCIL_TEST );

    // Enable alpha channel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Standard blend function

    // Enable anti aliasing
    glEnable( GL_MULTISAMPLE );
    ///glEnable(GL_POLYGON_SMOOTH);
    ///glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
    ///glSampleCoverage(GL_SAMPLE_ALPHA_TO_COVERAGE, GL_TRUE);

    // Alpha clamping
    ///glEnable(GL_ALPHA_TEST);
    ///glAlphaFunc(GL_GREATER,0);                           // 0.0 (transparent) to 1.0 (opaque)


    // ***** Update Camera / Matrices
    updateViewMatrix();

    // ***** Calculate time since last physics update as a percentage (and add how long a render takes)
    m_time_percent = (m_form_engine->getTimerMilliseconds(Engine_Timer::Physics) + m_time_one_frame_takes_to_render) / (1000.0 / m_engine->fps_physics);

    // ***** Render Scene
    drawObjects();


    // ********** Draws debug shapes onto screen
    QPainter painter( this );
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (m_engine->debug_shapes) {
        drawDebugShapes(painter);
        drawDebugJoints(painter);
    }
    if (m_engine->debug_collisions) {
        drawDebugCollisions(painter);
    }

    QFont font("Arial", 12);
    painter.setFont(font);
    painter.setPen( Qt::white );
    painter.drawText( QPointF(20, 20), "Items: " + QString::number( m_engine->objects.count()) + ", Scale: " + QString::number(double(m_scale)) );
    painter.drawText( QPointF(20, 40), "FPS: " + QString::number(m_engine->fps_render) + ", Physics: " + QString::number(m_engine->fps_physics));
    painter.drawText( QPointF(20, 60), g_info);

    ///int max_sample, max_text, max_number_textures, max_layers;
    ///glGetIntegerv ( GL_MAX_SAMPLES, &max_sample );                                      // Finds max multi sampling available on system
    ///glGetIntegerv ( GL_MAX_TEXTURE_SIZE, &max_text );                                   // Finds max texture size available on system
    ///glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_number_textures );        // Finds max number of textures can bind at one time
    ///glGetIntegerv ( GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers );
    ///painter.drawText( QPointF(20, 80), "Max Samples: " +  QString::number(max_sample));

    painter.end();

    // ********** Calculates Render Frames per Second
    ++m_fps_count;
    double fps_milli = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_fps).count() / 1000000.0;
    if (fps_milli > 1000.0) {
        m_engine->fps_render = m_fps_count;
        m_fps_count = 0;
        m_time_fps = Clock::now();
    }
}



//####################################################################################
//##        Update the view matrices before rendering
//####################################################################################
void OpenGL::updateViewMatrix() {
    //          Axis:
    //              -X left,  +X right
    //              -Y down,  +Y up
    //              -Z back,  +Z front
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());

    // Set camera position
    QVector3D  perspective_offset ( 200.0f, 200.0f, 0.0f);
    QVector3D  eye(     m_engine->getCameraPos().x() * m_scale + perspective_offset.x(),
                        m_engine->getCameraPos().y() * m_scale + perspective_offset.y(),
                        m_engine->getCameraPos().z() );
    QVector3D  look_at( m_engine->getCameraPos().x() * m_scale,
                        m_engine->getCameraPos().y() * m_scale,    0 );
    QVector3D  up(      0, 1, 0);

    // Create Matrices
    m_model_view.setToIdentity();
    m_projection.setToIdentity();
    if (m_engine->render_type == Render_Type::Orthographic) {
        float cam_x = m_engine->getCameraPos().x() * m_scale;
        float cam_y = m_engine->getCameraPos().y() * m_scale;
        float left =   cam_x - (width() /  2.0f);
        float right =  cam_x + (width() /  2.0f);
        float top =    cam_y + (height() / 2.0f);
        float bottom = cam_y - (height() / 2.0f);
        m_projection.ortho( left, right, bottom, top,  -1000.0f, 1000.0f);
    } else {
        m_projection.perspective( 70.0f, aspect_ratio, 1.0f, 5000.0f );
        m_model_view.lookAt(eye, look_at, up);
        m_model_view.scale( m_scale );
        // Rotates the camera around the center of the sceen
        ///m_angle += 1.0f;
        ///if (m_angle > 360) m_angle = 0;
        ///m_model_view.rotate( m_angle, 0.0f, 1.0f, 0.0f );
    }
}



//####################################################################################
//##        Render, Paint the Scene
//####################################################################################
// Before rendering 3D objects, enable face culling for triangles facing away from view
void OpenGL::cullingOn() {      glEnable( GL_CULL_FACE );   glCullFace(  GL_BACK );     glFrontFace( GL_CCW ); }
// Turn off culling before drawing 2D quads, NOTE: Must turn OFF culling for QPainter to work
void OpenGL::cullingOff() {     glDisable( GL_CULL_FACE ); }

// Renders All Scene Objects
void OpenGL::drawObjects() {

    // ***** Enable shader program
    if (!m_shader.bind()) return;

    // ***** Calculate / Apply Matrix
    QMatrix4x4 m_matrix = m_projection * m_model_view;
    m_shader.setUniformValue( m_uniform_matrix, m_matrix );

    // ***** Render 3D Objects
    cullingOn();
    drawCube( QVector3D( 2000, 400, -300) );
    cullingOff();


    // ***** Create a vector of the scene objects (ignoring lines / segments) and sort it by depth
    std::vector<std::pair<int, double>> v;
    for (int i = 0; i < m_engine->objects.count(); i++) {
        bool skip = false;
        for (auto shape : m_engine->objects[i]->shapes)
            if (m_engine->objects[i]->shape_type[shape] == Shape_Type::Segment)
                skip = true;
        if (skip) continue;

        v.push_back(std::make_pair(i, m_engine->objects[i]->z_order));
    }
    sort(v.begin(), v.end(), [] (std::pair<int, double>&i, std::pair<int, double>&j) { return i.second < j.second; });


    // ***** Render 2D Objects
    ///for (auto object : m_engine->objects) {
    for (ulong i = 0; i < static_cast<ulong>(v.size()); i++) {
        DrEngineObject *object = m_engine->objects[ v[i].first ];
        if (!object->has_been_processed) continue;                          // Don't draw Segments (lines)

        // ***** Render with texture
        DrEngineTexture *texture = m_engine->getTexture(object->texture_number);

        if (!texture->texture()->isBound())
            texture->texture()->bind();

        std::vector<float> texCoords;
        texCoords.clear();
        texCoords.resize( 8 );
        float one_x =  (1 / texture->width()) ;
        float one_y =  (1 / texture->height());
        texCoords[0] = 1 - one_x;    texCoords[1] = 1 - one_y;
        texCoords[2] =     one_x;    texCoords[3] = 1 - one_y;
        texCoords[4] = 1 - one_x;    texCoords[5] =     one_y;
        texCoords[6] =     one_x;    texCoords[7] =     one_y;
        m_shader.setAttributeArray( m_attribute_tex_coord, texCoords.data(), 2 );
        m_shader.enableAttributeArray( m_attribute_tex_coord );

        // ***** Get object position data
        QPointF center = (object->previous_position * (1.0 - m_time_percent)) + (object->position * m_time_percent);

        float x, y, z, half_width, half_height;
        if (m_engine->render_type == Render_Type::Orthographic) {
            x = static_cast<float>(center.x()) * m_scale;
            y = static_cast<float>(center.y()) * m_scale;
            z = static_cast<float>(object->z_order) * m_scale;
            half_width =  static_cast<float>(texture->width()) *  object->scale_x * m_scale / 2.0f;
            half_height = static_cast<float>(texture->height()) * object->scale_y * m_scale / 2.0f;
        } else {
            x = static_cast<float>(center.x());
            y = static_cast<float>(center.y());
            z = static_cast<float>(object->z_order);
            half_width =  static_cast<float>(texture->width()) *  object->scale_x / 2.0f + 1.0f;
            half_height = static_cast<float>(texture->height()) * object->scale_y / 2.0f + 1.0f;
        }


        // ***** Create rotation matrix, apply rotation to object
        QMatrix4x4 matrix;
        matrix.rotate( static_cast<float>(object->angle), 0.0, 0.0, 1.0 );
        QVector3D top_right = matrix * QVector3D( half_width,  half_height, 0);
        QVector3D top_left =  matrix * QVector3D(-half_width,  half_height, 0);
        QVector3D bot_right = matrix * QVector3D( half_width, -half_height, 0);
        QVector3D bot_left =  matrix * QVector3D(-half_width, -half_height, 0);

        // ***** Load vertices for this object
        QVector<GLfloat> vertices;
        vertices.clear();
        vertices.resize( 12 );              // in sets of x, y, z
        // Top Right
        vertices[0] = top_right.x() + x;
        vertices[1] = top_right.y() + y;
        vertices[2] = z;
        // Top Left
        vertices[3] = top_left.x()  + x;
        vertices[4] = top_left.y()  + y;
        vertices[5] = z;
        // Bottom Right
        vertices[6] = bot_right.x() + x;
        vertices[7] = bot_right.y() + y;
        vertices[8] = z;
        // Bottom Left
        vertices[ 9] = bot_left.x() + x;
        vertices[10] = bot_left.y() + y;
        vertices[11] = z;

        m_shader.setAttributeArray( m_attribute_vertex, vertices.data(), 3 );
        m_shader.enableAttributeArray( m_attribute_vertex );

        m_shader.setUniformValue( m_uniform_texture, 0 );                          // Use texture unit 0
        m_shader.setUniformValue( m_uniform_alpha, object->alpha );                // Add object alpha

        // ***** Draw triangles using shader program
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );                                    // GL_TRIANGLES

        // ***** Disable arrays
        m_shader.disableAttributeArray( m_attribute_vertex );
        m_shader.disableAttributeArray( m_attribute_tex_coord );

        ///texture->texture()->release();
    }

    // ***** Render Front 3D Objects
    cullingOn();
    drawCube( QVector3D(1600, 500, 600) );
    cullingOff();

    // *****Disable shader program
    m_shader.release();
}













