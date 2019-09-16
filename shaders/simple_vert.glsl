// version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Simple Vertex Shader
//##
//####################################################################################


// ***** Input from Engine
attribute highp     vec4 vertex;                    // Input
attribute highp     vec2 texture_coordinates;       // Input
uniform   highp     mat4 u_matrix;                  // Input

// ***** Output to Fragment Shader
varying   highp     vec2 coordinates;               // Output


//####################################################################################
//##    Main Shader Function
//####################################################################################
void main( void ) {

    // Adjust triangle through camera matrix
    gl_Position = u_matrix * vertex;

    // Send texture coordinate to fragment shader
    coordinates = texture_coordinates;

}










