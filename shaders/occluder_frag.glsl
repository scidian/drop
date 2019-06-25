#ifdef GL_ES
precision mediump float;
#endif
//
//
//  Occluder Fragment Shader
//
//

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_tex;                          // Texture
uniform lowp float  u_alpha;                        // Opacity

uniform highp float u_depth;                        // Z-Order of item, (recalculated to number from 0.0 to 1.0
                                                    //                   assuming max distances -100 to 100)

void main( void ) {

    float z = u_depth + 100.0;                                                             // Add 100 to equalize negative z numbers
          z = clamp(z, 0.0, 200.0);                                                        // Clamp to within near and far plane
          z = z / 200.0;                                                                   // Move to range between 0.0 and 1.0, store in red pixel
    gl_FragColor = vec4(z, 0.0, 0.0, texture2D(u_tex, coordinates.st).a * u_alpha);

}


















