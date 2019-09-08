#version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Main Vertex Shader
//##
//####################################################################################


// ***** Input from Engine
attribute highp     vec4 vertex;                    // Input
attribute highp     vec3 normal;                    // Input
attribute highp     vec2 texture_coordinates;       // Input
uniform   highp     mat4 u_matrix;                  // Input
uniform   mediump   mat4 u_matrix_object;           // Input


// ***** Output to Fragment Shader
varying   highp     vec2 coordinates;               // Output
varying   mediump   vec3 vert;                      // Output
varying   mediump   vec3 vert_normal;               // Output


//####################################################################################
//##    Main Shader Function
//####################################################################################
void main( void ) {

    // Adjust triangle through camera matrix
    gl_Position = u_matrix * vertex;

    // Send texture coordinate to fragment shader
    coordinates = texture_coordinates;

    // Adjust object vertex to proper rotation / scale / position
    vec4 object_vert = u_matrix_object * vertex;
    vert = object_vert.xyz;

    // Adjust object face normal to proper rotation / scale / position
    vec4 normal_vert = u_matrix_object * vec4(normal, 0.0);
    vert_normal = normal_vert.xyz;

}










