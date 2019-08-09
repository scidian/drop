#version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Main Fragment Shader
//##
//####################################################################################

// ***** Input from Vertex Shader
varying highp vec2  coordinates;                    // Texture Coodinates

// ***** Input from Engine
uniform sampler2D   u_texture;                      // Texture

uniform lowp  float u_alpha;                        // Opacity
uniform lowp  vec3  u_tint;// = vec3(0, 0, 0);      // Tint, adds rgb to final output
uniform highp float u_zoom;                         // Zoom factor

uniform highp float u_width;                        // Texture Width
uniform highp float u_height;                       // Texture Height
uniform highp float u_time;                         // Time in seconds
uniform       bool  u_premultiplied;                // True if the texture we are using has premultiplied alphas (affects negative)

// EXACT SAME ORDER AS IN EDITOR!!!!!
uniform highp float u_pixel_x;// = 1.0;             // Pixel Width X    1.0 Normal, 4.0 is nice pixelation
uniform highp float u_pixel_y;// = 1.0;             // Pixel Width Y    1.0 Normal, 4.0 is nice pixelation
uniform highp vec2  u_pixel_offset;                 // Used to offset pixelation to reduce pixel flicker
uniform       bool  u_negative;// = false;          // Negative         True / False
uniform       bool  u_grayscale;// = false;         // Grayscale        True / False
uniform lowp  float u_hue;// = 0.0;                 // Hue              Editor:    0 to 360     Shader:  0.0 to 1.0
uniform lowp  float u_saturation;// = 0.0;          // Saturation       Editor: -255 to 255     Shader: -1.0 to 1.0
uniform lowp  float u_contrast;// = 0.0;            // Contrast         Editor: -255 to 255     Shader: -1.0 to 1.0
uniform lowp  float u_brightness;// = 0.0;          // Brightness       Editor: -255 to 255     Shader: -1.0 to 1.0

uniform lowp  float u_bitrate;// = 256;             // Bitrate          Editor:    1 to  16
uniform       bool  u_cartoon;// = false;           // Cartoon          True / False
uniform       bool  u_wavy;// = false;              // Wavy             True / False
uniform       bool  u_fisheye;// = false;           // Fisheye Lens     True / False


//####################################################################################
//##        Fast Rgb / Hsv Conversion Functions
//####################################################################################
// ***** Convert red/green/blue to hue/saturation/vibrance
vec3 rgbToHsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// ***** Convert hue/saturation/vibrance to red/green/blue
vec3 hsvToRgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}



//####################################################################################
//##        Cartoon Filter Functions
//####################################################################################
float edge_thres  = 0.2;
float edge_thres2 = 5.0;

// Averaged pixel intensity from 3 color channels
float avgIntensity(vec4 pix) {
    return (pix.r + pix.g + pix.b) / 3.0;
}

// Returns pixel color
float isEdge(in vec2 coords) {
    float dxtex = 1.0 / float(u_width);     //textureSize(u_texture, 0));
    float dytex = 1.0 / float(u_height);    //textureSize(u_texture, 0));
    float pix[9];
    int   k = -1;
    float delta;

    // Read neighboring pixel intensities
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            k++;
            pix[k] = avgIntensity( texture2D(u_texture, coords + vec2(float(i) * dxtex, float(j) * dytex)) );
        }
    }

    // Average color differences around neighboring pixels
    delta = (abs(pix[1] - pix[7]) +
             abs(pix[5] - pix[3]) +
             abs(pix[0] - pix[8]) +
             abs(pix[2] - pix[6]) ) / 4.0;

    return clamp(edge_thres2 * delta, 0.0, 1.0);
}

vec3 cartoonRgbToHsvV(float r, float g, float b) {
    float minv, maxv, delta;
    vec3 res;

    minv = min(min(r, g), b);
    maxv = max(max(r, g), b);
    res.z = maxv;                                           // v
    delta = maxv - minv;

    if( maxv != 0.0 )
        res.y = delta / maxv;                               // s
    else {
        // r = g = b = 0                                    // s = 0, v is undefined
        res.y = 0.0;
        res.x = -1.0;
        return res;
    }

    if ( r == maxv )     res.x = (g - b) / delta;           // between yellow & magenta
    else if( g == maxv ) res.x = 2.0 + (b - r) / delta;     // between cyan & yellow
    else                 res.x = 4.0 + (r - g) / delta;     // between magenta & cyan

    res.x = res.x * 60.0;                                   // degrees
    if (res.x < 0.0) res.x = res.x + 360.0;

    return res;
}

