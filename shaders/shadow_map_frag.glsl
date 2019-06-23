#ifdef GL_ES
precision mediump float;
#endif
//
//
//  Fragment Shader - Shadow Map
//
//

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_texture;
uniform vec2        u_resolution;
uniform float       u_ray_count;

// Other Variables
const float         THRESHOLD = 0.75;               // Alpha threshold for our occlusion map
const float         PI = 3.14159;                   // Pi

void main(void) {
    float distance =     1.0;
    float rays =         u_ray_count;
    float full_length =  u_resolution.x;
    float ray_length =   u_resolution.y;
    float ray_diff =     ray_length / rays;
    float small_length = 0.02 / (ray_length * (ray_length / full_length));

    for (float travel_y = 0.0; travel_y < ray_length; travel_y += 1.0 * ray_diff) {

        // Rectangular to Polar filter
        vec2   norm = vec2(coordinates.s * ray_diff, (travel_y / ray_length) ) * 2.0 - 1.0;
        float theta = PI * 1.5 + norm.x * PI;
        float     r = (1.0 + norm.y) * 0.5;

        // Coordinate which we will sample from occluder map
        vec2 coord = vec2(-r * sin(theta), r * cos(theta)) / 2.0 + 0.5;

        // Sample the occluder map
        vec4 data = texture2D(u_texture, coord).rgba;

        // Current distance is how far from the top we've come
        float dst = travel_y / ray_length;

        // If we've hit an opaque fragment (occluder), then get new distance...
        // If the new distance is below the current, then we'll use that for our ray
        float caster = data.a;
        if (caster > THRESHOLD) {
            // Adds a little extra to get light closer to edges
            distance = min(distance, dst) + small_length;

            // We are very close, move on
            if (distance <= small_length) break;
        }

    }

    gl_FragColor = vec4(vec3(distance), 1.0);
}
















