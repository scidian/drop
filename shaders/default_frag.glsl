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
    // Nothing
    //gl_FragColor = texture2D( tex, texc.st ).rgba;


    // Divide out this pre-multiplied alpha
    //vec4 ts = texture2D( tex, texc.st ).rgba;
    //vec4 outColor = ts.rgba / ts.a;
    //gl_FragColor = vec4(ts.rgb, alpha);


    // Makes everything green
    //gl_FragColor = vec4(0,1,0,1) * vec4(texture2D(tex,texc.st).xyz, texture2D(tex,texc.st).w);


    // Grab color straight from texture, Multiply in alpha channel (from editor) for semi-transparent objects
    vec4 texture_color = texture2D( tex, texc.st ).rgba;
    gl_FragColor = texture_color * vec4(alpha, alpha, alpha, alpha);

}

