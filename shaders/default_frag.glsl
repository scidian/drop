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
uniform sampler2D   u_tex;                          // Texture
uniform lowp float  u_alpha;                        // Opacity

uniform lowp float  u_width;                        // Width of gl window
uniform lowp float  u_height;                       // Height of gl window

// EXACT SAME ORDER AS IN EDITOR!!!!!
uniform lowp float  u_pixel_x;// = 1.0;             // Pixel Width X    1.0 Normal, 4.0 is nice pixelation
uniform lowp float  u_pixel_y;// = 1.0;             // Pixel Width Y    1.0 Normal, 4.0 is nice pixelation
uniform      bool   u_negative;// = false;          // Negative         True / False
uniform      bool   u_grayscale;// = false;         // Grayscale        True / False
uniform lowp float  u_hue;// = 0.0;                 // Hue              Editor:    0 to 360     Shader:  0.0 to 1.0
uniform lowp float  u_saturation;// = 0.0;          // Saturation       Editor: -255 to 255     Shader: -1.0 to 1.0
uniform lowp float  u_contrast;// = 0.0;            // Contrast         Editor: -255 to 255     Shader: -1.0 to 1.0
uniform lowp float  u_brightness;// = 0.0;          // Brightness       Editor: -255 to 255     Shader: -1.0 to 1.0

uniform lowp float  u_bitrate;// = 256;             // Bitrate          Editor:    1 to  16
uniform lowp vec3   u_tint;// = vec3(0, 0, 0);      // Tint, adds rgb to final output
uniform      bool   u_kernel;// = false;            // Kernel Effects? (blur, sharpen, etc)


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
float IsEdge(in vec2 coords) {
    float dxtex = 1.0 / float(u_width);   //textureSize(u_tex, 0)) ;
    float dytex = 1.0 / float(u_height);  //textureSize(u_tex, 0));
    float pix[9];
    int   k = -1;
    float delta;

    // Read neighboring pixel intensities
    for (int i = -1; i < 2; i++) {
        for(int j = -1; j < 2; j++) {
            k++;
            pix[k] = avgIntensity( texture2D(u_tex, coords + vec2(float(i) * dxtex, float(j) * dytex)) );
        }
    }

    // Average color differences around neighboring pixels
    delta = (abs(pix[1] - pix[7]) +
             abs(pix[5] - pix[3]) +
             abs(pix[0] - pix[8]) +
             abs(pix[2] - pix[6]) ) / 4.0;

    return clamp(edge_thres2 * delta, 0.0, 1.0);
}

vec3 RGBtoHSV(float r, float g, float b) {
    float minv, maxv, delta;
    vec3 res;

    minv = min(min(r, g), b);
    maxv = max(max(r, g), b);
    res.z = maxv;                           // v
    delta = maxv - minv;

    if( maxv != 0.0 )
        res.y = delta / maxv;               // s
    else {
        // r = g = b = 0                    // s = 0, v is undefined
        res.y = 0.0;
        res.x = -1.0;
        return res;
    }

    if ( r == maxv )
        res.x = ( g - b ) / delta;          // between yellow & magenta
    else if( g == maxv )
        res.x = 2.0 + ( b - r ) / delta;    // between cyan & yellow
    else
        res.x = 4.0 + ( r - g ) / delta;    // between magenta & cyan

    res.x = res.x * 60.0;                   // degrees
    if( res.x < 0.0 )
        res.x = res.x + 360.0;

    return res;
}

