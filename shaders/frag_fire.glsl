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
uniform sampler2D   u_texture_noise;                // Fire Noise Texture
uniform sampler2D   u_texture_flame;                // Fire Flame Texture
uniform highp float u_alpha;                        // Texture opacity
uniform highp float u_time;                         // Time in seconds
uniform highp vec2  u_pos;                          // Fire position in world
uniform lowp  float u_width;                        // Fire width
uniform lowp  float u_height;                       // Fire height

uniform lowp  vec3  u_start_color;                  // Start (Top)  Color, r/g/b      0.0 to 1.0 x 3
uniform lowp  vec3  u_end_color;                    // End (Bottom) Color, r/g/b      0.0 to 1.0 x 3
uniform lowp  float u_intensity;                    // Flame Intensity
uniform lowp  float u_smoothness;                   // Flame Smoothness
uniform lowp  float u_wavy;                         // Flame Waviness


//####################################################################################
//##    Simplex 3D Noise, by Ian McEwan, Ashima Arts
//####################################################################################
vec4  permute(vec4 x) {          return mod(((x*34.0)+1.0)*x, 289.0); }
vec4  taylorInvSqrt(vec4 r) {    return 1.79284291400159 - 0.85373472095314 * r; }
float snoise(vec3 v) {
    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i  = floor(v + dot(v, vec3(C.y, C.y, C.y)) );
    vec3 x0 = v - i + dot(i, vec3(C.x, C.x, C.x)) ;

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );

    //  x0 = x0 - 0. + 0.0 * C
    vec3 x1 = x0 - i1 + 1.0 * vec3(C.x, C.x, C.x);
    vec3 x2 = x0 - i2 + 2.0 * vec3(C.x, C.x, C.x);
    vec3 x3 = x0 - 1. + 3.0 * vec3(C.x, C.x, C.x);

    // Permutations
    i = mod(i, 289.0 );
    vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

    // Gradients
    // ( N*N points uniformly over a square, mapped onto an octahedron.)
    float n_ = 1.0/7.0; // N=7
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

    vec4 x = x_ * ns.x + vec4(ns.y, ns.y, ns.y, ns.y);
    vec4 y = y_ * ns.x + vec4(ns.y, ns.y, ns.y, ns.y);
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );

    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww;

    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);

    // Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), dot(p2,x2), dot(p3,x3) ) );
}



//####################################################################################
//##        Main Shader Function
//####################################################################################
void main( void ) {

    // ***** Fire Input Variables
    vec3  start_color =         u_start_color;
    vec3  end_color =           u_end_color;
    float intensity =           u_intensity;                    // 0.5 is default   ranging form 0% to 100% (0 to 1)
    float smoothness =    clamp(u_smoothness, 0.001, 1.0);      // Smoothness       1.0 very smooth, 0.1 nice   0.001 very fine
    float wavy =                1.0 - u_wavy;                   // 0.0 (most wavy) to 1.0 (no wavy)


    float speed = 7.0;                                          // Speed            1.0 very fast,   7.0 nice,  25.00 very slow


    // ***** Octaves
    float result = 0.0;
    float t = u_time / speed;
    for (float i = 0.0; i < 5.0; i++) {
        result += snoise( vec3((coordinates.x * 2.0) / smoothness, (coordinates.y - t) / smoothness, t * 5.0)) / pow(2.0, i);
        smoothness /= 2.0;
    }
    result =  (result + 2.0) / 4.0;

    // Calculate color values
    float p1 = pow(coordinates.y, 1.7);
    float p2 = 8.0 * (1.0 - p1);
    result = pow(result, 8.0 - p2);                 // Powers for steeper curves
    float g = pow(result, 6.0);                     // Power for coloring


    // ***** Playing with Increasing Color Intensity
    result *= (intensity + 0.7);
    g *= (intensity + 0.7);


    // ***** Original Color Fade
    vec4 color;
    color = vec4(result,      g,    0.0, 1.0);          // Yellow   to  Red
//    color = vec4(result,    0.0,      g, 1.0);          // Purple   to  Red
//    color = vec4(   0.0, result,      g, 1.0);          // Cyan     to  Green
//    color = vec4(     g, result,    0.0, 1.0);          // Yellow   to  Green
//    color = vec4(   0.0,      g, result, 1.0);          // Cyan     to  Blue
//    color = vec4(     g,    0.0, result, 1.0);          // Purple   to  Blue


    // ***** Trying to use custom colors
//    vec3 black =   vec3(0.0, 0.0, 0.0);
//    vec3 color_1 = start_color;//mix(black, start_color, result);
//    vec3 color_2 = end_color;//mix(black, end_color,   g);
//    vec4 color = vec4(smoothstep(color_1, color_2, vec3(result, g, result)), 1.0);

//    // ***** Bit Depth (0.0 to 16.0)
//    highp float bit_depth = pow(2.0, 3.0);
//    color.rgb = vec3(floor(color.r * bit_depth), floor(color.g * bit_depth), floor(color.b * bit_depth)) / bit_depth;

    // ***** Apply Flame Shape
    float noise_density = 1.0;      // higher is more dense
    float whisp = 2.5;              // higher is less whispier
    float flame_alpha = texture2D(u_texture_flame, coordinates).b;
    float noise_alpha = flame_alpha - (texture2D(u_texture_noise, (coordinates - vec2(u_pos.x - (result/ whisp), u_time / speed)) * noise_density).a);
    color.a = mix(noise_alpha, flame_alpha, wavy);

    // ***** Final Output
    gl_FragColor = color * vec4(u_alpha, u_alpha, u_alpha, u_alpha);
}