vec3 cartoonHsvToRgb(float h, float s, float v ) {
    int i;
    float f, p, q, t;
    vec3 res;

    if( s == 0.0 ) {
        // achromatic (grey)
        res.x = v;
        res.y = v;
        res.z = v;
        return res;
    }

    h /= 60.0;                      // sector 0 to 5
    i = int(floor( h ));
    f = h - float(i);               // factorial part of h
    p = v * ( 1.0 - s );
    q = v * ( 1.0 - s * f );
    t = v * ( 1.0 - s * ( 1.0 - f ) );

    if (i == 0) {       res.x = v;  res.y = t;  res.z = p;  }
    else if (i == 1) {  res.x = q;  res.y = v;  res.z = p;  }
    else if (i == 2) {  res.x = p;  res.y = v;  res.z = t;  }
    else if (i == 3) {  res.x = p;  res.y = q;  res.z = v;  }
    else if (i == 4) {  res.x = t;  res.y = p;  res.z = v;  }
    else {              res.x = v;  res.y = p;  res.z = q;  }

    return res;
}



//####################################################################################
//##        Main Shader Function
//####################################################################################
void main( void ) {

    // Move coordinates into a vec2 that is not read-only
    highp vec2 coords = coordinates.st;
    float time = u_time;

    // ***** WAVY
    if (u_wavy) {
        //time = 100.0;                           // !!! Disables imported time (turns off animation)
        vec2  tc =  coords.xy;
        vec2  p =   -1.0 + 2.0 * tc;
        float len = length(p);
        coords = tc + (p / len) * cos(len*12.0 - time*4.0) * 0.03;
    }


    // ***** 2D SHOCKWAVE
    if (u_wavy) {
//        vec2 center = vec2(0.5, 0.5);
//        vec3 shock_params = vec3(10.0, 0.8, 0.1);

//        time = mod(time, 2.0);

//        vec2 uv = coords;
//        vec2 tex_coord = coords;
//        float distance = distance(uv, center);
//        if ( (distance <= (time + shock_params.z)) && (distance >= (time - shock_params.z)) ) {
//            float diff = (distance - time);
//            float powDiff = 1.0 - pow(abs(diff*shock_params.x), shock_params.y);
//            float diffTime = diff  * powDiff;
//            vec2 diffUV = normalize(uv - center);
//            tex_coord = uv + (diffUV * diffTime);
//        }
//        gl_FragColor = texture2D(u_texture, tex_coord);
//        return;
    }


    // ***** GRID
//    if (u_wavy) {
//        float multiplicationFactor = 32.0;          // Scales the number of stripes
//        float threshold = 0.5;                      // Defines the with of the lines (1.0 equals full square)
//        vec2 t = coords * multiplicationFactor;
//        if (fract(t.s * (u_width / u_height)) > threshold && fract(t.t) > threshold ) {
//            discard;
//        }
//    }


    // ***** FISHEYE
    if (u_fisheye) {
        float lens_size = 0.4;                  // Lens size
        float x_pos = 0.5;
        float y_pos = 0.5;

        vec2  p = coords.xy;                    // Current texture coordinate
        vec2  m = vec2(x_pos, 1.0 - y_pos);     // Lens coordinate
        vec2  d = p - m;
        float r = sqrt(dot(d, d));              // Distance of pixel from m
        vec2  uv;

        if (r < lens_size - 0.01) {
            // Choose one formula to uncomment:
            //uv = m + vec2(d.x * abs(d.x), d.y * abs(d.y));                        // SQUAREXY
            uv = m + d * (r * 2.56); // a.k.a. m + normalize(d) * r * r             // SQUARER
            //uv = m + normalize(d) * sin(r * 3.14159 * 0.5);                       // SINER
            //uv = m + normalize(d) * asin(r) / (3.14159 * 0.5);                    // ASINR
            coords = vec2(uv.x, uv.y);
        }
    }

    // ***** PIXELATED
    vec4 texture_color;
    if (u_pixel_x > 1.0 || u_pixel_y > 1.0) {       

        highp float pixel_width =  (1.0 / (u_width));
        highp float pixel_height = (1.0 / (u_height));

        // ***** Method 1
        highp float real_pixel_x = ((coords.x / 1.0) * u_width) - (fract(u_pixel_offset.x)*u_zoom);
        highp float real_pixel_y = (((1.0 - coords.y) / 1.0) * u_height) - (fract(u_pixel_offset.y)*u_zoom);
        highp float pixel_x =       u_pixel_x * floor(real_pixel_x / u_pixel_x) * pixel_width;
        highp float pixel_y = 1.0 - u_pixel_y * floor(real_pixel_y / u_pixel_y) * pixel_height;

        // ***** Method 2
        //highp float dx = u_pixel_x * pixel_width;
        //highp float dy = u_pixel_y * pixel_height;
        //highp float pixel_x = dx * floor(coords.x / dx) + (dx / 2.0) - (fract(u_pixel_offset.x) * pixel_width);
        //highp float pixel_y = dy * floor(coords.y / dy) + (dy / 2.0) - (fract(u_pixel_offset.y) * pixel_height);

        texture_color = texture2D(u_texture, highp vec2(pixel_x, pixel_y)).rgba;
    } else {
        texture_color = texture2D(u_texture, coords.st).rgba;                       // If not pixelated, grab initial texture color at the current location
    }


    // ********** Set some variables for use later
    highp vec4 alpha_in = vec4(u_alpha, u_alpha, u_alpha, u_alpha);                 // For adding in existing opacity of object
    highp vec3 frag_rgb = texture_color.rgb;                                        // Save rgb as a vec3 for working with

    // If texture is premultiplied...
    // Remove alpha first, then apply filters, then add it back later
    if (u_premultiplied) frag_rgb /= texture_color.a;


    // ***** NEGATIVE
    if (u_negative) {
        frag_rgb = 1.0 - frag_rgb;
    }

    // ***** GRAYSCALE
    if (u_grayscale) {
        highp float average = 0.2126 * frag_rgb.r + 0.7152 * frag_rgb.g + 0.0722 * frag_rgb.b;
        frag_rgb = highp vec3(average, average, average);
    }

    // ***** HUE / SATURATION ADJUSTMENT
    if (u_hue > 0.0 || (u_saturation > 0.001 || u_saturation < -0.001)) {
        vec3 frag_hsv = rgbToHsv(frag_rgb).xyz;
        frag_hsv.x += u_hue;
        frag_hsv.x =  mod(frag_hsv.x, 1.0);
        frag_hsv.y += u_saturation;
        frag_hsv.y =  clamp(frag_hsv.y, 0.0, 1.0);
        frag_rgb =    hsvToRgb(frag_hsv);
    }

    // ***** CONTRAST / BRIGHTNESS ADJUSTMENT
    frag_rgb.rgb =  ((frag_rgb.rgb - 0.5) * (u_contrast + 0.392) / 0.392) + 0.5;    // Contrast
    frag_rgb.rgb += u_brightness;                                                   // Brightness
    frag_rgb.rgb += u_tint;                                                         // Tint
    frag_rgb.rgb =  clamp(frag_rgb.rgb, 0.0, 1.0);

    // ***** BITRATE ADJUSTMENT (16 bit down to 1 bit)
    if (u_bitrate < 16.0) {
        // ***** Method 1
        highp float bit_depth = pow(2.0, u_bitrate);
        frag_rgb = highp vec3(floor(frag_rgb.r * bit_depth), floor(frag_rgb.g * bit_depth), floor(frag_rgb.b * bit_depth)) / bit_depth;

        // ***** Method 2
        //float num_colors = pow(2.0, u_bitrate);
        //float gamma = 1.5;                                                        // (adjustable)
        //vec3 c = frag_rgb.rgb;
        //c = pow(c, vec3(gamma, gamma, gamma));
        //c = c * num_colors;
        //c = floor(c);
        //c = c / num_colors;
        //c = pow(c, vec3(1.0 / gamma));
        //frag_rgb = c;
    }

    // ***** CARTOON
    if (u_cartoon) {
        vec3 original_color = frag_rgb;
        vec3 v_hsv = cartoonRgbToHsvV(original_color.r, original_color.g, original_color.b);
        v_hsv.x = 30.0 * (floor(v_hsv.x / 30.0) + 1.0);
        v_hsv.y =  0.1 * (floor(v_hsv.y / 0.1) +  1.0);
        v_hsv.z =  0.1 * (floor(v_hsv.z / 0.1) +  1.0);
        float edg = isEdge(coords.xy);
        vec3 v_rgb = (edg >= edge_thres) ? vec3(0.0, 0.0, 0.0) : cartoonHsvToRgb(v_hsv.x, v_hsv.y, v_hsv.z);
        frag_rgb = vec3(v_rgb.x, v_rgb.y, v_rgb.z);
    }

    // If texture is premultiplied, add back alpha
    if (u_premultiplied) frag_rgb *= texture_color.a;

    gl_FragColor = highp vec4(frag_rgb, texture_color.a) * alpha_in;

}













