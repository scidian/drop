#version 120
#ifdef GL_ES
precision highp float;
#endif
//
//
//  Fragment Shader - Shadow Map
//
//

// ***** Input from Vertex Shader
varying highp vec2      coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform highp sampler2D u_texture;                      // Occluder Map Texture
uniform lowp  vec2      u_resolution;                   // X: Total diameter of light
                                                        // Y: Diameter scaled based on screen size to light diameter ratio
uniform highp float     u_ray_count;                    // Width of 1D Shadow Map (i.e. number of rays to send out)
uniform highp float     u_depth;                        // Z-Order of light

// Other Variables
const   lowp  float THRESHOLD = 0.75;               // Alpha threshold for our occlusion map
const   highp float PI = 3.14159;                   // Pi
const   highp float RAD = 6.2831853;                // 2.0 * PI is 360 degrees in radians

// Unpacks a float from a color
highp float unpackColor(highp vec3 color) {
    return (color.r * 255.0) + (color.g * 255.0) * 256.0 + (color.b * 255.0) * 256.0 * 256.0;
}

void main(void) {        

    highp float distance =     1.0;
    highp float rays =         u_ray_count;
    highp float full_length =  u_resolution.x;
    highp float ray_length =   u_resolution.y;
    highp float ray_diff =     ray_length / rays;
    highp float small_length = 0.05 / (ray_length * (ray_length / full_length));

    for (highp float travel_y = 0.0; travel_y < ray_length; travel_y += ray_diff) {

        // Rectangular to Polar filter
        highp vec2    norm = highp vec2(coordinates.s * ray_diff, (travel_y / ray_length) ) * 2.0 - 1.0;
        highp float  theta = PI * 1.5 + norm.x * PI;
        highp float radius = (1.0 + norm.y) * 0.5;

        // Coordinate which we will sample from occluder map
        highp vec2 coord = highp vec2(-radius * sin(theta), radius * cos(theta)) / 2.0 + 0.5;

        // Sample the occluder map
        highp vec4 data = texture2D(u_texture, coord).rgba;

        // Current distance is how far from the top we've come
        highp float dst = travel_y / ray_length;

        // If we've hit an opaque fragment (occluder), then get new distance...
        // If the new distance is below the current, then we'll use that for our ray
        if (data.a > THRESHOLD) {
            // Retreive depth value stored in occluder_frag shader (stored in r/g/b)
            highp float z = (unpackColor(data.rgb) / 1000.0) - 5000.0;

            // Make sure light is behind object in z order
            if (u_depth <= z) {

                // Adds a little extra to get light closer to edges
                float add_extra = (ray_diff * small_length);
                if (theta < 0.0) theta += RAD;                              // Add 360 degrees in radians if theta is less than zero
                if (theta > 1.57 && theta < 4.80) {                         // If angle in towards the bottom of screen (> 90 and < 270), reduce add_extra
                    if (theta > 3.90)
                        add_extra *= 0.05;
                    else
                        add_extra *= 0.10;
                } else if ((theta > 4.88 && theta < 5.65) ||                //      Or if angle is around 280 to 315 degrees
                           (theta > 6.00)) {                                //      Or if angle is close to being back around to straight up, 350+ degrees
                    add_extra *= 0.10;
                }
                distance = min(distance, dst) + add_extra;

                // We are very close, move on
                if (distance <= add_extra) break;

            }
        }

    }

    gl_FragColor = highp vec4(highp vec3(distance), 1.0);
}
