vec3 HSVtoRGB(float h, float s, float v ) {
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

    // ***** PIXELATED
    vec4 texture_color;
    if (u_pixel_x > 1.0 || u_pixel_y > 1.0) {       
        float dx = u_pixel_x * 0.99 * (1.0 / u_width);          // 99 Percent is slight modifier found to be more like the function in Image_Filter_Color.cpp
        float dy = u_pixel_y * 0.99 * (1.0 / u_height);         // 99 Percent is slight modifier found to be more like the function in Image_Filter_Color.cpp
        vec2 coord = coordinates.st;
             coord = vec2(dx * floor(coord.x / dx) + (dx / 2.0), dy * floor(coord.y / dy) + (dy / 2.0));
        texture_color = texture2D(u_tex, coord).rgba;
    } else {
        texture_color = texture2D(u_tex, coordinates.st).rgba;                      // If not pixelated, grab initial texture color at the current location
    }

    // Set some variables for use later
    vec4 alpha_in = vec4(u_alpha, u_alpha, u_alpha, u_alpha);                       // For adding in existing opacity of object
    vec3 fragRGB = texture_color.rgb;                                               // Save rgb as a vec3 for working with

    // ***** NEGATIVE
    if (u_negative) {
        //fragRGB = 1.0 - fragRGB;


        vec3 colorOrg = fragRGB;
        vec3 vHSV = RGBtoHSV(colorOrg.r, colorOrg.g, colorOrg.b);
        //vec3 vHSV = rgbToHsv(vec3(colorOrg.r, colorOrg.g, colorOrg.b));
        vHSV.x = 30.0 * (floor(vHSV.x / 30.0) + 1.0);
        vHSV.y =  0.1 * (floor(vHSV.y / 0.1) +  1.0);
        vHSV.z =  0.1 * (floor(vHSV.z / 0.1) +  1.0);
        float edg = IsEdge(coordinates.st);

        vec3 vRGB = (edg >= edge_thres) ? vec3(0.0, 0.0, 0.0) : HSVtoRGB(vHSV.x, vHSV.y, vHSV.z);
        //vec3 vRGB = (edg >= edge_thres) ? vec3(0.0, 0.0, 0.0) : hsvToRgb(vec3(vHSV.x, vHSV.y, vHSV.z));
        fragRGB = vec3(vRGB.x, vRGB.y, vRGB.z);
    }

    // ***** GRAYSCALE
    if (u_grayscale) {
        float average = 0.2126 * fragRGB.r + 0.7152 * fragRGB.g + 0.0722 * fragRGB.b;
        fragRGB = vec3(average, average, average);
    }

    // ***** HUE / SATURATION ADJUSTMENT
    if (u_hue > 0.0 || u_saturation > 0.0) {
        vec3 fragHSV = rgbToHsv(fragRGB).xyz;
        fragHSV.x += u_hue;
        fragHSV.x =  mod(fragHSV.x, 1.0);
        fragHSV.y += u_saturation;
        fragHSV.y =  clamp(fragHSV.y, 0.0, 1.0);
        fragRGB = hsvToRgb(fragHSV);
    }

    // ***** CONTRAST / BRIGHTNESS ADJUSTMENT
    fragRGB.rgb =  ((fragRGB.rgb - 0.5) * (u_contrast + 0.392) / 0.392) + 0.5;      // Contrast
    fragRGB.rgb += u_brightness;                                                    // Brightness
    fragRGB.rgb += u_tint;                                                          // Tint
    fragRGB.rgb =  clamp(fragRGB.rgb, 0.0, 1.0);



    // ***** BITRATE ADJUSTMENT (16 bit down to 1 bit)
    if (u_bitrate < 16.0) {
        float bit_depth = pow(2.0, u_bitrate);
        fragRGB = vec3(floor(fragRGB.r * bit_depth), floor(fragRGB.g * bit_depth), floor(fragRGB.b * bit_depth)) / bit_depth;

        // ***** Alternate Method
        //float numColors = pow(2.0, u_bitrate);
        //float gamma = 1.5;                                                        // (adjustable)
        //vec3 c = fragRGB.rgb;
        //c = pow(c, vec3(gamma, gamma, gamma));
        //c = c * numColors;
        //c = floor(c);
        //c = c / numColors;
        //c = pow(c, vec3(1.0/gamma));
        //fragRGB = c;
    }


    gl_FragColor = vec4( fragRGB, texture_color.a) * alpha_in;


    // ******************** KERNEL EFFECTS
    if (u_kernel) {
        // Temp can use to darken pixels
        //gl_FragColor = gl_FragColor * 0.3;
        //return;

        float offset_x = 1.0 / u_width;
        float offset_y = 1.0 / u_height;
        vec2 offset_0 = vec2(-offset_x,  offset_y);     // top-left
        vec2 offset_1 = vec2( 0.0,       offset_y);     // top-center
        vec2 offset_2 = vec2( offset_x,  offset_y);     // top-right
        vec2 offset_3 = vec2(-offset_x,       0.0);     // center-left
        vec2 offset_4 = vec2( 0.0,            0.0);     // center-center
        vec2 offset_5 = vec2( offset_x,       0.0);     // center-right
        vec2 offset_6 = vec2(-offset_x, -offset_y);     // bottom-left
        vec2 offset_7 = vec2( 0.0,      -offset_y);     // bottom-center
        vec2 offset_8 = vec2( offset_x, -offset_y);     // bottom-right

        // ***** SHARPEN
        //float kernel_0 = -1.0; float kernel_1 = -1.0; float kernel_2 = -1.0;
        //float kernel_3 = -1.0; float kernel_4 =  9.0; float kernel_5 = -1.0;
        //float kernel_6 = -1.0; float kernel_7 = -1.0; float kernel_8 = -1.0;

        // ***** BLUR
        float kernel_0 = 1.0 / 16.0; float kernel_1 = 2.0 / 16.0; float kernel_2 = 1.0 / 16.0;
        float kernel_3 = 2.0 / 16.0; float kernel_4 = 4.0 / 16.0; float kernel_5 = 2.0 / 16.0;
        float kernel_6 = 1.0 / 16.0; float kernel_7 = 2.0 / 16.0; float kernel_8 = 1.0 / 16.0;

        // ***** EDGE DETECTION
        //float kernel_0 = 1.0; float kernel_1 =  1.00; float kernel_2 = 1.0;
        //float kernel_3 = 1.0; float kernel_4 = -8.00; float kernel_5 = 1.0;
        //float kernel_6 = 1.0; float kernel_7 =  1.00; float kernel_8 = 1.0;

        vec4 sample_tex_0 = texture2D(u_tex, coordinates.st + offset_0).rgba;
        vec4 sample_tex_1 = texture2D(u_tex, coordinates.st + offset_1).rgba;
        vec4 sample_tex_2 = texture2D(u_tex, coordinates.st + offset_2).rgba;
        vec4 sample_tex_3 = texture2D(u_tex, coordinates.st + offset_3).rgba;
        vec4 sample_tex_4 = texture2D(u_tex, coordinates.st + offset_4).rgba;
        vec4 sample_tex_5 = texture2D(u_tex, coordinates.st + offset_5).rgba;
        vec4 sample_tex_6 = texture2D(u_tex, coordinates.st + offset_6).rgba;
        vec4 sample_tex_7 = texture2D(u_tex, coordinates.st + offset_7).rgba;
        vec4 sample_tex_8 = texture2D(u_tex, coordinates.st + offset_8).rgba;

        vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
        col += sample_tex_0 * kernel_0;
        col += sample_tex_1 * kernel_1;
        col += sample_tex_2 * kernel_2;
        col += sample_tex_3 * kernel_3;
        col += sample_tex_4 * kernel_4;
        col += sample_tex_5 * kernel_5;
        col += sample_tex_6 * kernel_6;
        col += sample_tex_7 * kernel_7;
        col += sample_tex_8 * kernel_8;

        gl_FragColor = vec4(col.rgb, texture2D( u_tex, coordinates.st ).a * u_alpha);
    }

}


















