//
//
//
//
//
attribute highp   vec4 vertex;      // Input
attribute highp   vec4 texCoord;    // Input
uniform   mediump mat4 matrix;      // Input

varying   highp   vec4 texc;        // Output

void main( void )
{
    // Adjust traingle through camera matrix
    gl_Position = matrix * vertex;

    // Send texture coordinate to fragment shader
    texc =        texCoord;
}

