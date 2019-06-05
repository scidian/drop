#ifdef GL_ES
precision mediump float;
#endif
//
//
//  Fragment Shader
//
//

// ***** Input from Vertex Shader
varying highp vec4  coordinates;                    // Texture Coodinates


// ***** Input from Engine
uniform sampler2D   u_tex;                          // Texture
uniform lowp float  u_alpha;                        // Opacity

// EXACT SAME ORDER AS IN EDITOR!!!!!
uniform      bool   u_negative;// = false;          // Negative         True / False
uniform      bool   u_grayscale;// = false;         // Grayscale        True / False
uniform lowp float  u_hue;// = 0.0;                 // Hue              Editor:    0 to 360     Shader:  0.0 to 1.0
uniform lowp float  u_saturation;// = 0.0;          // Saturation       Editor: -255 to 255     Shader: -1.0 to 1.0
uniform lowp float  u_contrast;// = 0.0;            // Contrast         Editor: -255 to 255     Shader: -1.0 to 1.0
uniform lowp float  u_brightness;// = 0.0;          // Brightness       Editor: -255 to 255     Shader: -1.0 to 1.0


// ***** Convert red/green/blue to hue/saturation/value
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// ***** Convert hue/saturation/value to red/green/blue
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


void main( void ) {

    vec4 texture_color = texture2D(u_tex, coordinates.st).rgba;                     // Grab initial texture color at the current location
    vec4 alpha_in = vec4(u_alpha, u_alpha, u_alpha, u_alpha);                       // For adding in existing opacity of object
    vec3 fragRGB = texture_color.rgb;                                               // Save rgb as a vec3 for working with

    // ********** NORMAL: No post processing, just get texture color and multiply in alpha channel (from editor) for semi-transparent objects
    //gl_FragColor = texture2D( u_tex, coordinates.st ).rgba * alpha_in;
    // ***** RED: Makes everything green
    //gl_FragColor = vec4(1,0,0,1) * texture2D( u_tex, coordinates.st ).rgba * alpha_in;
    // ***** GREEN: Makes everything green
    //gl_FragColor = vec4(0,1,0,1) * texture2D( u_tex, coordinates.st ).rgba * alpha_in;

    // ***** NEGATIVE
    if (u_negative) fragRGB = 1.0 - fragRGB;

    // ***** GRAYSCALE
    if (u_grayscale) {
        float average = 0.2126 * fragRGB.r + 0.7152 * fragRGB.g + 0.0722 * fragRGB.b;
        fragRGB = vec3(average, average, average);
    }

    // ***** HUE / SATURATION ADJUSTMENT
    if (u_hue > 0.0 || u_saturation > 0.0) {
        vec3 fragHSV = rgb2hsv(fragRGB).xyz;
        fragHSV.x += u_hue;
        fragHSV.y += u_saturation;
        fragHSV.x =  mod(fragHSV.x, 1.0);
        fragHSV.yz = clamp(fragHSV.yz, 0.0, 1.0);
        fragRGB = hsv2rgb(fragHSV);
    }

    // ***** CONTRAST / BRIGHTNESS ADJUSTMENT
    fragRGB.rgb =  ((fragRGB.rgb - 0.5) * (u_contrast + 0.392) / 0.392) + 0.5;      // Contrast
    fragRGB.rgb += u_brightness;                                                    // Brightness
    fragRGB.rgb =  clamp(fragRGB.rgb, 0.0, 1.0);

    gl_FragColor = vec4(fragRGB, texture_color.a) * alpha_in;



//    // ******************** KERNEL EFFECTS
//    const float offset = 1.0 / 300.0;
//    vec2 offset_0 = vec2(-offset,  offset);     // top-left
//    vec2 offset_1 = vec2( 0.0,     offset);     // top-center
//    vec2 offset_2 = vec2( offset,  offset);     // top-right
//    vec2 offset_3 = vec2(-offset,  0.0);        // center-left
//    vec2 offset_4 = vec2( 0.0,     0.0);        // center-center
//    vec2 offset_5 = vec2( offset,  0.0);        // center-right
//    vec2 offset_6 = vec2(-offset, -offset);     // bottom-left
//    vec2 offset_7 = vec2( 0.0,    -offset);     // bottom-center
//    vec2 offset_8 = vec2( offset, -offset);     // bottom-right

//    // ***** SHARPEN
//    //float kernel_0 = -1.0; float kernel_1 = -1.0; float kernel_2 = -1.0;
//    //float kernel_3 = -1.0; float kernel_4 =  9.0; float kernel_5 = -1.0;
//    //float kernel_6 = -1.0; float kernel_7 = -1.0; float kernel_8 = -1.0;

//    // ***** BLUR
//    //float kernel_0 = 1.0 / 16.0; float kernel_1 = 2.0 / 16.0; float kernel_2 = 1.0 / 16.0;
//    //float kernel_3 = 2.0 / 16.0; float kernel_4 = 4.0 / 16.0; float kernel_5 = 2.0 / 16.0;
//    //float kernel_6 = 1.0 / 16.0; float kernel_7 = 2.0 / 16.0; float kernel_8 = 1.0 / 16.0;

//    // ***** EDGE DETECTION
//    float kernel_0 = 1.0; float kernel_1 =  1.00; float kernel_2 = 1.0;
//    float kernel_3 = 1.0; float kernel_4 = -8.00; float kernel_5 = 1.0;
//    float kernel_6 = 1.0; float kernel_7 =  1.00; float kernel_8 = 1.0;

//    vec3 sample_tex_0 = texture2D(u_tex, coordinates.st + offset_0).rgb;
//    vec3 sample_tex_1 = texture2D(u_tex, coordinates.st + offset_1).rgb;
//    vec3 sample_tex_2 = texture2D(u_tex, coordinates.st + offset_2).rgb;
//    vec3 sample_tex_3 = texture2D(u_tex, coordinates.st + offset_3).rgb;
//    vec3 sample_tex_4 = texture2D(u_tex, coordinates.st + offset_4).rgb;
//    vec3 sample_tex_5 = texture2D(u_tex, coordinates.st + offset_5).rgb;
//    vec3 sample_tex_6 = texture2D(u_tex, coordinates.st + offset_6).rgb;
//    vec3 sample_tex_7 = texture2D(u_tex, coordinates.st + offset_7).rgb;
//    vec3 sample_tex_8 = texture2D(u_tex, coordinates.st + offset_8).rgb;

//    vec3 col = vec3(0.0, 0.0, 0.0);
//    col += sample_tex_0 * kernel_0;
//    col += sample_tex_1 * kernel_1;
//    col += sample_tex_2 * kernel_2;
//    col += sample_tex_3 * kernel_3;
//    col += sample_tex_4 * kernel_4;
//    col += sample_tex_5 * kernel_5;
//    col += sample_tex_6 * kernel_6;
//    col += sample_tex_7 * kernel_7;
//    col += sample_tex_8 * kernel_8;

//    gl_FragColor = vec4(col, texture2D( u_tex, coordinates.st ).a * u_alpha);


}

















