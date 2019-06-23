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

void main( void ) {

    gl_FragColor = texture2D(u_tex, coordinates.st).rgba * vec4(u_alpha, u_alpha, u_alpha, u_alpha);

}


















