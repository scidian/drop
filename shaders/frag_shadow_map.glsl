// version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Fragment Shader - Shadow Map
//##
//####################################################################################

// ***** Input from Vertex Shader
varying highp vec2      coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform highp sampler2D u_texture;                      // Occluder Map Texture
uniform lowp  vec2      u_resolution;                   // X: Total diameter of light
                                                        // Y: Diameter scaled based on screen size to light diameter ratio
uniform highp float     u_ray_count;                    // Width of 1D Shadow Map (i.e. number of rays to send out)
uniform highp float     u_depth;                        // Z-Order of light
uniform highp float     u_near_plane;                   // Near Plane in DrOpenGL

// Other Variables
const   lowp  float THRESHOLD = 0.75;               // Alpha threshold for our occlusion map
const   highp float PI =  3.14159;                  // Pi
const   highp float RAD = 6.2831853;                // 2.0 * PI is 360 degrees in radians


//####################################################################################
//##    Unpacks a float from a color
//####################################################################################
highp float unpackColor(highp vec3 color) {
    return (color.r * 255.0) + (color.g * 255.0) * 256.0 + (color.b * 255.0) * 256.0 * 256.0;
}

//####################################################################################
//##    Returns a random number, that is between 0.0 and 0.999999 inclusive
//####################################################################################
float random(vec2 p) {
    return fract(cos(dot(p, vec2(23.14069263277926, 2.665144142690225))) * 12345.6789);
}


//####################################################################################
//##    Main Shader Function
//####################################################################################
void main(void) {        

    highp float distance =     1.0;
    highp float rays =         u_ray_count;             // Width of 1D Shadow Map (i.e. number of rays to send out)
    highp float full_length =  u_resolution.x;
    highp float ray_length =   u_resolution.y;
    highp float ray_diff =     ray_length / rays;

    bool  covered = false;
    float countdown = 0.0;

    for (highp float travel_y = 0.0; travel_y < ray_length; travel_y += ray_diff) {

        // Rectangular to Polar filter
        highp vec2    norm = vec2(coordinates.s * ray_diff, (travel_y / ray_length) ) * 2.0 - 1.0;
        highp float  theta = PI * 1.5 + norm.x * PI;
        highp float radius = (1.0 + norm.y) * 0.5;

        // Coordinate which we will sample from occluder map
        highp vec2 coord = vec2(-radius * sin(theta), radius * cos(theta)) / 2.0 + 0.5;

        // Sample the occluder map
        highp vec4 data = texture2D(u_texture, coord).rgba;

        // Current distance is how far from the top we've come
        highp float dst = travel_y / ray_length;

        // If we've hit an opaque fragment (occluder), then get new distance...
        // If the new distance is below the current, then we'll use that for our ray
        if (data.a > THRESHOLD) {
            // Retreive depth value stored in occluder_frag shader (stored in r/g/b)
            highp float z = (unpackColor(data.rgb) / 1000.0) - abs(u_near_plane);

            // Make sure light is behind object in z order
            if (u_depth <= z) {
                // ***** We check from the inside out, so if we made it here, we've hit the closest occluder to the center

//                // If the light is covered, allow some light to extrude, mark as covered here to start this process
//                if (travel_y <= 0.00001) {
//                    covered = true;
//                }

                // If light is not covered, we have hit the closest occluder (shadow object), we can exit
                if (!covered) {
                    distance = dst;
                    break;
                }
                continue;
            }
        }

        // If light is covered, we let some light extrude out
//        if (covered) {
//            countdown = 1.5 + (random(vec2(coord.x, theta)) * 0.20);
//            covered = false;
//        }
//        // While countdown is greater than zero light is allowed to continue
//        if (countdown > 0.0) {
//            countdown -= 0.05;
//            if (countdown <= 0.00001) {
//                distance = dst;
//                break;
//            }
//        }

    }   // End For

    gl_FragColor = vec4(vec3(distance), 1.0);
}
















