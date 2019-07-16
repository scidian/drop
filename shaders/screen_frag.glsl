#version 120
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

uniform lowp float  u_width;                        // Width of gl window
uniform lowp float  u_height;                       // Height of gl window


//####################################################################################
//##        Blends two colors with "Hard Light" blending
//####################################################################################
vec3 hardLightBlending(vec3 upper, vec3 lower) {
    vec3 out_color;
    if (upper.r > 0.5) {
        out_color.r = 1.0 - ( (1.0 - lower.r) * (1.0 - (2.0 * (upper.r - 0.5))) );
    } else {
        out_color.r = lower.r * (2.0 * upper.r);
    }
    if (upper.g > 0.5) {
        out_color.g = 1.0 - ( (1.0 - lower.g) * (1.0 - (2.0 * (upper.g - 0.5))) );
    } else {
        out_color.g = lower.g * (2.0 * upper.g);
    }
    if (upper.b > 0.5) {
        out_color.b = 1.0 - ( (1.0 - lower.b) * (1.0 - (2.0 * (upper.b - 0.5))) );
    } else {
        out_color.b = lower.b * (2.0 * upper.b);
    }
    return out_color;
}


//####################################################################################
//##        Main Shader Function
//####################################################################################
void main( void ) {

    vec4 upper = texture2D(u_upper, coordinates.st);            // Lights
    vec4 lower = texture2D(u_lower, coordinates.st);            // Ground
    vec3 out_color;


    //out_color = lower.rgb * (upper.rgb * 1.25);               // Multiply Blending (can change multiplier)


    out_color = hardLightBlending(upper.rgb, lower.rgb);        // Hard Light Blending
    out_color *= lower.rgb;                                     // Extra light added in for reveresed mode


    gl_FragColor = vec4(out_color, 1.0);

}


















