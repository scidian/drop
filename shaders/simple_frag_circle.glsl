// version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Simple Fragment Shader - Draw Circle
//##
//####################################################################################

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform mediump     vec3    u_color;                // Color of circle
uniform highp       float   u_alpha;                // Texture opacity


//####################################################################################
//##    Main Shader Function
//####################################################################################
void main( void ) {

    vec2  position = vec2(0.5, 0.5);                                    // Draw circle in middle of quad
    float radius = 0.5;                                                 // Fill the quad with the circle

    float  distance = distance(coordinates.st, position);
    if (distance > radius) discard;

    // ***** Final Output
    gl_FragColor = vec4(u_color, u_alpha);

}