//####################################################################################
//##        Main Shader Function (Original)
//####################################################################################
void main_old( void ) {

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
    float noise = 1.0;

    highp float time = (u_time * -1.0 * speed) + (u_pos.x * 0.2) + (u_pos.y * 0.2);


    // Pixelation
    vec2 coords = coordinates;
//    float u_width = 200.0;
//    float u_height = 200.0;
//    highp float pixel_width =  (1.0 / (u_width));
//    highp float pixel_height = (1.0 / (u_height));
//    float u_pixel_x = 2.0;
//    float u_pixel_y = 2.0;
//    highp float real_pixel_x = ((coords.x / 1.0) * u_width);
//    highp float real_pixel_y = (((1.0 - coords.y) / 1.0) * u_height);
//    coords.x =       u_pixel_x * floor(real_pixel_x / u_pixel_x) * pixel_width;
//    coords.y = 1.0 - u_pixel_y * floor(real_pixel_y / u_pixel_y) * pixel_height;


    // Generate noisy x value
    vec2 uv = coords.xy;
    vec2 x1 = vec2( ((uv.x)*1.40 + 0.010) * noise, ((uv.y    ) + time*0.69) * noise );
    vec2 x2 = vec2( ((uv.x)*0.50 - 0.033) * noise, ((uv.y*2.0) + time*0.12) * noise );
    vec2 x3 = vec2( ((uv.x)*0.94 + 0.020) * noise, ((uv.y*3.0) + time*0.61) * noise );

    // Generate noisy y value
    vec2 y1 = vec2( ((uv.x)*0.70 - 0.010) * noise, ((uv.y    ) + time*0.27) * noise );
    vec2 y2 = vec2( ((uv.x)*0.45 + 0.033) * noise, ((uv.y*1.9) + time*0.61) * noise );
    vec2 y3 = vec2( ((uv.x)*0.80 - 0.020) * noise, ((uv.y*2.5) + time*0.51) * noise );

    float nx1 = (texture2D(u_texture_noise, x1).a - 0.5) * 2.0;
    float nx2 = (texture2D(u_texture_noise, x2).a - 0.5) * 2.0;
    float nx3 = (texture2D(u_texture_noise, x3).a - 0.5) * 2.0;
    float ny1 = (texture2D(u_texture_noise, y1).a - 0.5) * 2.0;
    float ny2 = (texture2D(u_texture_noise, y2).a - 0.5) * 2.0;
    float ny3 = (texture2D(u_texture_noise, y3).a - 0.5) * 2.0;
    float noise_x = clamp(nx1 + nx2 + nx3, -1.0, 1.0);
    float noise_y = clamp(ny1 + ny2 + ny3, -1.0, 1.0);


    // ***** This gives blur to the bottom of the fire
    float sparse = 0.15;                                            // 0.0 to 1.0, 0.25 looks nice
    vec2  final_noise = vec2(noise_x, noise_y);
    float perturb = uv.y * sparse + 0.02;
    final_noise = (final_noise * perturb) + uv - 0.02;


    // ***** Figure out what color to use
    vec4 color = texture2D(u_texture_noise, final_noise);
    color = vec4(mix(start_color, end_color, color.x), color.a);
    //ORIGINAL: color = vec4(color.x * 2.2, color.y * 1.2, (color.y / color.x) * 0.2, 1.0);

    // ***** Clamps alpha to fire texture blue channel (with some offsets for an imperfect flame texture)
    vec2 alpha_val = clamp(coords,      0.0, 1.0);
    vec2 final_val = clamp(final_noise, 0.0, 1.0);
    color.a  = texture2D(u_texture_noise, final_val).b;
    color.a *= texture2D(u_texture_noise, alpha_val).b;
    color.a *= texture2D(u_texture_flame, alpha_val).b;
    color.a *= intensity / 5.0;

//      color.a  = texture2D(u_texture_flame, coordinates).b;


    // Bit Depth
//    highp float bit_depth = pow(2.0, 2.0);
//    color.rgb = vec3(floor(color.r * bit_depth), floor(color.g * bit_depth), floor(color.b * bit_depth)) / bit_depth;
//    color.a = floor(color.a * bit_depth) / bit_depth;


    // ***** Add existing opacity of object
    gl_FragColor = color * vec4(u_alpha, u_alpha, u_alpha, u_alpha);
}













