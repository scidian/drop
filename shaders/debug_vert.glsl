// version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Debug Vertex Shader
//##
//####################################################################################

// ***** Input from Engine
attribute           vec2    a_pos;
attribute           vec2    a_uv;
attribute           float   a_radius;

uniform     highp   mat4    u_matrix;


// ***** Output to Fragment Shader
varying             vec2    coordinates;    //uv;



//####################################################################################
//##    Main Shader Function
//####################################################################################
void main( void ) {
    gl_Position = u_matrix * vec4(a_pos + a_radius*a_uv, 0.0, 1.0);
    coordinates = a_uv;
}














