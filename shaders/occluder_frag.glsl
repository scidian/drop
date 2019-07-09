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
uniform highp float     u_depth;                    // Z-Order of item, assuming max distances -5000 to 5000

// Packs a float into a color
highp vec3 packColor(float f) {
    highp vec3 color;
    color.b = floor(f / 256.0 / 256.0);
    color.g = floor((f - (color.b * 65536.0)) / 256.0);
    color.r = floor((f - (color.b * 65536.0)) - (color.g * 256.0));
    return color / 255.0;                           // vec3 has 3 components in range [0..255], normalize it
}

void main( void ) {

    highp float z =  clamp(u_depth, -5000.0, 5000.0);                           // Clamp to within near and far plane
                z += 5000.0;                                                    // Add 5000 to equalize negative z numbers
                z *= 1000.0;                                                    // Multiply by 1000 to remove decimals

    highp vec3  color_out = packColor(z);

    gl_FragColor = highp vec4(color_out, texture2D(u_tex, coordinates.st).a * u_alpha);

}


















