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

const float         PI =  3.1415926;
const float         RAD = 6.2831853;                // 2.0 * PI is 360 degrees in radians
const float         DEG = 0.0174533;                // One Degree in Radians is 0.0174533


// Sample from the 1D Distance (Shadow) Map
float sample(vec2 coord, float r) {
    return step(r, texture2D(u_texture, coord).r);
}

void main(void) {

    // Rectangular to Polar
    vec2  norm =        coordinates.st * 2.0 - 1.0;
    float theta =       atan(norm.y, norm.x);
    float r =           length(norm);
    float coord =       (theta + PI) / RAD;

    float shrink =      u_light_diameter / u_light_fitted;
    float blur =        u_blur + 0.001;
    float opacity =     u_alpha;


    // Adjust our 0 to 100 range for intensity to better range (0 to 50 becomes 0 to 1, 50 to 100 becomes 1 to 10
    float intensity;
    if (u_intensity >= 50.0) {
        intensity = (u_intensity - 40.0) / 10.0;
    } else {
        intensity = u_intensity / 50.0;
    }


    // Check that pixel is within allowed light cone
    if (theta < 0.0) theta += RAD;                              // Add 360 degrees in radians if theta is less than zero
    if (u_cone.x > u_cone.y) {
        if (theta < u_cone.x && theta > u_cone.y) {
            float diff_x = smoothstep(0.0, 1.0, ((u_cone.x - theta) / DEG) * (1.0/blur));
            float diff_y = smoothstep(0.0, 1.0, ((theta - u_cone.y) / DEG) * (1.0/blur));
            opacity -= min(diff_x, diff_y);
            if (opacity <= 0.01) return;
        }
    } else {
        if (theta < u_cone.x || theta > u_cone.y) {
            float diff_x = 0.0;
            float diff_y = 0.0;
            if (theta > u_cone.y) {
                diff_x = smoothstep(0.0, 1.0, ((u_cone.x + RAD - theta) / DEG) * (1.0/blur));
                diff_y = smoothstep(0.0, 1.0, ((theta - u_cone.y)       / DEG) * (1.0/blur));
            } else if (theta < u_cone.x) {
                diff_x = smoothstep(0.0, 1.0, ((u_cone.x - theta)       / DEG) * (1.0/blur));
                diff_y = smoothstep(0.0, 1.0, ((theta + RAD - u_cone.y) / DEG) * (1.0/blur));
            }
            opacity -= min(diff_x, diff_y);
            if (opacity <= 0.01) return;
        }
    }


    // Multiply the sum by our distance, which gives us a radial falloff
    float amount = intensity * smoothstep(1.0, 0.0, r * shrink);


    float sum = 0.0;
    if (u_draw_shadows) {
        // The tex coord to sample our 1D lookup texture, always 0.0 on y axis
        vec2  tc = vec2(coord, 0.0);

        // The center tex coord, which gives us hard shadows
        float center = sample(tc, r);

        // We multiply the blur amount by our distance from center, this leads to more blurriness as the shadow "fades away"
        float blur = (1.0 / u_light_diameter) * smoothstep(0.0, 1.0, r * shrink) * (blur * 0.05);       // The 0.05 reduces our blur down, could be adjusted

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

        // At this point sum is less than or equal to 1.0
        sum *= amount;

        float shade = (amount - sum) * (u_light_shadows / 100.0);
        amount -= shade;
    }

    // Multiply by light color
    gl_FragColor = vec4(u_color, opacity) * vec4(amount, amount, amount, amount);
}


















