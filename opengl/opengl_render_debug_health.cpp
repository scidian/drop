//
//      Created by Stephens Nunnally on 6/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QPainter>
#include <cmath>

#include "engine/engine.h"
#include "engine/engine_object.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "forms/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##    Draws the health of each object using QPainter
//####################################################################################
void OpenGL::drawDebugHealth(QPainter &painter) {
    QFont health_font("Avenir", static_cast<int>(18 * m_scale));
    painter.setPen(Qt::NoPen);

    for (auto object : m_engine->getCurrentWorld()->objects) {
        if (!object->shouldProcess())       continue;
        if (!object->hasBeenProcessed())    continue;

        // Figure out what color to make the debug shapes
        QColor color = objectDebugColor(object, true);

        // Load Object Position
        QPointF center =object->getBodyPosition();
        QPointF text_coord = mapToScreen(center.x(), center.y(), 0);

        if (rect().contains( text_coord.toPoint() )) {
            // Health as a QPainterPath
            QPainterPath health;
            QString hp = Dr::RemoveTrailingDecimals( object->getHealth(), 2 );
            health.addText(text_coord, health_font, hp);
            painter.setBrush( QBrush(color) );

            double fw = Dr::CheckFontWidth(health_font, hp);
            painter.translate( -(fw / 2.0), health.boundingRect().height() * 1.5);
            painter.drawPath(health);
            painter.resetTransform();
        }
    }
}


//####################################################################################
//##    Draws the health of each object using Native GL
//####################################################################################
void OpenGL::drawDebugHealthNative(QPainter &painter) {
    painter.beginNativePainting();

    const float font_size = 12.0f;
    const float spacing = 0.75f;

    // ***** Enable shader program
    if (!m_shader.bind()) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set texture to numbers
    DrEngineTexture *texture = m_engine->getTexture( Asset_Textures::Numbers );
    texture->texture()->bind();

    // Set Matrix for Shader, calculates current matrix
    QMatrix4x4 m_matrix = m_projection * m_model_view;
    m_shader.setUniformValue( m_uniform_matrix, m_matrix );

    // ***** Loop through each object and draws its health
    for (auto object : m_engine->getCurrentWorld()->objects) {
        if (!object->shouldProcess())       continue;
        if (!object->hasBeenProcessed())    continue;

        // Get health as string
        QString hp = Dr::RemoveTrailingDecimals( object->getHealth(), 2 );
        if (hp.length() < 1) continue;

        // ***** Load object position
        QPointF center = object->getBodyPosition();
        float x, y, z, half_width, half_height;
        if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) {
            x = static_cast<float>(center.x()) * m_scale;
            y = static_cast<float>(center.y() - static_cast<double>(font_size)) * m_scale;
            z = static_cast<float>(object->getZOrder()) * m_scale;
            half_width =  font_size * m_scale / 2.0f;
            half_height = font_size * m_scale / 2.0f;
        } else {
            x = static_cast<float>(center.x());
            y = static_cast<float>(center.y() - static_cast<double>(font_size));
            z = static_cast<float>(object->getZOrder());
            half_width =  font_size / 2.0f;
            half_height = font_size / 2.0f;
        }

        QVector3D top_right = QVector3D( half_width,  half_height, 0);
        QVector3D top_left =  QVector3D(-half_width,  half_height, 0);
        QVector3D bot_right = QVector3D( half_width, -half_height, 0);
        QVector3D bot_left =  QVector3D(-half_width, -half_height, 0);

        // ***** Create Texture Coordinate and Vertex arrays
        std::vector<float> texture_coordinates;
        QVector<GLfloat> vertices;
        texture_coordinates.clear();
        vertices.clear();
        texture_coordinates.resize( static_cast<u_long>(8 * hp.length()) );
        vertices.resize( 12 * hp.length() );

        // Figure out what color to make the health, get health as string
        QColor color = objectDebugColor(object, true);

        // ***** Set Shader Variables
        setShaderDefaultValues(static_cast<float>(texture->width()), static_cast<float>(texture->height()));
        float r = static_cast<float>(color.redF());
        float g = static_cast<float>(color.greenF());
        float b = static_cast<float>(color.blueF());
        m_shader.setUniformValue( m_uniform_tint, r, g, b );

        // ***** Render each number as a textured quad
        x -= ((hp.length() - 1.0f) * (half_width * spacing));
        for (int i = 0; i < hp.length(); i++) {

            // Set Texture Coordinates
            setNumberTextureCoordinates(hp.at(i), texture_coordinates);

            // Set Vertices
            vertices[0] = top_right.x() + x;       vertices[ 1] = top_right.y() + y;      vertices[ 2] = z;      // Top Right
            vertices[3] = top_left.x()  + x;       vertices[ 4] = top_left.y()  + y;      vertices[ 5] = z;      // Top Left
            vertices[6] = bot_right.x() + x;       vertices[ 7] = bot_right.y() + y;      vertices[ 8] = z;      // Bottom Right
            vertices[9] = bot_left.x()  + x;       vertices[10] = bot_left.y()  + y;      vertices[11] = z;      // Bottom Left

            x += (half_width * 2.0f * spacing);

            m_shader.setAttributeArray( m_attribute_tex_coord, texture_coordinates.data(), 2 );
            m_shader.setAttributeArray( m_attribute_vertex, vertices.data(), 3 );
            m_shader.enableAttributeArray( m_attribute_tex_coord );
            m_shader.enableAttributeArray( m_attribute_vertex );

            // ***** Draw triangles using Shader
            glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

            // Release bound items
            m_shader.disableAttributeArray( m_attribute_tex_coord );
            m_shader.disableAttributeArray( m_attribute_vertex );
        }
    }

    m_shader.release();
    painter.endNativePainting();
}


