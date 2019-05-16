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
#include "engine/engine_texture.h"
#include "opengl/opengl.h"

//####################################################################################
//##        Render, Paint the Scene
//####################################################################################
void OpenGL::paintGL() {

    // ********** Initialize painter
    QPainter painter( this );
    painter.beginNativePainting();

    // ********** Make sure viewport is sized correctly and clear the buffers
    ///glViewport(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
    float background_red =   static_cast<float>(m_engine->getBackgroundColor().redF());
    float background_green = static_cast<float>(m_engine->getBackgroundColor().greenF());
    float background_blue =  static_cast<float>(m_engine->getBackgroundColor().blueF());
    glClearColor(background_red, background_green, background_blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    ///glEnable( GL_DEPTH_TEST  );                          // Enable depth test
    ///glEnable( GL_MULTISAMPLE );                          // Enable anti aliasing

    // Enable alpha channel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Standard blend function
    ///glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    ///glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // Alpha clamping
    ///glEnable(GL_ALPHA_TEST);
    ///glAlphaFunc(GL_GREATER,0);                           // 0.0 (transparent) to 1.0 (opaque)



    // ********** Set camera position
    // Axis:
    //  -X left,  X right
    //  -Y down,  Y up
    //  -Z back,  Z front
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());

    QVector3D  perspective_offset ( 250.0f, 250.0f, 0.0f);
    QVector3D  eye(     m_engine->getCameraPos().x() * m_scale + perspective_offset.x(),
                        m_engine->getCameraPos().y() * m_scale + perspective_offset.y(),
                        m_engine->getCameraPos().z() );
    QVector3D  look_at( m_engine->getCameraPos().x() * m_scale,
                        m_engine->getCameraPos().y() * m_scale,    0 );
    QVector3D  up(      0, 1, 0);
    m_model_view.setToIdentity();
    m_projection.setToIdentity();

    if (m_engine->render_type == Render_Type::Orthographic) {
        float cam_x = m_engine->getCameraPos().x() * m_scale;
        float cam_y = m_engine->getCameraPos().y() * m_scale;
        float left =   static_cast<float>(cam_x - (width() /  2.0f));
        float right =  static_cast<float>(cam_x + (width() /  2.0f));
        float top =    static_cast<float>(cam_y + (height() / 2.0f));
        float bottom = static_cast<float>(cam_y - (height() / 2.0f));
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
    QMatrix4x4 m_matrix = m_projection * m_model_view;




    // ********** Enable shader program
    if (!m_program.bind()) return;
    m_program.setUniformValue( m_uniform_matrix, m_matrix );


    // ***** Render 3D Objects - before rendering 3D objects, enable face culling for triangles facing away from view
    glEnable( GL_CULL_FACE );
    glCullFace(  GL_BACK );
    glFrontFace( GL_CCW );

    drawCube( QVector3D( -400, 400, -300) );

    // Turn off culling before drawing 2D objects, ALSO: Must turn OFF culling for QPainter to work
    glDisable( GL_CULL_FACE );




    // ***** Create a vector of the scene objects (ignoring lines / segments) and sort it by depth
    std::vector<std::pair<int, double>> v;
    for (int i = 0; i < m_engine->objects.count(); i++) {
        if (m_engine->objects[i]->shape_type == Shape_Type::Segment)
            continue;
        v.push_back(std::make_pair(i, m_engine->objects[i]->z_order));
    }
    sort(v.begin(), v.end(), [] (std::pair<int, double>&i, std::pair<int, double>&j) { return i.second < j.second; });


    // ***** Render 2D Objects
    ///for (auto object : m_engine->objects) {
    for (ulong i = 0; i < static_cast<ulong>(v.size()); i++) {
        SceneObject *object = m_engine->objects[ v[i].first ];
        if (!object->has_been_processed) continue;

        // ***** Render with texture
        DrEngineTexture *texture = m_engine->getTexture(object->texture_number);

        if (!texture->texture()->isBound())
            texture->texture()->bind();

        std::vector<float> texCoords;
        texCoords.clear();
        texCoords.resize( 8 );
        float one_x =  1 / texture->width();
        float one_y =  1 / texture->height();
        texCoords[0] = 1 - one_x;    texCoords[1] = 1 - one_y;
        texCoords[2] =     one_x;    texCoords[3] = 1 - one_y;
        texCoords[4] = 1 - one_x;    texCoords[5] =     one_y;
        texCoords[6] =     one_x;    texCoords[7] =     one_y;
        m_program.setAttributeArray( m_attribute_tex_coord, texCoords.data(), 2 );
        m_program.enableAttributeArray( m_attribute_tex_coord );


        // ***** Get object position data
        QPointF center = object->position;
        float x, y, z, half_width, half_height;

        if (m_engine->render_type == Render_Type::Orthographic) {
            x = static_cast<float>(center.x()) * m_scale;
            y = static_cast<float>(center.y()) * m_scale;
            z = static_cast<float>(object->z_order) * m_scale;
            half_width =  float(texture->width()) *  object->scale_x * m_scale / 2.0f;
            half_height = float(texture->height()) * object->scale_y * m_scale / 2.0f;
        } else {
            x = static_cast<float>(center.x());
            y = static_cast<float>(center.y());
            z = static_cast<float>(object->z_order);
            half_width =  float(texture->width()) *  object->scale_x / 2.0f;
            half_height = float(texture->height()) * object->scale_y / 2.0f;
        }


        // ***** Create rotation matrix, apply rotation to object
        QMatrix4x4 matrix;
        matrix.rotate( static_cast<float>(-object->angle), 0.0, 0.0, 1.0 );
        QVector3D top_right = QVector3D( half_width,  half_height, 0) * matrix;
        QVector3D top_left =  QVector3D(-half_width,  half_height, 0) * matrix;
        QVector3D bot_right = QVector3D( half_width, -half_height, 0) * matrix;
        QVector3D bot_left =  QVector3D(-half_width, -half_height, 0) * matrix;


        // ***** Load vertices for this object
        QVector<float> vertices;
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

        m_program.setAttributeArray( m_attribute_vertex, vertices.data(), 3 );
        m_program.enableAttributeArray( m_attribute_vertex );

        m_program.setUniformValue( m_uniform_texture, 0 );                          // Use texture unit 0
        m_program.setUniformValue( m_uniform_alpha, object->alpha );                // Add object alpha

        // ***** Draw triangles using shader program
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );                                    // GL_TRIANGLES

        // ***** Disable arrays
        m_program.disableAttributeArray( m_attribute_vertex );
        m_program.disableAttributeArray( m_attribute_tex_coord );

        ///texture->texture()->release();
    }


    // ***** Render Front 3D Objects
    glEnable( GL_CULL_FACE );
    glCullFace(  GL_BACK );
    glFrontFace( GL_CCW );

    drawCube( QVector3D(0, 300, 600) );

    glDisable( GL_CULL_FACE );


    // ***** Disable shader program, end native drawing
    m_program.release();
    painter.endNativePainting();



    // ********** Draws debug shapes onto screen
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (m_engine->debug_shapes)     drawDebugShapes(painter);
    if (m_engine->debug_collisions) drawDebugCollisions(painter);

    painter.end();


    // ***** Show frames per second
    m_engine->fps++;
    if (m_engine->fps_timer.elapsed() > 1000) {
        emit updateInfo(m_engine->info);
        m_engine->last_fps = m_engine->fps;
        m_engine->fps = 0;
        m_engine->fps_timer.restart();
    }
}



















