// version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Fisheye Fragment Shader
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
uniform lowp  float u_angle;                        // Angle in degrees

uniform lowp  float u_top;                          // Top of Lens,                 from 0.0 to 1.0 in screen coordinates
uniform lowp  float u_bottom;                       // Bottom of Lens,              from 0.0 to 1.0 in screen coordinates
uniform lowp  float u_left;                         // Left side of Lens,           from 0.0 to 1.0 in screen coordinates
uniform lowp  float u_right;                        // Right side of Lens,          from 0.0 to 1.0 in screen coordinates

uniform lowp  vec3  u_start_color;                  // Start Color, r/g/b                0.0 to 1.0 x 3
uniform lowp  float u_color_tint;                   // Color Tint Percent
uniform lowp  float u_lens_zoom;                    // Zoom level of Fisheye Lens        0.0 to 10.0

uniform highp float u_pixel_x;// = 1.0;             // Pixel Width X    1.0 Normal, 4.0 is nice pixelation
uniform highp float u_pixel_y;// = 1.0;             // Pixel Width Y    1.0 Normal, 4.0 is nice pixelation
uniform lowp  float u_bitrate;// = 256;             // Bitrate          Editor:    1 to  256


// Other Variables
const   lowp  float THRESHOLD = 0.75;               // Alpha threshold for our occlusion map
const   highp float PI =  3.14159;                  // Pi
const   highp float RAD = 6.2831853;                // 2.0 * PI is 360 degrees in radians
const   highp float PI180 = PI / 180.0;             // To convert Degrees to Radians

//####################################################################################
//##    2D Rotation / Translation
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
//##    Calculates angle from a center point to any target point, 0 = Up
//####################################################################################
highp float calculateRotationAngleInRadians(vec2 center_point, vec2 target_point) {
    // Calculate the angle theta from the deltaY and deltaX values (atan2 returns radians values from [-PI, PI])
    // 0 currently points EAST
    // #NOTE: By preserving Y and X param order to atan2, we are expecting a CLOCKWISE angle direction
    return atan(target_point.y - center_point.y, target_point.x - center_point.x);
}


//####################################################################################
//##    Pseudo-random number, that is between 0.0 and 0.999999 inclusive
//####################################################################################
float random (vec2 xy) {
    return fract(sin(dot(xy, vec2(11.2345, 81.456))) * 42758.12);
}


//####################################################################################
//##    Main Shader Function
//####################################################################################
void main( void ) {

    // ***** Fisheye Input Variables
    float time = u_time;
    float rotation = u_angle;   // mod(u_time, 360.0) * 15.0; // <-- spins based on time

    vec3  start_color =         u_start_color;
    float color_tint =          u_color_tint;
    float lens_zoom =           u_lens_zoom;


    // ***** Apply rotation. Move coordinates into a vec2 that is not read-only
    float lens_width = (u_right - u_left);
    float lens_height = (u_top - u_bottom);
    vec2  lens_center = vec2(u_right - lens_width/2.0, u_top - lens_height/2.0);
    vec2  coords = rotate(coordinates.xy, lens_center, rotation);

    // ***** Pixelation
    if (u_pixel_x > 1.0 || u_pixel_y > 1.0) {
        highp float dx = u_pixel_x * (1.0 / u_width);
        highp float dy = u_pixel_y * (1.0 / u_height);
        highp float pixel_x = dx * floor(coords.x / dx) + (dx / 2.0);
        highp float pixel_y = dy * floor(coords.y / dy) + (dy / 2.0);
        coords = vec2(pixel_x, pixel_y);
    }


    // ***** If out of range, don't process this pixel
    //if (coordinates.y > y_top || coordinates.y < u_bottom || coordinates.x < u_left || coordinates.x > u_right) {
    if (coords.y > u_top || coords.y < u_bottom || coords.x < u_left || coords.x > u_right) {
        discard;
    }


    // ***** FISHEYE
    vec2  dist =   coords.xy - lens_center;                                 // Distance vector of pixel from lens_center
    float radius = abs(length(dist));//sqrt(dot(dist, dist));               // Convert distance to float radius
    float theta =  calculateRotationAngleInRadians(lens_center, coords);    // Calculate angle of radius so we can figure out max ellipse radius at that angle

    // Calculates distance to border of ellipse at the angle of the current coordinate
    float a =  lens_width  / 2.0;
    float b =  lens_height / 2.0;
    float a2 = pow(a, 2.0);
    float b2 = pow(b, 2.0);
    float mx = abs( ((a * b)             ) / (sqrt(b2 + (a2 * pow(tan(theta), 2.0)))) );
    float my = abs( ((a * b) * tan(theta)) / (sqrt(b2 + (a2 * pow(tan(theta), 2.0)))) );

    if (theta > (PI/2.0) && theta < (RAD - PI/2.0)) {
        mx *= -1.0;
        my *= -1.0;
    }

    vec2  max_dist =  vec2(mx, my);
    float lens_size = abs(length(max_dist));//sqrt(dot(max_dist, max_dist));
    if (radius > lens_size) {
        discard;                                        // If outside of ellipse, don't process
        return;
    }

    // Choose one formula to uncomment:
    vec2  uv;
    uv = lens_center + dist * (radius * lens_zoom / u_zoom); // a.k.a. lens_center + normalize(dist) * radius * radius      // SQUARER
    //uv = lens_center + vec2(dist.x * abs(dist.x), dist.y * abs(dist.y));                                                  // SQUAREXY
    //uv = lens_center + normalize(dist) * sin(radius * 3.14159 * 0.5);                                                     // SINER
    //uv = lens_center + normalize(dist) * asin(radius) / (3.14159 * 0.5);                                                  // ASINR
    coords = rotate(vec2(uv.x, uv.y), lens_center, -rotation);

    // Mix in overlay_color and lens texture, add some noise to start color to reduce appearance of rings
    vec4 lens = texture2D(u_texture, coords);
    vec3 noise_color = start_color + vec3((random(coords) - 0.5) * 0.05);
         noise_color = clamp(noise_color, 0.0, 1.0);
    lens = vec4( mix(lens.rgb, noise_color, color_tint * (1.0 - (radius / lens_size)*0.75)), 1.0 );


    // ***** Bit Depth (0.0 to 256.0)
    highp float bit_depth = u_bitrate;
    lens.rgb = vec3(floor(lens.r * bit_depth), floor(lens.g * bit_depth), floor(lens.b * bit_depth)) / bit_depth;


    gl_FragColor = vec4(lens.rgb, lens.a * u_alpha);

}













