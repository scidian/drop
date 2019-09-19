// version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Main Vertex Shader
//##
//####################################################################################


// ***** Input from Engine
attribute   highp   vec4 vertex;                    // Input
attribute   highp   vec3 normal;                    // Input
attribute   highp   vec2 texture_coordinates;       // Input
attribute   highp   vec3 barycentric;               // Input

uniform     highp   mat4 u_matrix;                  // Input
uniform     highp   mat4 u_matrix_object;           // Input


// ***** Output to Fragment Shader
varying     highp   vec2 coordinates;               // Output
varying     highp   vec3 vert;                      // Output
varying     highp   vec3 vert_normal;               // Output
varying     highp   vec3 vert_bary;                 // Output


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
    vec4 normal_vert = vec4(normal, 0.0);                                   // !!!!! w MUST be 0.0
         normal_vert = u_matrix_object * normal_vert;
    vert_normal = normal_vert.xyz;

    vert_bary = barycentric;
}










