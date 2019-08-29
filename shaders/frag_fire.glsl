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
// Fire Noise Texture
//  r (x) = red color noise
//  g (y) = green color noise
//  b (z) = outline
//  a (w) = texture noise
uniform sampler2D   u_texture_noise;                // Fire Noise Texture
uniform sampler2D   u_texture_flame;                // Fire Flame Texture
uniform highp float u_alpha;                        // Texture opacity
uniform highp float u_time;                         // Time in seconds
uniform highp vec2  u_pos;                          // Fire position in world
uniform lowp  float u_width;                        // Fire width
uniform lowp  float u_height;                       // Fire height

uniform       int   u_shape;                        // Which shape mask we are using
uniform lowp  vec3  u_start_color;                  // Start (Top)  Color, r/g/b        0.0 to 1.0 x 3
uniform lowp  vec3  u_end_color;                    // End (Bottom) Color, r/g/b        0.0 to 1.0 x 3
uniform lowp  vec3  u_smoke_color;                  // Smoke Color, r/g/b               0.0 to 1.0 x 3
uniform lowp  float u_intensity;                    // Flame Intensity
uniform lowp  float u_smoothness;                   // Flame Smoothness
uniform lowp  float u_wavy;                         // Flame Waviness
uniform lowp  float u_speed;                        // Flame Speed                      1.0 to 25.0

uniform highp float u_pixel_x;// = 1.0;             // Pixel Width X    1.0 Normal, 4.0 is nice pixelation
uniform highp float u_pixel_y;// = 1.0;             // Pixel Width Y    1.0 Normal, 4.0 is nice pixelation
uniform lowp  float u_bitrate;// = 256;             // Bitrate          Editor:    1 to  256


//####################################################################################
//##    Description :   Array and textureless GLSL 3D simplex noise
//##    Author:         Iam McEwan, Ashima Arts
//##    License:        Copyright (C) 2011 Ashima Arts. All rights reserved.
//##                    Distributed under the MIT License.
//##    Source:         http://weber.itn.liu.se/~stegu/jgt2011/supplement.pdf
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

    // Gradients from 7x7 points over a square, mapped onto an octahedron
    float n_ = 1.0/7.0;
    vec3  ns = n_ * D.wyz - D.xzx;
    vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  // mod(p, 7*7)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );             // mod(j,7)

    vec4 x = x_ * ns.x + vec4(ns.y, ns.y, ns.y, ns.y);
    vec4 y = y_ * ns.x + vec4(ns.y, ns.y, ns.y, ns.y);
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );

    //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 − 1.0;
    //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 − 1.0;
    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww;

    vec3 p0 = vec3(a0.xy, h.x);
    vec3 p1 = vec3(a0.zw, h.y);
    vec3 p2 = vec3(a1.xy, h.z);
    vec3 p3 = vec3(a1.zw, h.w);

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
//##        Shape Functions
//####################################################################################
const vec2 center = vec2(0.5, 0.8);
float egg_shape(vec2 coord, float radius){
    coord.y = 1.0 - coord.y;
    vec2 diff = abs(coord - center);

    if (coord.y < center.y){
        diff.y /= 2.0;
    } else {
        diff.y *= 2.0;
    }

    float  dist =  sqrt(diff.x * diff.x + diff.y * diff.y) / radius;
    float  value = sqrt(1.0 - dist * dist);
    return clamp(value, 0.0, 1.0);
}



