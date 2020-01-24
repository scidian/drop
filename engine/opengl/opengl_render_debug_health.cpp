//
//      Created by Stephens Nunnally on 6/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <cmath>

#include "core/dr_debug.h"
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Draws the health of each object using Native GL
//####################################################################################
void DrOpenGL::drawDebugHealth() {
    const float font_size = 12.0f;
    const float spacing = 0.75f;

    // ***** Enable shader program
    if (!m_default_shader.bind()) return;

    // ***** Blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);              // Standard non-premultiplied alpha blend

    // ***** Set texture to numbers
    DrEngineTexture *texture = m_engine->getTexture( Asset_Textures::Numbers );
    texture->texture()->bind();

    // ***** Set Matrix for Shader, calculates current matrix
    QMatrix4x4 m_matrix = m_projection * m_view;
    m_default_shader.setUniformValue( u_default_matrix, m_matrix );

    // ***** Loop through each object and draws its health
    for (auto thing : m_engine->getCurrentWorld()->getThings()) {
        if ( thing->getThingType() != DrThingType::Object)  continue;
        DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
        if (object->isPhysicsChild()) continue;

        // Get health as string
        std::string health = Dr::RoundToDecimalPlace(object->getHealth(), 2);
        std::string hp = Dr::RemoveTrailingZeros( health );
        if (hp.length() < 1) continue;

        // ***** Load object position
        DrPointF center = object->getPosition();
        float x, y, z;
        float half_width, half_height;
        x = static_cast<float>(center.x);
        y = static_cast<float>(center.y - static_cast<double>(font_size));
        z = static_cast<float>(0.f);///object->getZOrder());
        half_width =  font_size / 2.0f;
        half_height = font_size / 2.0f;

        DrVec3 top_right = DrVec3( half_width,  half_height, 0.f);
        DrVec3 top_left =  DrVec3(-half_width,  half_height, 0.f);
        DrVec3 bot_right = DrVec3( half_width, -half_height, 0.f);
        DrVec3 bot_left =  DrVec3(-half_width, -half_height, 0.f);

        // ***** Create Texture Coordinate and Vertex arrays
        std::vector<float> texture_coordinates;
        std::vector<GLfloat> vertices;
        texture_coordinates.clear();
        vertices.clear();
        texture_coordinates.resize( static_cast<std::size_t>(8 * hp.length()) );
        vertices.resize( 12 * hp.length() );

        // Figure out what color to make the health, get health as string
        DrColor color = objectDebugColor(object->getCollisionType(), cpBodyIsSleeping(object->body));

        // ***** Set Shader Variables
        setShaderDefaultValues(static_cast<float>(texture->width()), static_cast<float>(texture->height()));
        float r = static_cast<float>(color.redF());
        float g = static_cast<float>(color.greenF());
        float b = static_cast<float>(color.blueF());
        m_default_shader.setUniformValue( u_default_tint, r, g, b );

        // ***** Render each number as a textured quad
        x -= ((hp.length() - 1.0f) * (half_width * spacing));
        for (size_t i = 0; i < hp.length(); i++) {

            // Set Texture Coordinates
            setNumberTextureCoordinates(hp.substr(i, 1), texture_coordinates);

            // Set Vertices
            setQuadRotatedVertices(vertices, top_right, top_left, bot_left, bot_right, DrVec3(x, y, z));

            x += (half_width * 2.0f * spacing);

            m_default_shader.setAttributeArray( a_default_texture_coord, texture_coordinates.data(), 2 );
            m_default_shader.setAttributeArray( a_default_vertex, vertices.data(), 3 );
            m_default_shader.enableAttributeArray( a_default_texture_coord );
            m_default_shader.enableAttributeArray( a_default_vertex );

            // ***** Draw triangles using Shader
            glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

            // Release bound items
            m_default_shader.disableAttributeArray( a_default_texture_coord );
            m_default_shader.disableAttributeArray( a_default_vertex );
        }
    }

    m_default_shader.release();
    ///painter.endNativePainting();
}


void DrOpenGL::setNumberTextureCoordinates(std::string letter, std::vector<float> &c) {
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















