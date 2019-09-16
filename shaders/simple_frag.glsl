// version 120
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
//##    Main Shader Function
//####################################################################################
void main( void ) {

    // ***** Grab coordinates into an editable vec2
    vec2 coords = coordinates;
    vec4 texture_color = texture2D(u_texture, coords.st).rgba;

    highp vec4 alpha_in = vec4(u_alpha, u_alpha, u_alpha, u_alpha);                 // For adding in existing opacity of object
    highp vec3 frag_rgb = texture_color.rgb;                                        // Save rgb as a vec3 for working with

    // ***** Don't draw fragment to depth buffer if mostly invisible
    if (texture_color.a < 0.05) discard;


    // ***** Final Output
    gl_FragColor = vec4(frag_rgb, texture_color.a) * alpha_in;
}











