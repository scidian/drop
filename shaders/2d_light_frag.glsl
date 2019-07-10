#version 120
#ifdef GL_ES
precision highp float;
#endif
//
//
//  Fragment Shader - 2D Light
//
//

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform highp sampler2D u_texture;                  // Shadow Map 1D

uniform highp float u_light_diameter;               // Original Light Diameter
uniform highp float u_light_fitted;                 // Light Diameter Fit to Max Size
uniform lowp  float u_alpha;                        // Opacity of light                 0.0 to 1.0

uniform highp vec3  u_color;                        // Light Color, r/g/b               0.0 to 1.0 x 3
uniform lowp  vec2  u_cone;                         // Start angle, End Angle           0.0, 360.0 is Full Circle
uniform highp float u_light_shadows;                // Visible shadows                  0.0 to 100.0
uniform highp float u_intensity;                    // Intensity                        0.0 to 100.0?
uniform highp float u_blur;                         // Blur                             0.0 to 100.0
uniform bool        u_draw_shadows;                 // Draw shadows                     true or false
uniform bool        u_is_glow;                      // True if this is a glow light

// Local Constants / Variables
const   highp float PI =  3.1415926;
const   highp float RAD = 6.2831853;                // 2.0 * PI is 360 degrees in radians
const   highp float DEG = 0.0174533;                // One Degree in Radians is 0.0174533

        highp float shadow_fade;                    // Used to fade the start of shadows


// Sample from the 1D Distance (Shadow) Map
highp float sampleShadow(highp vec2 coord_in, highp float r) {
    // Original Return
    //return step(r, texture2D(u_texture, coord_in).r);

    // Playing Around With Fading Shadows
    //return (texture2D(u_texture, coord_in).r / r);

    // Awesome!!! Fades into the start of Shadow
    return 1.0 - clamp((r - texture2D(u_texture, coord_in).r) * shadow_fade, 0.0, 1.0);
}


