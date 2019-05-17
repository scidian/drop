//
//
//
//
//
uniform float alpha;            // Input
uniform sampler2D  tex;         // Input
varying highp vec4 texc;        // Input from Vertex Shader

void main( void )
{
    // Divide out this pre-multiplied alpha
    //vec4 ts = texture2D( tex, texc.st ).rgba;
    //vec3 outColor = ts.rgb / ts.a;
    //gl_FragColor = vec4(outColor, ts.a);

    // Grab color straight from texture wothout doing anything
    vec4 texture_color = texture2D( tex, texc.st ).rgba;

    // Multiply in alpha channel for semi-transparent objects
    gl_FragColor = texture_color * vec4(alpha, alpha, alpha, alpha);

}

