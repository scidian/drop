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


//####################################################################################
//##    Main Shader Function
//####################################################################################
void main( void ) {

    // ***** Don't draw fragment to depth buffer if mostly invisible
    vec4 texture_color = texture2D(u_texture, coordinates.st).rgba;
    if (texture_color.a < 0.05) discard;

    // ***** Final Output
    gl_FragColor = texture_color * vec4(u_alpha);

}











