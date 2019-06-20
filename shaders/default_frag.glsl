#ifdef GL_ES
precision mediump float;
#endif
//
//
//  Fragment Shader
//
//

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_tex;                          // Texture
uniform lowp float  u_alpha;                        // Opacity

uniform lowp float  u_width;                        // Width of gl window
uniform lowp float  u_height;                       // Height of gl window

// EXACT SAME ORDER AS IN EDITOR!!!!!
uniform lowp float  u_pixel_x;// = 1.0;             // Pixel Width X    1.0 Normal, 4.0 is nice pixelation
uniform lowp float  u_pixel_y;// = 1.0;             // Pixel Width Y    1.0 Normal, 4.0 is nice pixelation
uniform      bool   u_negative;// = false;          // Negative         True / False
uniform      bool   u_grayscale;// = false;         // Grayscale        True / False
uniform lowp float  u_hue;// = 0.0;                 // Hue              Editor:    0 to 360     Shader:  0.0 to 1.0
uniform lowp float  u_saturation;// = 0.0;          // Saturation       Editor: -255 to 255     Shader: -1.0 to 1.0
uniform lowp float  u_contrast;// = 0.0;            // Contrast         Editor: -255 to 255     Shader: -1.0 to 1.0
uniform lowp float  u_brightness;// = 0.0;          // Brightness       Editor: -255 to 255     Shader: -1.0 to 1.0

uniform lowp float  u_bitrate;// = 256;             // Bitrate          Editor:    1 to  16
uniform lowp vec3   u_tint;// = vec3(0, 0, 0);      // Tint, adds rgb to final output
uniform      bool   u_kernel;// = false;            // Kernel Effects? (blur, sharpen, etc)


// ***** Convert red/green/blue to hue/saturation/vibrance
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// ***** Convert hue/saturation/vibrance to red/green/blue
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


void main( void ) {

    // ***** PIXELATED
    vec4 texture_color;
    if (u_pixel_x > 1.0 || u_pixel_y > 1.0) {       
        float dx = u_pixel_x * 0.99 * (1.0 / u_width);          // 99 Percent is slight modifier found to be more like the function in Image_Filter_Color.cpp
        float dy = u_pixel_y * 0.99 * (1.0 / u_height);         // 99 Percent is slight modifier found to be more like the function in Image_Filter_Color.cpp
        vec2 coord = coordinates.st;
             coord = vec2(dx * floor(coord.x / dx) + (dx / 2.0), dy * floor(coord.y / dy) + (dy / 2.0));
        texture_color = texture2D(u_tex, coord).rgba;
    } else {
        texture_color = texture2D(u_tex, coordinates.st).rgba;                      // If not pixelated, grab initial texture color at the current location
    }

    // Set some variables for use later
    vec4 alpha_in = vec4(u_alpha, u_alpha, u_alpha, u_alpha);                       // For adding in existing opacity of object
    vec3 fragRGB = texture_color.rgb;                                               // Save rgb as a vec3 for working with

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
        fragHSV.x =  mod(fragHSV.x, 1.0);
        fragHSV.y += u_saturation;
        fragHSV.y =  clamp(fragHSV.y, 0.0, 1.0);
        fragRGB = hsv2rgb(fragHSV);
    }

    // ***** CONTRAST / BRIGHTNESS ADJUSTMENT
    fragRGB.rgb =  ((fragRGB.rgb - 0.5) * (u_contrast + 0.392) / 0.392) + 0.5;      // Contrast
    fragRGB.rgb += u_brightness;                                                    // Brightness
    fragRGB.rgb += u_tint;                                                          // Tint
    fragRGB.rgb =  clamp(fragRGB.rgb, 0.0, 1.0);

    // ***** BITRATE ADJUSTMENT (16 bit down to 1 bit)
    if (u_bitrate < 16.0) {
        float bit_depth = pow(2.0, u_bitrate);
        fragRGB = vec3(floor(fragRGB.r * bit_depth), floor(fragRGB.g * bit_depth), floor(fragRGB.b * bit_depth)) / bit_depth;
    }


    gl_FragColor = vec4(fragRGB, texture_color.a) * alpha_in;


    // ******************** KERNEL EFFECTS
    if (u_kernel) {
        float offset_x = 1.0 / u_width;
        float offset_y = 1.0 / u_height;
        vec2 offset_0 = vec2(-offset_x,  offset_y);     // top-left
        vec2 offset_1 = vec2( 0.0,       offset_y);     // top-center
        vec2 offset_2 = vec2( offset_x,  offset_y);     // top-right
        vec2 offset_3 = vec2(-offset_x,       0.0);     // center-left
        vec2 offset_4 = vec2( 0.0,            0.0);     // center-center
        vec2 offset_5 = vec2( offset_x,       0.0);     // center-right
        vec2 offset_6 = vec2(-offset_x, -offset_y);     // bottom-left
        vec2 offset_7 = vec2( 0.0,      -offset_y);     // bottom-center
        vec2 offset_8 = vec2( offset_x, -offset_y);     // bottom-right

        // ***** SHARPEN
        //float kernel_0 = -1.0; float kernel_1 = -1.0; float kernel_2 = -1.0;
        //float kernel_3 = -1.0; float kernel_4 =  9.0; float kernel_5 = -1.0;
        //float kernel_6 = -1.0; float kernel_7 = -1.0; float kernel_8 = -1.0;

        // ***** BLUR
        float kernel_0 = 1.0 / 16.0; float kernel_1 = 2.0 / 16.0; float kernel_2 = 1.0 / 16.0;
        float kernel_3 = 2.0 / 16.0; float kernel_4 = 4.0 / 16.0; float kernel_5 = 2.0 / 16.0;
        float kernel_6 = 1.0 / 16.0; float kernel_7 = 2.0 / 16.0; float kernel_8 = 1.0 / 16.0;

        // ***** EDGE DETECTION
        //float kernel_0 = 1.0; float kernel_1 =  1.00; float kernel_2 = 1.0;
        //float kernel_3 = 1.0; float kernel_4 = -8.00; float kernel_5 = 1.0;
        //float kernel_6 = 1.0; float kernel_7 =  1.00; float kernel_8 = 1.0;

        vec4 sample_tex_0 = texture2D(u_tex, coordinates.st + offset_0).rgba;
        vec4 sample_tex_1 = texture2D(u_tex, coordinates.st + offset_1).rgba;
        vec4 sample_tex_2 = texture2D(u_tex, coordinates.st + offset_2).rgba;
        vec4 sample_tex_3 = texture2D(u_tex, coordinates.st + offset_3).rgba;
        vec4 sample_tex_4 = texture2D(u_tex, coordinates.st + offset_4).rgba;
        vec4 sample_tex_5 = texture2D(u_tex, coordinates.st + offset_5).rgba;
        vec4 sample_tex_6 = texture2D(u_tex, coordinates.st + offset_6).rgba;
        vec4 sample_tex_7 = texture2D(u_tex, coordinates.st + offset_7).rgba;
        vec4 sample_tex_8 = texture2D(u_tex, coordinates.st + offset_8).rgba;

        vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
        col += sample_tex_0 * kernel_0;
        col += sample_tex_1 * kernel_1;
        col += sample_tex_2 * kernel_2;
        col += sample_tex_3 * kernel_3;
        col += sample_tex_4 * kernel_4;
        col += sample_tex_5 * kernel_5;
        col += sample_tex_6 * kernel_6;
        col += sample_tex_7 * kernel_7;
        col += sample_tex_8 * kernel_8;

        gl_FragColor = vec4(col.rgb, texture2D( u_tex, coordinates.st ).a * u_alpha);
    }

}


















