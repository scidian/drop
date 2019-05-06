//
//
//
//
//
uniform sampler2D  tex;     // Input
varying highp vec4 texc;    // Input from Vertex Shader

void main( void )
{
    // Divide out this pre-multiplied alpha
    //vec4 ts = texture2D( tex, texc.st ).rgba;
    //vec3 outColor = ts.rgb / ts.a;
    //gl_FragColor = vec4(outColor, ts.a);

    // Grab color straight from texture wothout doing anything
    gl_FragColor = texture2D( tex, texc.st ).rgba;

}
