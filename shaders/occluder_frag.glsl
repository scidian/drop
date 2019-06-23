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

uniform float       u_depth;                        // Z-Order of item, (recalculated to number from 0.0 to 1.0
                                                    //                   assuming near / far plane of -1000 to 1000)

void main( void ) {

    float z = (u_depth + 1000.0) / 2000.0;
    gl_FragColor = vec4(z, 0.0, 0.0, texture2D(u_tex, coordinates.st).a * u_alpha);

}


















