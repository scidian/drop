#version 120
#ifdef GL_ES
precision highp float;
#endif
//
//
//  Occluder Fragment Shader
//
//

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform highp sampler2D u_tex;                      // Texture
uniform highp float     u_alpha;                    // Opacity
uniform highp float     u_depth;                    // Z-Order of item, assuming max distances u_near_plane to u_far_plane

uniform highp float     u_near_plane;               // Near plane in DrOpenGL
uniform highp float     u_far_plane;                // Far plane in DrOpenGL

// Packs a float into a color
highp vec3 packColor(highp float f) {
    highp vec3 color;
    color.b = floor(f / 65536.0);
    color.g = floor((f - (color.b * 65536.0)) / 256.0);
    color.r = floor((f - (color.b * 65536.0)) - (color.g * 256.0));
    return color / 255.0;                           // Return has 3 components in range [0..255], normalize them
}

void main( void ) {

    highp float z =  clamp(u_depth, u_near_plane, u_far_plane);                 // Clamp to within near and far plane
                z += abs(u_near_plane);                                         // Add near plane to equalize negative z numbers
                z *= 1000.0;                                                    // Multiply by 1000 to include decimal precision
    highp vec3  color_out = packColor(z);                                       // Pack z order into a vec3

    gl_FragColor = highp vec4(color_out, texture2D(u_tex, coordinates.st).a * u_alpha);

}


















