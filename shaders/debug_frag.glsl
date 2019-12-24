// version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Debug Fragment Shader
//##
//####################################################################################

// ***** Input from Vertex Shader
varying     vec2    coordinates; //uv;

// ***** Input from Engine
uniform mediump     vec4    u_fill;                 // Color of circle
uniform mediump     vec4    u_outline;              // Color of circle outline


//####################################################################################
//##    Main Shader Function
//####################################################################################
void main( void ) {
    float len =  length(coordinates);
    float fw =   length(fwidth(coordinates));
    float mask = smoothstep(-1.0, fw - 1.0, -len);

    float outline = 1.0 - fw;
    float outline_mask = smoothstep(outline - fw, outline, len);
    vec4  color = u_fill + ((u_outline - u_fill*u_outline.a) * outline_mask);
    gl_FragColor = color * mask;
}






