#version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Fire Fragment Shader
//##
//####################################################################################

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_texture;                      // Fire Noise Texture
uniform highp float u_alpha;                        // Texture opacity
uniform highp float u_time;                         // Time in seconds
uniform highp vec2  u_pos;                          // Fire position in world

uniform lowp  vec3  u_start_color;                  // Start (Top)  Color, r/g/b      0.0 to 1.0 x 3
uniform lowp  vec3  u_end_color;                    // End (Bottom) Color, r/g/b      0.0 to 1.0 x 3
uniform lowp  float u_intensity;                    // Flame Intensity


//####################################################################################
//##        Main Shader Function
//####################################################################################
void main( void ) {

    // Fire texture
    //  r (x) = red color noise
    //  g (y) = green color noise
    //  b (z) = outline
    //  a (w) = texture noise

    // ***** Fire Input Variables
    vec3  start_color =         u_start_color;
    vec3  end_color =           u_end_color;
    float intensity =           u_intensity * 100.0;                // 50.0 is default intensity (ranging form 0% to 100% should be 0.0 to 20.0)



    float speed = 10.0 * 0.045;                                     // 10.0 should be default speed
    float noise_scale = 0.5;

    highp float time = (u_time * -1.0 * speed) + (u_pos.x * 0.2) + (u_pos.y * 0.2);


    // Generate noisy x value
    vec2 uv = coordinates.xy;
    vec2 x1 = vec2(uv.x*1.40 + 0.010, uv.y     + time*0.69);
    vec2 x2 = vec2(uv.x*0.50 - 0.033, uv.y*2.0 + time*0.12);
    vec2 x3 = vec2(uv.x*0.94 + 0.020, uv.y*3.0 + time*0.61);
    float nx1 = (texture2D(u_texture, x1).a - 0.5) * 2.0;
    float nx2 = (texture2D(u_texture, x2).a - 0.5) * 2.0;
    float nx3 = (texture2D(u_texture, x3).a - 0.5) * 2.0;
    float noise_x = clamp(nx1 + nx2 + nx3, -1.0, 1.0);

    // Generate noisy y value
    vec2 y1 = vec2(uv.x*0.70 - 0.010, uv.y     + time*0.27);
    vec2 y2 = vec2(uv.x*0.45 + 0.033, uv.y*1.9 + time*0.61);
    vec2 y3 = vec2(uv.x*0.80 - 0.020, uv.y*2.5 + time*0.51);
    float ny1 = (texture2D(u_texture, y1).a - 0.5) * 2.0;
    float ny2 = (texture2D(u_texture, y2).a - 0.5) * 2.0;
    float ny3 = (texture2D(u_texture, y3).a - 0.5) * 2.0;
    float noise_y = clamp(ny1 + ny2 + ny3, -1.0, 1.0);


    // ***** This gives blur to the bottom of the fire
    float sparse = 0.15;                                            // 0.0 to 1.0, 0.25 looks nice
    vec2  final_noise = vec2(noise_x, noise_y);
    float perturb = uv.y * sparse + 0.02;
    final_noise = (final_noise * perturb) + uv - 0.02;


    // ***** Figure out what color to use
    vec4 color = texture2D(u_texture, final_noise);
    color = vec4(color.x * 2.2, color.y * 1.2, (color.y / color.x) * 0.2, 1.0);
    color = vec4(mix(start_color, end_color, color.x), 1.0);


    // ***** Clamps alpha to fire texture blue channel (with some offsets for an imperfect flame texture)
    vec2 alpha_val = uv;
         alpha_val.x -= 0.05;
         alpha_val.y *= 0.70;
         alpha_val = clamp(alpha_val, 0.0, 1.0);
    vec2 final_val = final_noise;
         final_val.x -= 0.05;
         final_val.y *= 0.70;
         final_val = clamp(final_val, 0.0, 1.0);
    color.a  = texture2D(u_texture, final_val).z;
    color.a *= texture2D(u_texture, alpha_val).z;
    color.a *= intensity / 5.0;


    // ***** Add existing opacity of object
    gl_FragColor = color * vec4(u_alpha, u_alpha, u_alpha, u_alpha);
}













