//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QPainter>
#include "glu.h"

#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Render, Paint the Scene
//####################################################################################
void OpenGL::paintGL() {

    // ***** Initialize painter
    QPainter painter( this );
    painter.beginNativePainting();

    // ***** Make sure viewport is sized correctly and clear the buffers
    //glViewport(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
    glClearColor(0.0, 0.0, 0.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    //glEnable( GL_DEPTH_TEST  );                          // Enable depth test
    ///glEnable( GL_MULTISAMPLE );                          // Enable anti aliasing

    // Enable face culling for triangles facing away from view
    glEnable( GL_CULL_FACE );
    glCullFace(  GL_BACK );
    glFrontFace( GL_CCW );

    // Enable alpha channel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Standard blend function
    ///glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
    ///glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // Alpha clamping
    ///glEnable(GL_ALPHA_TEST);
    ///glAlphaFunc(GL_GREATER,0);                           // 0.0 (transparent) to 1.0 (opaque)



    // ***** Set camera position
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());

    QMatrix4x4 matrix_model_view;
    if (m_engine->render_type == RenderType::Orthographic) {
        double scale =  static_cast<double>(m_scale);
        float cam_x = m_engine->getCameraPos().x() * static_cast<float>(scale);
        float cam_y = m_engine->getCameraPos().y() * static_cast<float>(scale);

        float left =   static_cast<float>(cam_x - (width() /  2.0f));
        float right =  static_cast<float>(cam_x + (width() /  2.0f));
        float top =    static_cast<float>(cam_y + (height() / 2.0f));
        float bottom = static_cast<float>(cam_y - (height() / 2.0f));
        matrix_model_view.ortho( left, right, bottom, top,  -1.0f, 1.0f);

    } else {
        matrix_model_view.perspective( 50.0f, aspect_ratio, 1.0f, 1000.0f );

        // Sets the camera back 800 pixels
        matrix_model_view.translate( m_engine->getCameraPos().x() * -m_scale, m_engine->getCameraPos().y() * -m_scale, m_engine->getCameraPos().z() );
        matrix_model_view.scale( m_scale );
    }

    // Rotates the camera around the center of the sceen
    //m_angle += 1.0f;
    //if (m_angle > 360) m_angle = 0;
    //m_model_view.rotate( m_angle, 0.0f, 1.0f, 0.0f );



    //myGlobals.matWorld = Matrix.Identity;
    //myGlobals.matView = Matrix.CreateLookAt(myGlobals.cameraPosition, new Vector3(0, 0, 0), Vector3.Up);
    //myGlobals.matProj = Matrix.CreateOrthographic(10f, 10f, 0.01f, 500.0f);
    //mousePoint = theViewport.Project(flex.myCube[block].polyFaces[i].myVertices[ic].Position, globals.matProj, globals.matView, globals.matWorld);


    // ***** Enable shader program
    if (!m_program.bind()) return;
    m_program.setUniformValue( m_matrixUniform, matrix_model_view );

    drawCube();

    // ********** Draw each object in space
    for (auto object : m_engine->objects) {
        if (object->shape_type == ShapeType::Segment) continue;

        // ***** Render with texture
        EngineTexture *texture = m_engine->getTexture(object->texture_number);
        texture->texture()->bind();

        std::vector<float> texCoords;
        texCoords.clear();
        texCoords.resize( 8 );
        float one_x = (1 / texture->width())  * c_texture_border;
        float one_y = (1 / texture->height()) * c_texture_border;
        texCoords[0] = 1 - one_x;    texCoords[1] = 1 - one_y;
        texCoords[2] =     one_x;    texCoords[3] = 1 - one_y;
        texCoords[4] = 1 - one_x;    texCoords[5] =     one_y;
        texCoords[6] =     one_x;    texCoords[7] =     one_y;
        m_program.setAttributeArray( m_texCoordAttr, texCoords.data(), 2 );
        m_program.enableAttributeArray( m_texCoordAttr );


        // ***** Get object position data
        QPointF center = object->position;
        float x, y, half_width, half_height;

        if (m_engine->render_type == RenderType::Orthographic) {
            x = static_cast<float>(center.x()) * m_scale;
            y = static_cast<float>(center.y()) * m_scale;
            half_width =  float(texture->width())  * m_scale / 2.0f;
            half_height = float(texture->height()) * m_scale / 2.0f;
        } else {
            x = static_cast<float>(center.x());
            y = static_cast<float>(center.y());
            half_width =  float(texture->width())  / 2.0f;
            half_height = float(texture->height()) / 2.0f;
        }


        // ***** Create rotation matrix, apply rotation to object
        QMatrix4x4 matrix;
        matrix.rotate( static_cast<float>(-object->angle), 0.0, 0.0, 1.0 );
        QVector3D top_right = QVector3D( half_width,  half_height, 0) * matrix;
        QVector3D top_left =  QVector3D(-half_width,  half_height, 0) * matrix;
        QVector3D bot_right = QVector3D( half_width, -half_height, 0) * matrix;
        QVector3D bot_left =  QVector3D(-half_width, -half_height, 0) * matrix;


        // ***** Load vertices for this object, this includes added size of c_texture_border
        QVector<float> vertices;
        vertices.clear();
        vertices.resize( 12 );              // in sets of x, y, z
        // Top Right
        vertices[0] = top_right.x() + x;
        vertices[1] = top_right.y() + y;
        vertices[2] = 0.00f;
        // Top Left
        vertices[3] = top_left.x()  + x;
        vertices[4] = top_left.y()  + y;
        vertices[5] = 0.00f;
        // Bottom Right
        vertices[6] = bot_right.x() + x;
        vertices[7] = bot_right.y() + y;
        vertices[8] = 0.00f;
        // Bottom Left
        vertices[ 9] = bot_left.x() + x;
        vertices[10] = bot_left.y() + y;
        vertices[11] = 0.00f;

        m_program.setAttributeArray( m_vertexAttr, vertices.data(), 3 );
        m_program.setUniformValue( m_texUniform, 0 );                           // Use texture unit 0
        m_program.enableAttributeArray( m_vertexAttr );

        // ***** Draw triangles using shader program
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );                                // GL_TRIANGLES

        // ***** Disable arrays
        m_program.disableAttributeArray( m_vertexAttr );
        m_program.disableAttributeArray( m_texCoordAttr );

        texture->texture()->release();
    }

    // ***** Disable shader program, end native drawing
    m_program.release();

    glDisable( GL_CULL_FACE );              // Must turn off culling for QPainter for some reason
    painter.endNativePainting();



    // ********** Draws debug shapes onto screen
    painter.setRenderHint(QPainter::Antialiasing, true);

    int t = 0;
    QString info = "";
    if (m_engine->debug) {


        for (auto object : m_engine->objects) {
            if (object->in_scene == false) continue;

            QColor color;
            switch (object->body_type) {
                case BodyType::Dynamic:         color = Qt::red;        break;
                case BodyType::Static:          color = Qt::blue;       break;
                case BodyType::Kinematic:       color = Qt::green;      break;
            }

            QPen cosmetic_pen( QBrush(color), 1);
            cosmetic_pen.setCosmetic(true);
            painter.setPen( cosmetic_pen );
            QColor brush_color = color;
            brush_color.setAlpha(64);
            painter.setBrush( QBrush( brush_color));

            if (object->shape_type == ShapeType::Circle) {
                double radius = cpCircleShapeGetRadius(object->shape) * static_cast<double>(m_scale);
                float  x_pos = static_cast<float>( object->position.x()) * m_scale;
                float  y_pos = static_cast<float>( object->position.y()) * m_scale;

                QMatrix4x4 identity;
                identity.setToIdentity();

                QMatrix4x4 projection;
                projection.perspective( 50.0f, aspect_ratio, 1.0f, 1000.0f );

                QMatrix4x4 model_view;
                model_view.translate( x_pos - (m_engine->getCameraPos().x() * m_scale), y_pos - (m_engine->getCameraPos().y() * m_scale), 0);


                QVector3D point;
                if (m_engine->render_type == RenderType::Orthographic) {
                    point = QVector3D( -x_pos, height() - y_pos, 0).unproject(identity, matrix_model_view, QRect(0, 0, width(), height()));
                } else {
                    point = QVector3D( 0, 0, 0).unproject(model_view, projection, QRect(0, 0, width(), height()));
                    point.setX( point.x() - width() / 2);
                    point.setY( point.y() + height() / 2);
                }




                //QPointF top (center.x(), center.y() - radius);
                double xd = static_cast<double>(point.x());
                double yd = static_cast<double>(point.y());
                painter.drawEllipse( QPointF( -xd, yd ), radius, radius );


                if (t==0) {
                    info += " - PX: " + QString::number(xd) + ", Y: " + QString::number(yd);
                }
                t++;

                //QTransform t = QTransform().translate(center.x(), center.y()).rotate(-object->angle).translate(-center.x(), -center.y());
                //QPointF l2 = t.map( top );
                //painter.drawLine( center, l2 );

    //        } else if (object->shape_type == ShapeType::Box) {
    //            QRectF    box = object->item->boundingRect();
    //            QPolygonF rotated_box = mapFromScene( object->item->sceneTransform().map( box ) );
    //            painter.drawPolygon( rotated_box );

    //        } else if (object->shape_type == ShapeType::Segment) {
    //            cpVect l1 = cpSegmentShapeGetA( object->shape);
    //            cpVect l2 = cpSegmentShapeGetB( object->shape);

    //            QPointF p1, p2;
    //            p1.setX(l1.x);  p1.setY(-l1.y);
    //            p2.setX(l2.x);  p2.setY(-l2.y);
    //            p1 = mapFromScene( p1 );
    //            p2 = mapFromScene( p2 );

    //            QPen line_pen( QBrush(color), 2);
    //            painter.setPen( line_pen );
    //            painter.drawLine(p1, p2);

    //        } else if (object->shape_type == ShapeType::Polygon) {
    //            QPolygonF poly;
    //            for (int i = 0; i < cpPolyShapeGetCount( object->shape ); i++) {
    //                cpVect vert = cpPolyShapeGetVert( object->shape, i );
    //                poly.append( QPointF(vert.x + (object->item->boundingRect().width() / 2), -vert.y + (object->item->boundingRect().height() / 2)) );
    //            }
    //            poly = mapFromScene( object->item->sceneTransform().map(poly) );
    //            painter.drawPolygon( poly );

            }
        }
    }       // end if (debug)

    painter.end();

    info += ", T: " + QString::number(t);

    // Show frames per second
    m_engine->fps++;
    if (m_engine->fps_timer.elapsed() > 1000) {
        emit updateInfo(info);
        m_engine->last_fps = m_engine->fps;
        m_engine->fps = 0;
        m_engine->fps_timer.restart();
    }
}








