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
uniform highp vec2      u_resolution;                   // X: Total diameter of light
                                                        // Y: Diameter scaled based on screen size to light diameter ratio
uniform highp float     u_ray_count;                    // Width of 1D Shadow Map (i.e. number of rays to send out)

uniform highp float     u_depth;                        // Z-Order of light

// Other Variables
const highp float   THRESHOLD = 0.75;               // Alpha threshold for our occlusion map
const highp float   PI = 3.14159;                   // Pi

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
    highp float small_length = 0.01 / (ray_length * (ray_length / full_length));

    for (highp float travel_y = 0.0; travel_y < ray_length; travel_y += ray_diff) {

        // Rectangular to Polar filter
        highp vec2   norm = highp vec2(coordinates.s * ray_diff, (travel_y / ray_length) ) * 2.0 - 1.0;
        highp float theta = PI * 1.5 + norm.x * PI;
        highp float     r = (1.0 + norm.y) * 0.5;

        // Coordinate which we will sample from occluder map
        highp vec2 coord = highp vec2(-r * sin(theta), r * cos(theta)) / 2.0 + 0.5;

        // Sample the occluder map
        highp vec4 data = texture2D(u_texture, coord).rgba;

        // Current distance is how far from the top we've come
        highp float dst = travel_y / ray_length;

        // If we've hit an opaque fragment (occluder), then get new distance...
        // If the new distance is below the current, then we'll use that for our ray
        if (data.a > THRESHOLD) {
            // Retreive depth value stored in occluder_frag shader (stored in red)
            //                           99.6078453063964844    // Does not work
            //float z = (data.r * 200.0) - 99.607845;
            //                           99.6078453063964843    // Works
            highp float z = (unpackColor(data.rgb) / 1000.0) - 5000.0;

            if (u_depth <= z) {
                // Adds a little extra to get light closer to edges
                distance = min(distance, dst) + small_length;

                // We are very close, move on
                if (distance <= small_length) break;
            }
        }

    }

    gl_FragColor = highp vec4(highp vec3(distance), 1.0);
}
















