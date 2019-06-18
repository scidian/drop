#ifdef GL_ES
precision mediump float;
#endif
//
//
//  Fragment Shader - Shadow Map
//
//

// ***** Input from Vertex Shader
varying highp vec4  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_texture;
uniform vec2        u_resolution;

// Other Variables
const float         THRESHOLD = 0.5;                // Alpha threshold for our occlusion map
const float         PI = 3.14;                      // Pi
const float         RAYS = 360.0;                   // Circle

void main(void) {
    float distance = 1.0;

///    for (float y = 0.0; y < u_resolution.y; y += 1.0) {
    for (float y = 0.0; y < RAYS; y += 1.0) {

        // Rectangular to Polar filter
        vec2  norm =  vec2(coordinates.s, y / u_resolution.y) * 2.0 - 1.0;
//        vec2  norm =  vec2(coordinates.s, y / RAYS) * 2.0 - 1.0;
        float theta = PI * 1.5 + norm.x * PI;
        float r = (1.0 + norm.y) * 0.5;

        // Coordinate which we will sample from occluder map
        vec2 coord = vec2(-r * sin(theta), -r * cos(theta)) / 2.0 + 0.5;

        // Sample the occlusion map
        vec4 data = texture2D(u_texture, coord).rgba;

        // Current distance is how far from the top we've come
        float dst = y / u_resolution.y / 1.25;
//        float dst = y / u_resolution.y;

        // If we've hit an opaque fragment (occluder), then get new distance
        // If the new distance is below the current, then we'll use that for our ray
        float caster = data.a;
        if (caster > THRESHOLD) {
            distance = min(distance, dst);
            //NOTE: we could probably use "break" or "return" here
        }

    }

    gl_FragColor = vec4(vec3(distance), 1.0);
}
















