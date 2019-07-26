#version 120
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

uniform lowp  float u_alpha;                        // Opacity
uniform lowp  float u_zoom;                         // Current zoom level
uniform lowp  vec3  u_position;                     // Current camera position
uniform highp float u_width;                        // Texture Width
uniform highp float u_height;                       // Texture Height
uniform highp float u_time;                         // Time in seconds

uniform lowp  float u_water_top;                    // Top of Water,        from 0.0 to 1.0 in screen coordinates
uniform lowp  float u_water_bottom;                 // Bottom of Water,     from 0.0 to 1.0 in screen coordinates
uniform lowp  float u_water_left;                   // Left side of Water,  from 0.0 to 1.0 in screen coordinates
uniform lowp  float u_water_right;                  // Right side of Water, from 0.0 to 1.0 in screen coordinates

uniform highp vec3  u_color;                        // Water Color, r/g/b               0.0 to 1.0 x 3
uniform highp float u_color_tint;                   // Water Color Tint Percent
uniform highp float u_reflection;                   // Reflection Opacity               good start = 0.5

uniform highp float u_ripple_length;                // Vertical wave length                 0.0 to  20.0    good = 5 big wave, 10 small ripples
uniform highp float u_ripple_speed;                 // Vertical wave speed                  0.0 to  10.0    good = 1 big wave,  5 small ripples

//####################################################################################
//##        Main Shader Function
//####################################################################################
void main( void ) {

    // ***** Water Input Variables
    vec3  overlay_color =       u_color;
    float color_tint =          u_color_tint;
    float reflection_opacity =  u_reflection;

    float ripple_length =  pow((u_ripple_length/2.5), 2.0);
    float ripple_speed =        u_ripple_speed /5.0;



    float wave_min_width_x =    0.25;                   // Starting vertical wave width         0.0 to   1.0    good =  0.25
    float wave_stretch_x =      2.00;                   // Stretches width away from the start  0.0 to  10.0    good =  3.00

    float wave_height =         0.0;                    // Horizontal wave length               0.0 to   1.0    good =  0.25
    float peak_distance =   pow(1.5, 2.0);              // Lower is further apart               0.0 to  20.0    good =  5.00

    float bob_speed =           0.0;//3.0;                    // Between 0.0 and 5.0                                  good =  2.00
    float bob_amount =          0.0;//5.0;                    // Between 0.0 and 50.0                                 good =  2.00

    vec3  surface_color =       vec3(0.75, 0.75, 1.0);
    float surface_tint =        0.5;
    float surface_height =      7.5;

    // Refraction amounts on the different textures
    float refract_reflection =    0.0 * u_zoom;
    float refract_underwater =    0.0 * u_zoom;
    float refract_texture    =    0.0 * u_zoom;
    float refract_top_of_water =  0.0 * u_zoom;




    // Move coordinates into a vec2 that is not read-only
    highp vec2 coords = coordinates.st;
    float time = u_time;
    float y_start = u_water_top;                                                // 0.0 is bottom, 1.0 is top

    // Calculate some position and scaling values
    float shrink_texture = 3.0;                                                 // 1.0 = normal size, 4.0 equals 1/4 size
    float player_x = u_position.x*0.00083 * (1.0/u_zoom);
    float player_y = u_position.y*0.00083 * (1.0/u_zoom);
    float diff_w = (u_width* (1.0/u_zoom)) / 1200.0;
    float diff_h = (u_height*(1.0/u_zoom)) / 1200.0;

    // Grab value from 2D Water Normal Texture, use it to get refraction values
    vec3  displacement = texture2D(u_texture_displacement,
                                   vec2(( (coords.x*diff_w)/u_zoom + player_x - time/20.0), //50.0),                        // gives movement
                                        ( (coords.y*diff_h)/u_zoom + player_y)) / (shrink_texture) ).rgb;
    float refract_x = (displacement.x - displacement.y)*0.01;
    float refract_y = (displacement.y - displacement.x)*0.01;



    // Calculates vertical waves (ripples)
    float xoffset = cos(time*ripple_speed + ripple_length*((coords.y*diff_h)/u_zoom + player_y)*u_zoom) *
                    (wave_min_width_x + (y_start - coords.y) * wave_stretch_x) * 0.005*u_zoom;

    // Calculates horizontal waves
    float bob = sin(time*bob_speed + coords.x+refract_x*refract_reflection) * bob_amount * u_zoom;
    float yoffset = sin( (coords.x*diff_w + player_x) * (peak_distance) + time ) * (wave_height*0.01*u_zoom);
    float y_top = y_start - abs(refract_y)*refract_top_of_water + yoffset*bob;

    // Grab the reflected value from existing screen
    vec4 reflection, water, original;
    reflection = texture2D(u_texture, vec2(              coords.x + refract_x*refract_reflection - xoffset,
                                           2.0*y_start - coords.y - refract_y*refract_reflection + yoffset*bob) );
    // Simple reflection
    //reflection = texture2D(u_texture, vec2(coords.x, ((2.0 * y_start) - coords.y)));



    // If we are above offset, just pass pixel color through as it is above the top of the wave
    if (coords.y > y_top || coords.y < u_water_bottom || coords.x < u_water_left || coords.x > u_water_right) {
        discard;

    // Otherwise Refract Original Pixel Color, mix in Overlay Color
    } else {
        // Existing pixel color refracted through water
        original = texture2D(u_texture, vec2(coords.x + refract_x*refract_underwater + xoffset,
                                             coords.y - refract_y*refract_underwater + yoffset));

        // Mix in overlay_color and water texture
        float bob_texture =   10.0;                                                 // Increases bob effect on water texture
              bob_texture *= bob*refract_x*refract_texture*(1.0/u_zoom);
        water = texture2D(u_texture_water, vec2(
                    (coords.x*diff_w*(1.0/u_zoom) + refract_x*refract_texture*(1.0/u_zoom) + player_x + xoffset), // + (time/50.0),      // gives movement
                    (coords.y*diff_h*(1.0/u_zoom) + refract_y*refract_texture*(1.0/u_zoom) + player_y + yoffset*bob_texture)) * (shrink_texture*u_zoom) );
        water = vec4(mix(water.rgb, overlay_color, color_tint),   1.0);

        original = mix(original,   water,          u_alpha);
        original = mix(original,   reflection,     reflection_opacity*reflection.a);

        // Surface coloring (sea foam)
        float foam_distance = (surface_height + sin(refract_x+time/50.0)) / u_height * u_zoom;
        if (coords.y > (y_top - foam_distance)) {
            float foam_position = (coords.y - (y_top - foam_distance)) / foam_distance;
            original = vec4(mix(original.rgb, surface_color, surface_tint * clamp(2.0*foam_position, 0.0, 1.0)), 1.0);

            // Anti-alias the top of the sea foam
            if (coords.y > (y_top - (u_zoom/u_height)))
                original = mix(texture2D(u_texture, vec2(coords.x, coords.y)), original, 0.5);
        }
    }


    gl_FragColor = original;
}













