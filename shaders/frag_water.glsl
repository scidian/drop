// version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Water / Reflection / Refraction Fragment Shader
//##
//####################################################################################

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_texture;                      // Texture
uniform sampler2D   u_texture_displacement;         // Refraction Texture
uniform sampler2D   u_texture_water;                // Water Texture

uniform mediump float u_alpha;                      // Opacity
uniform mediump float u_zoom;                       // Current zoom level
uniform mediump vec3  u_position;                   // Current camera position
uniform highp   float u_width;                      // Texture Width
uniform highp   float u_height;                     // Texture Height
uniform highp   float u_time;                       // Time in seconds
uniform mediump float u_angle;                      // Angle of water in degrees

uniform mediump float u_water_top;                  // Top of Water,        from 0.0 to 1.0 in screen coordinates
uniform mediump float u_water_bottom;               // Bottom of Water,     from 0.0 to 1.0 in screen coordinates
uniform mediump float u_water_left;                 // Left side of Water,  from 0.0 to 1.0 in screen coordinates
uniform mediump float u_water_right;                // Right side of Water, from 0.0 to 1.0 in screen coordinates

uniform mediump vec3  u_start_color;                // Water Start (Top)  Color, r/g/b      0.0 to 1.0 x 3
uniform mediump vec3  u_end_color;                  // Water End (Bottom) Color, r/g/b      0.0 to 1.0 x 3
uniform mediump float u_color_tint;                 // Water Color Tint Percent
uniform mediump float u_reflection;                 // Reflection Opacity                   good start = 0.5

uniform mediump float u_ripple_frequency;           // Vertical wave length                 0.0 to 100.0    good = 25 big wave, 100 small ripples
uniform mediump float u_ripple_speed;               // Vertical wave speed                  0.0 to 100.0    good = 10 big wave,  50 small ripples
uniform mediump float u_ripple_amplitude;           // Starting vertical wave width         0.0 to 100.0    good = 25
uniform mediump float u_ripple_stretch;             // Stretches width away from the start  0.0 to 100.0    good = 25

uniform mediump float u_wave_frequency;             // Horizontal wave length               0.0 to 100.0    good = 10
uniform mediump float u_wave_speed;                 // Bob speed                            0.0 to 100.0    good = 20
uniform mediump float u_wave_amplitude;             // Bob amount                           0.0 to 100.0    good = 20

uniform mediump vec3  u_surface_color;              // Surface Color, r/g/b                 0.0 to 1.0 x 3
uniform mediump float u_surface_tint;               // Surface Color Tint Percent
uniform mediump float u_surface_height;             // Surface Height
uniform         bool  u_surface_keep_flat;          // Keep top of water flat?

uniform mediump float u_refract_reflection;         // good = 1.0
uniform mediump float u_refract_underwater;         // good = 2.0
uniform mediump float u_refract_texture;            // good = 8.0
uniform mediump float u_refract_foam;               // good = 2.0

uniform mediump float u_movement_speed;             // Moves texture, top of water left or right, default should be 0

