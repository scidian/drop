#ifdef GL_ES
precision mediump float;
#endif
//
//
//  Fragment Shader - 2D Light
//
//

// ***** Input from Vertex Shader
varying highp vec4  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_texture;
uniform vec2        u_resolution;
uniform vec3        u_color;

const float         PI = 3.14159;

// Sample from the 1D Distance (Shadow) Map
float sample(vec2 coord, float r) {
    return step(r, texture2D(u_texture, coord).r);
}

void main(void) {
    // Rectangular to Polar
    vec2  norm =    coordinates.st * 2.0 - 1.0;
    float theta =   atan(norm.y, norm.x);
    float r =       length(norm);
    float coord =   (theta + PI) / (2.0 * PI);

    // The tex coord to sample our 1D lookup texture, always 0.0 on y axis
    vec2 tc = vec2(coord, 0.0);

    // The center tex coord, which gives us hard shadows
    float center = sample(tc, r);

    // We multiply the blur amount by our distance from center, this leads to more blurriness as the shadow "fades away"
    float blur = (1.0 / u_resolution.x) * smoothstep(0.0, 1.0, r);

    // Now we use a simple gaussian blur, sum of 1.0 == in light, 0.0 == in shadow
    float sum = 0.0;
    sum += sample(vec2(tc.x - 4.0*blur, tc.y), r) * 0.05;
    sum += sample(vec2(tc.x - 3.0*blur, tc.y), r) * 0.09;
    sum += sample(vec2(tc.x - 2.0*blur, tc.y), r) * 0.12;
    sum += sample(vec2(tc.x - 1.0*blur, tc.y), r) * 0.15;
    sum += center * 0.16;
    sum += sample(vec2(tc.x + 1.0*blur, tc.y), r) * 0.15;
    sum += sample(vec2(tc.x + 2.0*blur, tc.y), r) * 0.12;
    sum += sample(vec2(tc.x + 3.0*blur, tc.y), r) * 0.09;
    sum += sample(vec2(tc.x + 4.0*blur, tc.y), r) * 0.05;

    // Multiply the summed amount by our distance, which gives us a radial falloff, then multiply by light color
    float amount = sum * smoothstep(1.0, 0.0, r);
    gl_FragColor = vec4(u_color, 1.0) * vec4(amount, amount, amount, amount);
}



















