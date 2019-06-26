#ifdef GL_ES
precision mediump float;
#endif
//
//
//  Fragment Shader - 2D Light
//
//

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_texture;                      // Shadow Map 1D
uniform float       u_light_diameter;               // Original Light Diameter
uniform float       u_light_fitted;                 // Light Diameter Fit to Max Size
uniform lowp float  u_alpha;                        // Opacity of light                 0.0 to 1.0

uniform vec3        u_color;                        // Light Color, r/g/b               0.0 to 1.0 x 3
uniform lowp vec2   u_cone;                         // Start angle, End Angle           0.0, 360.0 is Full Circle
uniform lowp float  u_light_shadows;                // Visible shadows                  0.0 to 100.0
uniform lowp float  u_intensity;                    // Intensity                        0.0 to 100.0?
uniform lowp float  u_blur;                         // Blur                             0.0 to 100.0
uniform bool        u_draw_shadows;                 // Draw shadows                     true or false

const float         PI = 3.14159;

// Sample from the 1D Distance (Shadow) Map
float sample(vec2 coord, float r) {
    return step(r, texture2D(u_texture, coord).r);
}

void main(void) {
    //shrink_multiplier = static_cast<float>( qSqrt(double(light->getLightDiameter()) / double(light->getLightDiameterFitted())) );
    //m_light_shader.setUniformValue( m_uniform_light_resolution, light->getLightDiameter(), shrink_multiplier);

    // Rectangular to Polar
    vec2  norm =        coordinates.st * 2.0 - 1.0;
    float theta =       atan(norm.y, norm.x);
    float r =           length(norm);
    float coord =       (theta + PI) / (2.0 * PI);

    float shrink =      u_light_diameter / u_light_fitted;
    float intensity =   sqrt(u_intensity);

    // Check that pixel is within allowed light cone
    if (theta < 0.0) theta += (2.0 * PI);
    if (u_cone.x > u_cone.y) {
        if (theta < u_cone.x && theta > u_cone.y)   return;
    } else {
        if (theta < u_cone.x || theta > u_cone.y)   return;
    }

    float sum = 0.0;
    if (u_draw_shadows) {
        // The tex coord to sample our 1D lookup texture, always 0.0 on y axis
        vec2  tc = vec2(coord, 0.0);

        // The center tex coord, which gives us hard shadows
        float center = sample(tc, r);

        // We multiply the blur amount by our distance from center, this leads to more blurriness as the shadow "fades away"
        float blur = (1.0 / u_light_diameter) * smoothstep(0.0, 1.0, r * shrink) * (u_blur * 0.1);

        // Now we use a simple gaussian blur, sum of 1.0 == in light, 0.0 == in shadow
        sum += sample(vec2(tc.x - 4.0 * blur, tc.y), r) * 0.05;
        sum += sample(vec2(tc.x - 3.0 * blur, tc.y), r) * 0.09;
        sum += sample(vec2(tc.x - 2.0 * blur, tc.y), r) * 0.12;
        sum += sample(vec2(tc.x - 1.0 * blur, tc.y), r) * 0.15;
        sum += center * 0.16;
        sum += sample(vec2(tc.x + 1.0 * blur, tc.y), r) * 0.15;
        sum += sample(vec2(tc.x + 2.0 * blur, tc.y), r) * 0.12;
        sum += sample(vec2(tc.x + 3.0 * blur, tc.y), r) * 0.09;
        sum += sample(vec2(tc.x + 4.0 * blur, tc.y), r) * 0.05;

        // Increase intensity, reduce shadows
        sum *= intensity;
        float reduce_shadows = u_light_shadows * 2.0;
        sum = (sum + reduce_shadows) / (1.0 + reduce_shadows);

        // If we need to reduce intensity
        if (intensity < 1.0) sum *= intensity;
    } else {
        sum = 1.0 * intensity;
    }

    // Multiply the sum by our distance, which gives us a radial falloff
    float amount = sum * smoothstep(1.0, 0.0, r * shrink);

    // Multiply by light color
    gl_FragColor = vec4(u_color, u_alpha) * vec4(amount, amount, amount, amount);
}

