//####################################################################################
//##        Main Shader Function
//####################################################################################
void main( void ) {

    // ***** Fire Input Variables
    vec3  start_color =         u_start_color;
    vec3  end_color =           u_end_color;
    float intensity =           u_intensity;                        // 0.5  is default   ranging form 0% to 100% (0 to 1)
    float frequency =     clamp(u_smoothness / 1.5, 0.001, 1.0);    // 0.30 is default   1.0 very smooth, 0.1 nice   0.001 very fine
    float wavy =                1.0 - u_wavy;                       // 0.5  is default   0.0 (most wavy) to 1.0 (no wavy)
    float speed;                                                    // Speed             1.0 very slow,  15.0 nice,  25.00 very fast
    if (u_speed > 25.0) {
        speed = sqrt(26.0 - (u_speed - 25.0));
    } else if (u_speed <= 0.001) {
        speed = 200.0;
    } else {
        speed = 30.0 - u_speed;
    }


    // ***** Grab coordinates into an editable vec2
    vec2 coords = coordinates;

    // ***** Pixelation
    if (u_pixel_x > 1.0 || u_pixel_y > 1.0) {
        highp float pixel_width =  (1.0 / (u_width));
        highp float pixel_height = (1.0 / (u_height));
        highp float real_pixel_x = ((coords.x / 1.0) * u_width);
        highp float real_pixel_y = (((1.0 - coords.y) / 1.0) * u_height);
        highp float pixel_x =       u_pixel_x * floor(real_pixel_x / u_pixel_x) * pixel_width;
        highp float pixel_y = 1.0 - u_pixel_y * floor(real_pixel_y / u_pixel_y) * pixel_height;
        coords = vec2(pixel_x, pixel_y);
    }


    // ***** Perlin Noise Octaves
    float color_1 = 0.0, color_2 = 0.0;
    float octaves = 5.0;
    float power =   2.0;
    float z = u_time / speed;
    for (float i = 0.0; i < octaves; i++) {
        float amplitude = pow(power, i);
        float x_noise = ((coords.x * 1.5) / frequency) + u_pos.x;
        float y_noise = ((coords.y - z)   / frequency) + u_pos.y;
        float z_noise = z * 4.0;
        color_1 += snoise(vec3(x_noise, y_noise, z_noise)) / amplitude;
        frequency /= 2.0;
    }
    color_1 = (color_1 + 2.0) / (octaves - 1.0);

    // ***** Calculate color values
    float top_shrink;
    float bottom_shrink;                                    // Higher numbers shrinks bottom color
    // Candle
    if (u_shape == 1) {
        top_shrink =    7.0;
        bottom_shrink = 4.4;
    // Torch 0, Square 2, Triangle 3
    } else {
        top_shrink =    7.0;
        bottom_shrink = 6.0;
    }
    float p1 = pow(coords.y, 1.695);
    float p2 = top_shrink * (1.0 - p1);
    color_1 =  pow(color_1, top_shrink - p2);
    color_2 =  pow(color_1, bottom_shrink);

    // ***** Increasing Color Intensity
    //color_1 *= pow(intensity + 0.5, 2.0);
    //color_2 *= pow(intensity + 0.5, 2.0);
    color_1 *= intensity * 2.0;
    color_2 *= intensity * 2.0;


    // ***** Original Color Fade
    vec4 color;
    //  color = vec4( color_1, color_2,     0.0, 1.0);          // Yellow   to  Red
    //  color = vec4( color_1,     0.0, color_2, 1.0);          // Purple   to  Red
    //  color = vec4(     0.0, color_1, color_2, 1.0);          // Cyan     to  Green
    //  color = vec4( color_2, color_1,     0.0, 1.0);          // Yellow   to  Green
    //  color = vec4(     0.0,       g, color_1, 1.0);          // Cyan     to  Blue
    //  color = vec4( color_2,     0.0, color_1, 1.0);          // Purple   to  Blue

    // ***** Custom Colors
    vec4 smoke =  vec4(u_smoke_color, 0.0);
    vec4 top =    mix(smoke, vec4(start_color, 1.0), clamp(color_1 * 1.35, 0.0, 1.0));
    vec4 bottom = mix(smoke, vec4(end_color,   1.0), clamp(color_2 * 1.35, 0.0, 1.0));
    color =       mix(top,   bottom,                 clamp((1.0 - coords.y) * color_2, 0.0, 1.0));


    // ***** Bit Depth (0.0 to 256.0)
    highp float bit_depth = u_bitrate;
    color.rgb = vec3(floor(color.r * bit_depth), floor(color.g * bit_depth), floor(color.b * bit_depth)) / bit_depth;


    // ***** Apply Flame Shape
    float noise_density = 1.0;      // higher is more dense
    float whisp = 2.5;              // higher is less whispier
    float flame_alpha;
    // Candle
    if (u_shape == 1) {
        float egg_s =  egg_shape(coords, 0.4);
              //egg_s += egg_shape(coords, 0.2) / 2.0;
        flame_alpha = egg_s;
    } else {
        flame_alpha = texture2D(u_texture_flame, coords).b;
    }
    float noise_alpha = texture2D(u_texture_noise, (coords - vec2(u_pos.x - (color_1 / whisp), u_time / speed)) * noise_density).a;
          noise_alpha = flame_alpha - noise_alpha;
    color.a = mix(noise_alpha, flame_alpha, wavy);


    // ***** Final Output
    gl_FragColor = color * vec4(u_alpha, u_alpha, u_alpha, u_alpha);
}











