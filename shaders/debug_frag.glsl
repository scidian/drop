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
varying             vec2    coordinates; //uv;
varying             vec4    color_fill;
varying             vec4    color_border;


//####################################################################################
//##    Main Shader Function
//####################################################################################
void main( void ) {

    float len =  length(coordinates);
    float fw =   length(fwidth(coordinates));//* 2.0;       // Multiplier changes border width
    float mask = smoothstep(-1.0, fw - 1.0, -len);

    float outline =         1.0 - fw;
    float outline_mask =    smoothstep(outline - fw, outline, len);

    // To fade into border:
    //float outline_mask =  smoothstep(outline - (fw * 20.0), outline + (fw * 20.0), len);

    vec4  color =   color_fill + ((color_border - color_fill*color_border.a) * outline_mask);
    gl_FragColor =  color * mask;

}






