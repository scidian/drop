// version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    "Screen" Light Shader
//##
//####################################################################################

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_upper;                        // Texture (Lights)
uniform sampler2D   u_lower;                        // Texture (Ground)
uniform int         u_blend_mode;                   // Blend mode

uniform lowp float  u_width;                        // Width of gl window
uniform lowp float  u_height;                       // Height of gl window


//####################################################################################
//##    Blends two colors with "Hard Light" blending
//####################################################################################
vec3 hardLightBlending(vec3 upper, vec3 lower) {
    vec3 out_color;
    if (upper.r > 0.5) {
        out_color.r = 1.0 - ((1.0 - lower.r) * (1.0 - (2.0 * (upper.r - 0.5))));
    } else {
        out_color.r = 2.0 * upper.r * lower.r;
    }
    if (upper.g > 0.5) {
        out_color.g = 1.0 - ((1.0 - lower.g) * (1.0 - (2.0 * (upper.g - 0.5))));
    } else {
        out_color.g = 2.0 * upper.g * lower.g;
    }
    if (upper.b > 0.5) {
        out_color.b = 1.0 - ((1.0 - lower.b) * (1.0 - (2.0 * (upper.b - 0.5))));
    } else {
        out_color.b = 2.0 * upper.b * lower.b;
    }
    return out_color;
}


//####################################################################################
//##    Main Shader Function
//####################################################################################
void main( void ) {

    // enum class Blend_Mode
    //  Default     = 0,
    //  Multiply    = 1,
    //  Overlay     = 2
    //  Hard_Light  = 3,

    // ????? Can also do this to get last drawn pixel color instead of copying last frame buffer:
    //
    // lowp vec4 last_color = gl_LastFragData[0];
    //
    // ?????

    vec4 upper = texture2D(u_upper, coordinates.st);            // Lights
    vec4 lower = texture2D(u_lower, coordinates.st);            // Ground
    vec3 out_color;


    // Multiply Blending (can change multiplier)
    if (u_blend_mode == 1) {
        out_color = lower.rgb * upper.rgb * 1.5;

    // Screen Blending
    //} else if (u_blend_mode == ?) {
    //    out_color = 1.0 - ((1.0 - lower.rgb) * (1.0 - (2.0 * (upper.rgb - 0.5))));

    // Overlay Blending
    } else if (u_blend_mode == 2) {
        out_color = hardLightBlending(upper.rgb, lower.rgb);
        //out_color *= lower.rgb;

    // Hard Light Blending, extra light added in for reveresed mode
    } else if (u_blend_mode == 3) {
        out_color = hardLightBlending(lower.rgb, upper.rgb);
        out_color *= lower.rgb;
    }


    gl_FragColor = vec4(out_color, 1.0);
}












