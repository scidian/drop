#version 120
#ifdef GL_ES
precision highp float;
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
uniform sampler2D   u_tex2;                         // Texture

uniform lowp float  u_width;                        // Width of gl window
uniform lowp float  u_height;                       // Height of gl window

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

    vec4 texture_color = texture2D(u_tex, coordinates.st).rgba;                     // If not pixelated, grab initial texture color at the current location


    gl_FragColor = texture_color;


    // ******************** KERNEL EFFECTS
    if (u_kernel) {
        // Temp can use to darken pixels
        //gl_FragColor = gl_FragColor * 0.3;
        //return;

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

        // ***** BLUR
        float kernel_0 = 1.0 / 16.0; float kernel_1 = 2.0 / 16.0; float kernel_2 = 1.0 / 16.0;
        float kernel_3 = 2.0 / 16.0; float kernel_4 = 4.0 / 16.0; float kernel_5 = 2.0 / 16.0;
        float kernel_6 = 1.0 / 16.0; float kernel_7 = 2.0 / 16.0; float kernel_8 = 1.0 / 16.0;

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

        gl_FragColor = vec4(col.rgb, texture2D( u_tex, coordinates.st ).a);
    }

    //    // Hard Light Blending
    //    vec4 upper = light_color;   //texture2D(texLmap, v_vTexcoord);
    //    vec4 lower = texture2D(u_base_texture, coordinates.st);
    //    vec4 out_color = vec4(0.0, 0.0, 0.0, upper.a);
    //    if (upper.r > 0.5) {
    //        out_color.r = (1.0 - (1.0-lower.r) * (1.0-2.0*(upper.r-0.5)));
    //    } else {
    //        out_color.r = lower.r * (2.0*upper.r);
    //    }
    //    if (upper.g > 0.5) {
    //        out_color.g = (1.0 - (1.0-lower.g) * (1.0-2.0*(upper.g-0.5)));
    //    } else {
    //        out_color.g = lower.g * (2.0*upper.g);
    //    }
    //    if (upper.b > 0.5) {
    //        out_color.b = (1.0 - (1.0-lower.b) * (1.0-2.0*(upper.b-0.5)));
    //    } else {
    //        out_color.b = lower.b * (2.0*upper.b);
    //    }
    //    if (upper.a > 0.5) {
    //        out_color.a = (1.0 - (1.0-lower.a) * (1.0-2.0*(upper.a-0.5)));
    //    } else {
    //        out_color.a = lower.a * (2.0*upper.a);
    //    }
    //    gl_FragColor = vec4(out_color.rgb, upper.a);


}


