void main(void) {

    // User Inputs
    highp float shrink =      u_light_diameter / u_light_fitted;
    highp float blur =        u_blur + 0.001;
    highp float opacity =     u_alpha;

    // Rectangular to Polar
    highp vec2  norm =        coordinates.st * 2.0 - 1.0;
    highp float theta =       atan(norm.y, norm.x);
    highp float radius =      length(norm);
    highp float coord =       (theta + PI) / RAD;


    // Adjust our 0 to 100 range for intensity to better range (0 to 50 becomes 0 to 1, 50 to 100 becomes 1 to 10
    highp float intensity;
    if (u_intensity >= 50.0) {
        intensity = (u_intensity - 40.0) / 10.0;
    } else {
        intensity = u_intensity / 50.0;
    }


    // Check that pixel is within allowed light cone
    if (theta < 0.0) theta += RAD;                              // Add 360 degrees in radians if theta is less than zero
    if (u_cone.x > u_cone.y) {
        if (theta < u_cone.x && theta > u_cone.y) {
            highp float diff_x = smoothstep(0.0, 1.0, ((u_cone.x - theta) / DEG) * (1.0/blur));
            highp float diff_y = smoothstep(0.0, 1.0, ((theta - u_cone.y) / DEG) * (1.0/blur));
            opacity -= min(diff_x, diff_y);
            if (opacity <= 0.01) discard;
        }
    } else {
        if (theta < u_cone.x || theta > u_cone.y) {
            highp float diff_x = 0.0;
            highp float diff_y = 0.0;
            if (theta > u_cone.y) {
                diff_x = smoothstep(0.0, 1.0, ((u_cone.x + RAD - theta) / DEG) * (1.0/blur));
                diff_y = smoothstep(0.0, 1.0, ((theta - u_cone.y)       / DEG) * (1.0/blur));
            } else if (theta < u_cone.x) {
                diff_x = smoothstep(0.0, 1.0, ((u_cone.x - theta)       / DEG) * (1.0/blur));
                diff_y = smoothstep(0.0, 1.0, ((theta + RAD - u_cone.y) / DEG) * (1.0/blur));
            }
            opacity -= min(diff_x, diff_y);
            if (opacity <= 0.01) discard;
        }
    }


    // Multiply the sum by our distance, which gives us a radial falloff
    highp float falloff = intensity * smoothstep(1.0, 0.0, radius * shrink);

    // Set Shadow fade amount based on light type
    if (u_is_glow) {
        shadow_fade = 10.0;             // Equals distance of 0.10)
    } else {
        shadow_fade = 50.0;             // Equals distance of 0.02
    }

    highp float sum = 0.0;
    if (u_draw_shadows) {
        // The texture coordinate to sample our 1D lookup texture, always 0.0 on y axis
        highp vec2  tc = highp vec2(coord, 0.0);

        // The center sample, which gives us hard shadows
        highp float center = sampleShadow(tc, radius);

        // Calculate blur
        highp float radial_blur = 1.5;                                                      // Ability to increase (1.5) blur away towards edges
        highp float center_dist = smoothstep(0.0, 1.0, radius * radial_blur * shrink);      // To be multiplied into fuzz, more bluriness as the shadow "fades away"
                    center_dist += ((1.0 - center_dist) * 0.50);                            // Adding (0.50) sets the minimum blur at the center, can be adjusted
        highp float fuzz = (1.0 / u_light_diameter) * (blur * 0.05) * center_dist;          // The (0.05) reduces our blur down, could be adjusted

        // Now we use a simple gaussian blur, sum of 1.0 == in light, 0.0 == in shadow
        sum += sampleShadow(highp vec2(tc.x - 4.0 * fuzz, tc.y), radius) * 0.05;
        sum += sampleShadow(highp vec2(tc.x - 3.0 * fuzz, tc.y), radius) * 0.09;
        sum += sampleShadow(highp vec2(tc.x - 2.0 * fuzz, tc.y), radius) * 0.12;
        sum += sampleShadow(highp vec2(tc.x - 1.0 * fuzz, tc.y), radius) * 0.15;
        sum += center * 0.16;
        sum += sampleShadow(highp vec2(tc.x + 1.0 * fuzz, tc.y), radius) * 0.15;
        sum += sampleShadow(highp vec2(tc.x + 2.0 * fuzz, tc.y), radius) * 0.12;
        sum += sampleShadow(highp vec2(tc.x + 3.0 * fuzz, tc.y), radius) * 0.09;
        sum += sampleShadow(highp vec2(tc.x + 4.0 * fuzz, tc.y), radius) * 0.05;

        // At this point sum is less than or equal to 1.0
        sum *= falloff;

        highp float shade = (falloff - sum) * (u_light_shadows / 100.0);
        falloff -= shade;
    }

    // Multiply by light color
    gl_FragColor = highp vec4(u_color, opacity) * highp vec4(falloff, falloff, falloff, falloff);




//    // Hard Light Blending
//    vec4 upper = light_color;   //texture2D(texLmap, v_vTexcoord);
//    vec4 lower = texture2D(u_base_texture, coordinates.st);
//    vec4 out_color = vec4(0.0, 0.0, 0.0, upper.a);
//    if (upper.r > 0.5) {
//        out_color.r = (1.0 - (1.0-lower.r) * (1.0-2.0*(upper.r-0.5)));
//    } else {
//        out_color.r = lower.r * (2.0*upper.r);
//    }
//    if (upper.g > 0.5) {
//        out_color.g = (1.0 - (1.0-lower.g) * (1.0-2.0*(upper.g-0.5)));
//    } else {
//        out_color.g = lower.g * (2.0*upper.g);
//    }
//    if (upper.b > 0.5) {
//        out_color.b = (1.0 - (1.0-lower.b) * (1.0-2.0*(upper.b-0.5)));
//    } else {
//        out_color.b = lower.b * (2.0*upper.b);
//    }
//    if (upper.a > 0.5) {
//        out_color.a = (1.0 - (1.0-lower.a) * (1.0-2.0*(upper.a-0.5)));
//    } else {
//        out_color.a = lower.a * (2.0*upper.a);
//    }
//    gl_FragColor = vec4(out_color.rgb, upper.a);

}


















