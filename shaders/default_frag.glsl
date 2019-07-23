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
uniform sampler2D   u_texture_displacement;         // Refraction Texture
uniform sampler2D   u_texture_water;                // Water Texture

uniform lowp  float u_alpha;                        // Opacity
uniform lowp  vec3  u_tint;// = vec3(0, 0, 0);      // Tint, adds rgb to final output
uniform lowp  float u_zoom;                         // Current zoom level
uniform lowp  vec3  u_position;                     // Current camera position

uniform highp float u_width;                        // Texture Width
uniform highp float u_height;                       // Texture Height
uniform highp float u_time;                         // Time in seconds
uniform       bool  u_premultiplied;                // True if the texture we are using has premultiplied alphas (affects negative)

// EXACT SAME ORDER AS IN EDITOR!!!!!
uniform lowp  float u_pixel_x;// = 1.0;             // Pixel Width X    1.0 Normal, 4.0 is nice pixelation
uniform lowp  float u_pixel_y;// = 1.0;             // Pixel Width Y    1.0 Normal, 4.0 is nice pixelation
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
    float dxtex = 1.0 / float(u_width);     //textureSize(u_texture, 0)) ;
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


// Random usued for frosted glass
float rand(vec2 co) {
    vec2 v1 = vec2(92.0, 80.0);
    vec2 v2 = vec2(41.0, 62.0);
    float rnd_scale = 5.1;
    return fract(sin(dot(co.xy ,v1)) + cos(dot(co.xy ,v2)) * rnd_scale);
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
//        time = 100.0;                           // !!! Disables imported time (turns off animation)
//        vec2  tc =  coords.xy;
//        vec2  p =   -1.0 + 2.0 * tc;
//        float len = length(p);
//        coords = tc + (p / len) * cos(len*12.0 - time*4.0) * 0.03;
    }


    // ***** 2D Shockwave
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




    // ***** Water
    if (u_wavy) {
        float y_start = 0.4;                                // 0.0 is bottom, 1.0 is top

        // ***** Water Input Variables
        vec3  overlay_color =       vec3(0.3, 0.3, 1.0);    // light blue
        float color_tint =          0.35;
        float reflection_opacity =  0.50;                   // 0.25 is nice
        float water_opacity =       0.80;                   // 0.85 is nice

        float wave_length =    pow( 5.0, 2.0);              // Vertical wave length                 0.0 to  20.0    good = 5 big wave, 15 small ripples
        float wave_speed =          5.0;                    // Vertical wave speed                  0.0 to  10.0    good = 1 big wave,  5 small ripples

        float wave_min_width_x =    0.05;                   // Starting vertical wave width         0.0 to   1.0    good =  0.25
        float wave_stretch_x =      0.25;                   // Stretches width away from the start  0.0 to  10.0    good =  3.00

        float wave_height =         0.1;                    // Horizontal wave length               0.0 to   1.0    good =  0.25
        float peak_distance =   pow(1.5, 2.0);              // Lower is further apart               0.0 to  20.0    good =  5.00

        float bob_speed =           3.0;                    // Between 0.0 and 5.0                                  good =  2.00
        float bob_amount =          5.0;                    // Between 0.0 and 50.0                                 good =  2.00

        // Refraction amounts on the different textures
        float refract_reflection =    5.0 * u_zoom;
        float refract_underwater =    5.0 * u_zoom;
        float refract_texture    =   25.0 * u_zoom;
        float refract_top_of_water =  3.0 * u_zoom;

        // ----- End Water Input Variables


        // Calculate some position and scaling values
        float shrink_texture = 2.0;                                                 // 1.0 = normal size, 4.0 equals 1/4 size
        float player_x = u_position.x*0.00082 * (1.0/u_zoom);
        float player_y = u_position.y*0.00082 * (1.0/u_zoom);
        float diff_w = (u_width*(1.0/u_zoom))  / 1200.0;
        float diff_h = (u_height*(1.0/u_zoom)) / 1200.0;

        // Grab value from 2D Water Normal Texture, use it to get refraction values
        vec3  displacement = texture2D(u_texture_displacement,
                                       vec2(( (coords.x*diff_w)/u_zoom + player_x + time/50.0),
                                            ( (coords.y*diff_h)/u_zoom + player_y)) / (shrink_texture/u_zoom) ).rgb;
        float refract_x = (displacement.x - displacement.y)*0.01;
        float refract_y = (displacement.y - displacement.x)*0.01;



        // Calculates vertical waves
        float xoffset = cos(time*wave_speed + wave_length * coords.y) * (wave_min_width_x + (y_start - coords.y) * wave_stretch_x) * 0.005;
              //xoffset -= refract_x;// -= ((5.0*u_zoom)/u_width) ; //(u_zoom/(u_width/6.0));

        // Calculates horizontal waves
        float bob = sin(time*bob_speed + coords.x+refract_x*refract_reflection) * bob_amount * u_zoom;
        float yoffset = sin( (coords.x*diff_w*(1.0/u_zoom) + player_x) * (peak_distance/u_zoom) + time ) * (wave_height*0.01*u_zoom);
        float y_top = y_start - abs(refract_y)*refract_top_of_water + yoffset*bob;

        // Grab the reflected value from existing screen
        vec4 reflection, water, original;
        reflection = texture2D(u_texture, vec2(              coords.x + refract_x*refract_reflection - xoffset,
                                               2.0*y_start - coords.y - refract_y*refract_reflection + yoffset*bob) );
        // Simple reflection
        //reflection = texture2D(u_texture, vec2(coords.x, ((2.0 * y_start) - coords.y)));


        // If we are above offset, just pass pixel color through as it is above the top of the wave
        if (coords.y > y_top) {
            original = texture2D(u_texture, vec2(coords.x, coords.y));

        // Otherwise Refract Original Pixel Color, mix in Overlay Color
        } else {
            // Existing pixel color refracted through water
            original = texture2D(u_texture, vec2(coords.x + refract_x*refract_underwater + xoffset,
                                                 coords.y - refract_y*refract_underwater + yoffset));

            // Mix in overlay_color and water texture
            float bob_texture =   10.0;                                                 // Increases bob effect on water texture
                  bob_texture *= bob*refract_x*refract_texture;
            water = texture2D(u_texture_water, vec2(
                        (coords.x*diff_w*(1.0/u_zoom) + refract_x*refract_texture + player_x + xoffset), // + (time/50.0),      // gives movement
                        (coords.y*diff_h*(1.0/u_zoom) + refract_y*refract_texture + player_y + yoffset*bob_texture)) * shrink_texture );
            water =    vec4(mix(water.rgb,  overlay_color,  color_tint),          1.0);
            original =      mix(original,   water,          water_opacity);
            original =      mix(original,   reflection,     reflection_opacity);
        }

        gl_FragColor = original;
        return;
    }

    // ***** FISHEYE
    if (u_fisheye) {
        float lens_size = 0.4;                  // lens size
        float x_pos = 0.5;
        float y_pos = 0.5;

        vec2  p = coords.xy;                    // current texture coordinate
        vec2  m = vec2(x_pos, 1.0 - y_pos);     // lens coordinate
        vec2  d = p - m;
        float r = sqrt(dot(d, d));              // distance of pixel from m
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
        highp float dx = u_pixel_x * (1.0 / (u_width)) * 0.99;                      // 99 Percent modifier is more like the function in Image_Filter_Color.cpp
        highp float dy = u_pixel_y * (1.0 / (u_height)) * 0.99;                     // 99 Percent modifier is more like the function in Image_Filter_Color.cpp

        highp float pixel_x = dx * floor(coords.x / dx) + (dx / 2.0);
        highp float pixel_y = dy * floor(coords.y / dy) + (dy / 2.0);
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
        highp float bit_depth = pow(2.0, u_bitrate);
        frag_rgb = highp vec3(floor(frag_rgb.r * bit_depth), floor(frag_rgb.g * bit_depth), floor(frag_rgb.b * bit_depth)) / bit_depth;

        // ***** Alternate Method
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













