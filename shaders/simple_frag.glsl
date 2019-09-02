#version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Simple Fragment Shader
//##
//####################################################################################

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_texture;                      // Texture
uniform highp float u_alpha;                        // Texture opacity
uniform highp float u_time;                         // Time in seconds
uniform highp vec2  u_pos;                          // Position in world
uniform lowp  float u_width;                        // Texture Width
uniform lowp  float u_height;                       // Texture Height



//####################################################################################
//##        Main Shader Function
//####################################################################################
void main( void ) {

    // ***** Grab coordinates into an editable vec2
    vec2 coords = coordinates;



    // ***** Final Output
    gl_FragColor = texture2D(u_texture, coords) * vec4(u_alpha, u_alpha, u_alpha, u_alpha);
    if (gl_FragColor.a < 0.05) discard;
}











