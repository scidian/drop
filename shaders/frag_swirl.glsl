#version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Swirl Fragment Shader
//##
//####################################################################################

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_texture;                      // Texture

uniform lowp  float u_alpha;                        // Opacity
uniform lowp  float u_zoom;                         // Current zoom level
uniform lowp  vec3  u_position;                     // Current camera position
uniform highp float u_width;                        // Texture Width
uniform highp float u_height;                       // Texture Height
uniform highp float u_time;                         // Time in seconds
uniform lowp  float u_angle;                        // Angle of Swirl in degrees

uniform highp float u_top;                          // Top of Swirl,        from 0.0 to 1.0 in screen coordinates
uniform highp float u_bottom;                       // Bottom of Swirl,     from 0.0 to 1.0 in screen coordinates
uniform highp float u_left;                         // Left side of Swirl,  from 0.0 to 1.0 in screen coordinates
uniform highp float u_right;                        // Right side of Swirl, from 0.0 to 1.0 in screen coordinates

uniform lowp  vec3  u_start_color;                  // Start Color, r/g/b                0.0 to 1.0 x 3
uniform lowp  float u_color_tint;                   // Color Tint Percent
uniform lowp  float u_rotation;
uniform lowp  float u_radius;                       // Radius of swirl

uniform highp float u_pixel_x;// = 1.0;             // Pixel Width X    1.0 Normal, 4.0 is nice pixelation
uniform highp float u_pixel_y;// = 1.0;             // Pixel Width Y    1.0 Normal, 4.0 is nice pixelation
uniform lowp  float u_bitrate;// = 256;             // Bitrate          Editor:    1 to  256


// Constants
const   lowp  float THRESHOLD = 0.75;                       // Alpha threshold for our occlusion map
const   highp float PI =  3.14159;                          // Pi
const   highp float RAD = 6.2831853;                        // 2.0 * PI is 360 degrees in radians
const   highp float PI180 = highp float(PI / 180.0);        // To convert Degrees to Radians


//####################################################################################
//##        2D Rotation / Translation
//####################################################################################
vec2 translate(vec2 v, float x, float y) {
    v.x = v.x + x;
    v.y = v.y + y;
    return v;
}

vec2 rotate(vec2 v, vec2 center_point, float angle) {
    v = translate(v, -(center_point.x), -(center_point.y));         // Translate to center point
    v.x *= (u_width / u_height);                                    // Account for texture ratio
    float a = angle * PI180;                                        // Convert to radians
    float s = sin(a);
    float c = cos(a);

    mat2 m = mat2(c, s, -s, c);
    v = m * v;

    v.x /= (u_width / u_height);                                    // Account for texture ratio
    v = translate(v,  (center_point.x),  (center_point.y));         // Remove center translation
    return v;
}


//####################################################################################
//##        Main Shader Function
//####################################################################################
void main( void ) {

    // ***** Swirl Input Variables
    float time = u_time;


    // ***** Move coordinates into a vec2 that is not read-only
    highp vec2 coords = coordinates.xy;

    // ***** Pixelation
    if (u_pixel_x > 1.0 || u_pixel_y > 1.0) {
        highp float pixel_width =  (1.0 / (u_width));
        highp float pixel_height = (1.0 / (u_height));
        highp float real_pixel_x = ((coords.x / 1.0) * u_width);
        highp float real_pixel_y = (((1.0 - coords.y) / 1.0) * u_height);
        highp float pixel_x =       u_pixel_x * floor(real_pixel_x / u_pixel_x) * pixel_width;
        highp float pixel_y = 1.0 - u_pixel_y * floor(real_pixel_y / u_pixel_y) * pixel_height;
        coords = vec2(pixel_x, pixel_y);
    }

    // ***** Apply rotation
    highp vec2  swirl_center = vec2(u_right - (u_right - u_left)/2.0, u_top - (u_top - u_bottom)/2.0);
    coords = rotate(coords, swirl_center, u_angle);

//    // ***** If out of range, don't process this pixel
//    if (coords.y > u_top || coords.y < u_bottom || coords.x < u_left || coords.x > u_right) {
//        discard;
//    }


    // ***** Draw Swirl Effect
    vec2 tex_size = vec2(u_width, u_height);

    float radius = (u_radius / 2.0) * u_zoom;
    float angle = u_rotation / 100.0;      // 5.0 is about as high as youd want to go?


    vec2 center = swirl_center * tex_size;
    vec2 tc = coords * tex_size;
    tc -= center;
    float dist = length(tc);
    if (dist > radius) discard;

    float percent = (radius - dist) / radius;
    float theta = percent * percent * angle * 8.0;
    float s = sin(theta);
    float c = cos(theta);
    tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));

    tc += center;
    tc /= tex_size;
    vec3 color = texture2D(u_texture, tc).rgb;
    vec4 original = vec4(color, u_alpha);


    // ***** Mix in overlay_color
    original.rgb = mix(original.rgb, u_start_color, u_color_tint * (1.0 - (dist / radius)*0.75));

    // ***** Bit Depth (0.0 to 256.0)
    highp float bit_depth = u_bitrate;
    original.rgb = vec3(floor(original.r * bit_depth), floor(original.g * bit_depth), floor(original.b * bit_depth)) / bit_depth;


    gl_FragColor = original;
}