// Draws a rotating cube
void OpenGL::drawCube() {

    static float angle = 0;
    angle++;
    if (angle > 360) angle = 0;

    EngineTexture *texture = m_engine->getTexture( 1 );     // 1 is metal_block
    texture->texture()->bind();

    for (int i = 0; i < 5; i++) {
        std::vector<float> texCoords;
        texCoords.clear();
        texCoords.resize( 8 );
        float one_x = (1 / texture->width())  * c_texture_border;
        float one_y = (1 / texture->height()) * c_texture_border;
        texCoords[0] = 1 - one_x;    texCoords[1] = 1 - one_y;
        texCoords[2] =     one_x;    texCoords[3] = 1 - one_y;
        texCoords[4] = 1 - one_x;    texCoords[5] =     one_y;
        texCoords[6] =     one_x;    texCoords[7] =     one_y;
        m_program.setAttributeArray( m_texCoordAttr, texCoords.data(), 2 );
        m_program.enableAttributeArray( m_texCoordAttr );


        // ***** Get object position data
        QPointF center = QPointF(0, 300);
        float x, y, half_width, half_height;
        float half_no_border;

        float multi =  3;
        float width =  texture->width() *  multi;
        float height = texture->height() * multi;

        if (m_engine->render_type == RenderType::Orthographic) {
            x = static_cast<float>(center.x()) * m_scale;
            y = static_cast<float>(center.y()) * m_scale;
            half_width =  width  * m_scale / 2.0f;
            half_height = height * m_scale / 2.0f;
            half_no_border = (width - c_texture_border * 2 * multi) * m_scale / 2.0f;
        } else {
            x = static_cast<float>(center.x());
            y = static_cast<float>(center.y());
            half_width =  width  / 2.0f;
            half_height = height / 2.0f;
            half_no_border = (width - c_texture_border * 2 * multi) / 2.0f;
        }


        // ***** Create rotation matrix, apply rotation to object
        QMatrix4x4 matrix;
        if (i == 4) {
            matrix.rotate( static_cast<float>( -angle  ), 0.0, 0.0, 1.0 );
            matrix.rotate( -90, 1.0, 0.0, 0.0 );
        } else {
            matrix.rotate( static_cast<float>( angle + (90 * i) ), 0.0, 1.0, 0.0 );
        }
        matrix.rotate( static_cast<float>( 45               ), 1.0, 0.0, 1.0 );


        QVector3D top_right, top_left, bot_right, bot_left;
        top_right = QVector3D( half_width,  half_height, -half_no_border) * matrix;
        top_left =  QVector3D(-half_width,  half_height, -half_no_border) * matrix;
        bot_right = QVector3D( half_width, -half_height, -half_no_border) * matrix;
        bot_left =  QVector3D(-half_width, -half_height, -half_no_border) * matrix;


        // ***** Load vertices for this object, this includes added size of c_texture_border
        QVector<float> vertices;
        vertices.clear();
        vertices.resize( 12 );  // in sets of x, y, z
        // Top Right
        vertices[0] = top_right.x() + x;
        vertices[1] = top_right.y() + y;
        vertices[2] = 0.00f;
        // Top Left
        vertices[3] = top_left.x()  + x;
        vertices[4] = top_left.y()  + y;
        vertices[5] = 0.00f;
        // Bottom Right
        vertices[6] = bot_right.x() + x;
        vertices[7] = bot_right.y() + y;
        vertices[8] = 0.00f;
        // Bottom Left
        vertices[ 9] = bot_left.x() + x;
        vertices[10] = bot_left.y() + y;
        vertices[11] = 0.00f;

        m_program.setAttributeArray( m_vertexAttr, vertices.data(), 3 );
        m_program.setUniformValue( m_texUniform, 0 );
        m_program.enableAttributeArray( m_vertexAttr );

        // ***** Draw triangles using shader program
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );            // GL_TRIANGLES

        // ***** Disable arrays
        m_program.disableAttributeArray( m_vertexAttr );
        m_program.disableAttributeArray( m_texCoordAttr );
    }

    texture->texture()->release();

}