uniform highp   float u_pixel_x;// = 1.0;           // Pixel Width X    1.0 Normal, 4.0 is nice pixelation
uniform highp   float u_pixel_y;// = 1.0;           // Pixel Width Y    1.0 Normal, 4.0 is nice pixelation
uniform mediump float u_bitrate;// = 256;           // Bitrate          Editor:    1 to  256


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
//##    Main Shader Function
//####################################################################################
void main( void ) {

    // ***** Water Input Variables
    vec3  start_color =         u_start_color;
    vec3  end_color =           u_end_color;
    float color_tint =          u_color_tint;
    float reflection_opacity =  u_reflection;

    float ripple_frequency =    u_ripple_frequency*2.0;
    float ripple_speed =        u_ripple_speed/10.0;
    float ripple_amplitude =    u_ripple_amplitude/100.0;
    float ripple_stretch =      u_ripple_stretch/10.0;

    float wave_frequency =      u_wave_frequency*2.0;
    float wave_speed =          u_wave_speed/10.0;
    float wave_amplitude =      u_wave_amplitude/10.0;

    vec3  surface_color =       u_surface_color;
    float surface_tint =        u_surface_tint;
    float surface_height =      u_surface_height;

    // Refraction amounts on the different textures
    float refract_reflection =  u_refract_reflection/20.0;
    float refract_underwater =  u_refract_underwater/20.0;
    float refract_texture    =  u_refract_texture/4.0;
    float refract_foam =        u_refract_foam/20.0;

    // Moves texture, top of water left or right
    float movement_speed =      u_movement_speed/10.0;
    float refract_speed =       1.0;

    // ***** Move coordinates into a vec2 that is not read-only
    highp vec2 coords = coordinates.xy;

    // ***** Pixelation
    if (u_pixel_x > 1.0 || u_pixel_y > 1.0) {
        highp float dx = u_pixel_x * (1.0 / u_width);
        highp float dy = u_pixel_y * (1.0 / u_height);
        highp float pixel_x = dx * floor(coords.x / dx) + (dx / 2.0);
        highp float pixel_y = dy * floor(coords.y / dy) + (dy / 2.0);
        coords = vec2(pixel_x, pixel_y);
    }

    // Apply rotation
    float rotation = u_angle;   // mod(u_time, 360.0) * 15.0; // <-- spins based on time
    vec2  water_center = vec2(u_water_right - (u_water_right - u_water_left)/2.0, u_water_top - (u_water_top - u_water_bottom)/2.0);
    coords = rotate(coords, water_center, rotation);

    float time = u_time;
    float movement = -movement_speed * (time/20.0);
    if (movement < 0.0) wave_speed *= -1.0;
    float y_start = u_water_top;                                                // 0.0 is bottom, 1.0 is top


    // ***** Calculate some position and scaling values
    const float shrink_texture = 1.5;                                           // 1.0 = normal size, 4.0 equals 1/4 size... 3.0 was good for 512x512 textures
    float player_x = u_position.x*0.00083 * (1.0/u_zoom);
    float player_y = u_position.y*0.00083 * (1.0/u_zoom);
    float diff_w = (u_width  * (1.0/u_zoom)) / 1200.0;
    float diff_h = (u_height * (1.0/u_zoom)) / 1200.0;

    float zoom_coord_x = ((coords.x - 0.5)*diff_w)/u_zoom;
    float zoom_coord_y = ((coords.y - 0.5)*diff_h)/u_zoom;


    // ***** Grab value from 2D Water Normal Texture, use it to get refraction values
    vec3  displacement = texture2D(u_texture_displacement, vec2(  (zoom_coord_x + player_x) * u_zoom + movement/2.0,
                                                                 ((zoom_coord_y + player_y) * u_zoom) / shrink_texture) - (refract_speed*(time/100.0))).rgb;
    float refract_x = (displacement.x - displacement.y) * 0.01;
    float refract_y = (displacement.y - displacement.x) * 0.01;


    // ***** Calculates vertical waves (ripples)
    float xoffset = cos(time*ripple_speed + ripple_frequency * (zoom_coord_y + player_y) * u_zoom) * (0.005 * u_zoom) *
                    (ripple_amplitude + (y_start - coords.y) * ripple_stretch);

    // Calculates horizontal waves
    float yoffset = sin(time*wave_speed   + wave_frequency   * (zoom_coord_x + player_x) * u_zoom) * (0.005 * u_zoom);
    float bob =     sin(time*wave_speed   + coords.x + refract_x*refract_reflection) * wave_amplitude;
    //float bob =   wave_amplitude;

    // Decide top of water start
    float y_top = y_start - abs(refract_y)*refract_foam*u_zoom + yoffset*bob;
    float add_foam;
    if (u_surface_keep_flat) {
        add_foam = u_water_top - y_top;
        y_top = u_water_top;
    } else {
        add_foam = 0.0;
    }


    // ***** If out of range, don't process this pixel
    //if (coordinates.y > y_top || coordinates.y < u_water_bottom || coordinates.x < u_water_left || coordinates.x > u_water_right) {
    if (coords.y > y_top || coords.y < u_water_bottom || coords.x < u_water_left || coords.x > u_water_right) {
        discard;
    }


    // ***** Grab the reflected value from existing screen
    vec4 reflection, water, original;
    //reflection = texture2D(u_texture, vec2(coords.x, ((2.0 * y_start) - coords.y)));                                      // OPTIONAL: Simple reflection
    //reflection = texture2D(u_texture, vec2(              coords.x + refract_x*refract_reflection - xoffset,               // OPTIONAL: Original reflection
    //                                       2.0*y_start - coords.y - refract_y*refract_reflection + yoffset*bob) );
    vec2 reflect_refraction = vec2(refract_x*refract_reflection - xoffset, refract_y*refract_reflection + yoffset*bob);
         reflect_refraction = rotate(reflect_refraction, vec2(0.0, 0.0), rotation);
    vec2 get_reflection = vec2(coords.x, y_start + (y_start - coords.y));
         get_reflection = rotate(get_reflection, water_center, -rotation);
         get_reflection.x += reflect_refraction.x;
         get_reflection.y -= reflect_refraction.y;
    reflection = texture2D(u_texture, get_reflection);



    // Existing pixel color refracted through water
    vec2 under_refraction = vec2(refract_x*refract_underwater + xoffset, refract_y*refract_underwater - yoffset*bob);
         under_refraction = rotate(under_refraction, vec2(0.0, 0.0), rotation);
    vec2 get_under = vec2(coordinates.x + under_refraction.x, coordinates.y - under_refraction.y);
         get_under = clamp(get_under, (1.0 / u_width), 1.0 - (1.0 / u_height));
    original = texture2D(u_texture, get_under);

    // Mix in overlay_color and water texture
    water = texture2D(u_texture_water, vec2(
                          (zoom_coord_x + refract_x*refract_texture*(1.0/u_zoom)  + player_x + xoffset/u_zoom + movement/2.0),
                          (zoom_coord_y + refract_y*refract_texture*(1.0/u_zoom)  + player_y + yoffset*bob)) * (shrink_texture*u_zoom) );
    vec3 water_color = mix(start_color, end_color, smoothstep(0.0, 1.0, (coords.y - u_water_top) / (u_water_bottom - u_water_top)));
    water = vec4(mix(water.rgb, water_color, color_tint), 1.0);

    original = mix(original,   water,          u_alpha);
    original = mix(original,   reflection,     reflection_opacity);

    // Surface coloring (sea foam)
    float foam_distance = (surface_height + sin(refract_x + movement)) / u_height * u_zoom;
          foam_distance += add_foam;
    if (coords.y > (y_top - foam_distance)) {
        float foam_position = (coords.y - (y_top - foam_distance)) / foam_distance;
        original = vec4(mix(original.rgb, surface_color, surface_tint * clamp(2.0*foam_position, 0.0, 1.0)), 1.0);

        // Anti-alias the top of the sea foam
        if (coords.y > (y_top - (u_zoom/u_height))) {
            original = mix(texture2D(u_texture, coordinates), original, 0.5);
        }
    }


    // ***** Bit Depth (0.0 to 256.0)
    highp float bit_depth = u_bitrate;
    original.rgb = vec3(floor(original.r * bit_depth), floor(original.g * bit_depth), floor(original.b * bit_depth)) / bit_depth;


    gl_FragColor = original;
}













