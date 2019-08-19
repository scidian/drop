#version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Mirror Fragment Shader
//##
//####################################################################################

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_texture;                      // Texture
uniform sampler2D   u_texture_noise;                // Noise Texture

uniform lowp  float u_alpha;                        // Opacity
uniform lowp  float u_zoom;                         // Current zoom level
uniform lowp  vec3  u_position;                     // Current camera position
uniform highp float u_width;                        // Texture Width
uniform highp float u_height;                       // Texture Height
uniform highp float u_time;                         // Time in seconds
uniform lowp  float u_angle;                        // Angle of Mirror in degrees

uniform lowp  float u_top;                          // Top of Mirror,        from 0.0 to 1.0 in screen coordinates
uniform lowp  float u_bottom;                       // Bottom of Mirror,     from 0.0 to 1.0 in screen coordinates
uniform lowp  float u_left;                         // Left side of Mirror,  from 0.0 to 1.0 in screen coordinates
uniform lowp  float u_right;                        // Right side of Mirror, from 0.0 to 1.0 in screen coordinates

uniform lowp  vec3  u_color_top;                    // Mirror Color, r/g/b      0.0 to 1.0 x 3
uniform lowp  vec3  u_color_bottom;                 // Mirror Color, r/g/b      0.0 to 1.0 x 3
uniform lowp  float u_color_tint;                   // Mirror Color Tint Percent
uniform lowp  float u_blur;
uniform lowp  float u_blur_stretch;


// Other Variables
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

    // ***** Mirror Input Variables
    vec3  start_color =         u_color_top;
    vec3  end_color =           u_color_bottom;
    float color_tint =          u_color_tint;
    float time = u_time;


    // ***** Move coordinates into a vec2 that is not read-only
    highp vec2 coords = coordinates.xy;

    // Apply rotation
    float rotation = u_angle;   // mod(u_time, 360.0) * 15.0; // <-- spins based on time
    vec2  mirror_center = vec2(u_right - (u_right - u_left)/2.0, u_top - (u_top - u_bottom)/2.0);
    coords = rotate(coords, mirror_center, rotation);


    // ***** Calculate some position and scaling values
    const float shrink_texture = 1.0;                                           // 1.0 = normal size, 4.0 equals 1/4 size
    float player_x = u_position.x*0.00083 * (1.0/u_zoom);
    float player_y = u_position.y*0.00083 * (1.0/u_zoom);
    float diff_w = (u_width  * (1.0/u_zoom)) / 1200.0;
    float diff_h = (u_height * (1.0/u_zoom)) / 1200.0;
    float zoom_coord_x = ((coords.x - 0.5)*diff_w)/u_zoom;
    float zoom_coord_y = ((coords.y - 0.5)*diff_h)/u_zoom;

    // ***** Grab value from 2D Noise Texture, use it to get refraction values
    vec3  displacement = texture2D(u_texture_noise, vec2( (zoom_coord_x + player_x) * u_zoom,
                                                         ((zoom_coord_y + player_y) * u_zoom) / shrink_texture)).rgb;
    float pixel_x = (1.0 / u_width)  * u_zoom;
    float pixel_y = (1.0 / u_height) * u_zoom;
    //float refract_x = (displacement.x - displacement.y) * 10.0 * pixel_x;
    //float refract_y = (displacement.y - displacement.x) * 10.0 * pixel_y;
    float refract_x = ((displacement.x * 2.0) - 1.0) * (u_blur + ((u_top - coords.y) * u_blur_stretch)) * pixel_x;
    float refract_y = ((displacement.y * 2.0) - 1.0) * (u_blur + ((u_top - coords.y) * u_blur_stretch)) * pixel_y;

    // ***** If out of range, don't process this pixel
    if (coords.y > u_top || coords.y < u_bottom || coords.x < u_left || coords.x > u_right) {
        discard;
    }


    // ***** Grab the reflected value from existing screen
    vec4 reflection, original;
    //reflection = texture2D(u_texture, vec2(coords.x, ((2.0 * y_start) - coords.y)));                                      // OPTIONAL: Simple reflection
    //reflection = texture2D(u_texture, vec2(              coords.x + refract_x*refract_reflection - xoffset,               // OPTIONAL: Original reflection
    //                                       2.0*y_start - coords.y - refract_y*refract_reflection + yoffset*bob) );
    vec2 get_reflection = vec2(coords.x + refract_x, u_top + (u_top - coords.y) + refract_y);
         get_reflection = clamp(get_reflection, (1.0 / u_width), 1.0 - (1.0 / u_height));
         get_reflection = rotate(get_reflection, mirror_center, -rotation);
    reflection = texture2D(u_texture, get_reflection);

    vec3 mirror_color = mix(start_color, end_color, smoothstep(0.0, 1.0, (coords.y - u_top) / (u_bottom - u_top)));
    reflection.rgb = mix(reflection.rgb, mirror_color, color_tint);


    // Existing pixel color through mirror
    vec2 get_under = vec2(coordinates.x, coordinates.y);
    original = texture2D(u_texture, get_under);
    original = mix(original,   reflection,     u_alpha);


    gl_FragColor = original;
}