void OpenGL::setNumberTextureCoordinates(QString letter, std::vector<float> &c) {
    //                        Top Right                     Top Left                    Bottom Right                Bottom Left
    if      (letter == "1") { c[0] = 0.25; c[1] = 1.00;     c[2] = 0.00; c[3] = 1.00;   c[4] = 0.25; c[5] = 0.75;   c[6] = 0.00; c[7] = 0.75; }
    else if (letter == "2") { c[0] = 0.50; c[1] = 1.00;     c[2] = 0.25; c[3] = 1.00;   c[4] = 0.50; c[5] = 0.75;   c[6] = 0.25; c[7] = 0.75; }
    else if (letter == "3") { c[0] = 0.75; c[1] = 1.00;     c[2] = 0.50; c[3] = 1.00;   c[4] = 0.75; c[5] = 0.75;   c[6] = 0.50; c[7] = 0.75; }
    else if (letter == "4") { c[0] = 1.00; c[1] = 1.00;     c[2] = 0.75; c[3] = 1.00;   c[4] = 1.00; c[5] = 0.75;   c[6] = 0.75; c[7] = 0.75; }

    else if (letter == "5") { c[0] = 0.25; c[1] = 0.75;     c[2] = 0.00; c[3] = 0.75;   c[4] = 0.25; c[5] = 0.50;   c[6] = 0.00; c[7] = 0.50; }
    else if (letter == "6") { c[0] = 0.50; c[1] = 0.75;     c[2] = 0.25; c[3] = 0.75;   c[4] = 0.50; c[5] = 0.50;   c[6] = 0.25; c[7] = 0.50; }
    else if (letter == "7") { c[0] = 0.75; c[1] = 0.75;     c[2] = 0.50; c[3] = 0.75;   c[4] = 0.75; c[5] = 0.50;   c[6] = 0.50; c[7] = 0.50; }
    else if (letter == "8") { c[0] = 1.00; c[1] = 0.75;     c[2] = 0.75; c[3] = 0.75;   c[4] = 1.00; c[5] = 0.50;   c[6] = 0.75; c[7] = 0.50; }

    else if (letter == "9") { c[0] = 0.25; c[1] = 0.50;     c[2] = 0.00; c[3] = 0.50;   c[4] = 0.25; c[5] = 0.25;   c[6] = 0.00; c[7] = 0.25; }
    else if (letter == "0") { c[0] = 0.50; c[1] = 0.50;     c[2] = 0.25; c[3] = 0.50;   c[4] = 0.50; c[5] = 0.25;   c[6] = 0.25; c[7] = 0.25; }
    else if (letter == "-") { c[0] = 0.75; c[1] = 0.50;     c[2] = 0.50; c[3] = 0.50;   c[4] = 0.75; c[5] = 0.25;   c[6] = 0.50; c[7] = 0.25; }
    else if (letter == "/") { c[0] = 1.00; c[1] = 0.50;     c[2] = 0.75; c[3] = 0.50;   c[4] = 1.00; c[5] = 0.25;   c[6] = 0.75; c[7] = 0.25; }

    else if (letter == ":") { c[0] = 0.25; c[1] = 0.25;     c[2] = 0.00; c[3] = 0.25;   c[4] = 0.25; c[5] = 0.00;   c[6] = 0.00; c[7] = 0.00; }
    else if (letter == "\""){ c[0] = 0.50; c[1] = 0.25;     c[2] = 0.25; c[3] = 0.25;   c[4] = 0.50; c[5] = 0.00;   c[6] = 0.25; c[7] = 0.00; }
    else if (letter == ".") { c[0] = 0.75; c[1] = 0.25;     c[2] = 0.50; c[3] = 0.25;   c[4] = 0.75; c[5] = 0.00;   c[6] = 0.50; c[7] = 0.00; }
    else                    { c[0] = 1.00; c[1] = 0.25;     c[2] = 0.75; c[3] = 0.25;   c[4] = 1.00; c[5] = 0.00;   c[6] = 0.75; c[7] = 0.00; }
}















