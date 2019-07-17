#version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Kernel Effects Fragment Shader
//##
//####################################################################################

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_texture;                      // Texture
uniform lowp  float u_alpha;                        // Opacity

uniform highp float u_width;                        // Texture Width
uniform highp float u_height;                       // Texture Height


//####################################################################################
//##        Main Shader Function
//####################################################################################
void main( void ) {

    // Grab coordinates into a vec2 that is not read-only
    highp vec2 coords = coordinates.st;

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

    vec4 sample_tex_0 = texture2D(u_texture, coords.xy + offset_0).rgba;
    vec4 sample_tex_1 = texture2D(u_texture, coords.xy + offset_1).rgba;
    vec4 sample_tex_2 = texture2D(u_texture, coords.xy + offset_2).rgba;
    vec4 sample_tex_3 = texture2D(u_texture, coords.xy + offset_3).rgba;
    vec4 sample_tex_4 = texture2D(u_texture, coords.xy + offset_4).rgba;
    vec4 sample_tex_5 = texture2D(u_texture, coords.xy + offset_5).rgba;
    vec4 sample_tex_6 = texture2D(u_texture, coords.xy + offset_6).rgba;
    vec4 sample_tex_7 = texture2D(u_texture, coords.xy + offset_7).rgba;
    vec4 sample_tex_8 = texture2D(u_texture, coords.xy + offset_8).rgba;

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

    gl_FragColor = vec4(col.rgb, texture2D(u_texture, coords.st).a * u_alpha);

}












