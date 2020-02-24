// version 120
#ifdef GL_ES
precision highp float;
#endif
//####################################################################################
//##
//##    Main Fragment Shader
//##
//####################################################################################


// ***** Input from Vertex Shader
varying highp   vec2    coordinates;                        // Texture Coodinates
varying highp   vec3    vert;                               // Current vertex
varying highp   vec3    vert_normal;                        // Current vertex normal
varying highp   vec3    vert_bary;                          // Current vertex barycentric coordinate (used for wireframe)

// ***** Input from Engine
uniform sampler2D       u_texture;                          // Texture
uniform sampler2D       u_texture_pixel;                    // Texture for Pixel Shading... Stiches, Tiles, etc

uniform mediump float   u_alpha;                            // Opacity
uniform mediump vec3    u_average_color;                    // Average color of texture
uniform mediump vec3    u_tint;// = vec3(0, 0, 0);          // Tint, adds rgb to final output
uniform highp   float   u_zoom;                             // Zoom factor

uniform highp   float   u_width;                            // Texture Width
uniform highp   float   u_height;                           // Texture Height
uniform highp   float   u_time;                             // Time in seconds
uniform         bool    u_premultiplied;                    // True if the texture we are using has premultiplied alphas (affects negative)

// EXACT SAME ORDER AS IN EDITOR!!!!!
uniform highp   float   u_pixel_x;// = 1.0;                 // Pixel Width X    1.0 Normal, 4.0 is nice pixelation
uniform highp   float   u_pixel_y;// = 1.0;                 // Pixel Width Y    1.0 Normal, 4.0 is nice pixelation
uniform highp   vec2    u_pixel_offset;                     // Used to offset pixelation to reduce pixel flicker
uniform highp   float   u_pixel_type;                       // Which pixelation texture to use, 0 = none, 1 = kintted, 2 = woven
uniform         bool    u_negative;// = false;              // Negative         True / False
uniform         bool    u_grayscale;// = false;             // Grayscale        True / False
uniform mediump float   u_hue;// = 0.0;                     // Hue              Editor:    0 to 360     Shader:  0.0 to 1.0
uniform mediump float   u_saturation;// = 0.0;              // Saturation       Editor: -255 to 255     Shader: -1.0 to 1.0
uniform mediump float   u_contrast;// = 0.0;                // Contrast         Editor: -255 to 255     Shader: -1.0 to 1.0
uniform mediump float   u_brightness;// = 0.0;              // Brightness       Editor: -255 to 255     Shader: -1.0 to 1.0

uniform         bool    u_shade_away;
uniform highp   vec3    u_camera_pos;

uniform mediump float   u_bitrate;// = 256;                 // Bitrate              Editor:    0 to  256
uniform         bool    u_cartoon;// = false;               // Cartoon              True / False
uniform mediump float   u_cartoon_width;// = 5.0            // Cartoon Edge Width   0.0 to ?
uniform         bool    u_cross_hatch;//false               // Cross Hatch          True / False
uniform mediump float   u_cross_hatch_width;// = 5.0        // Cross Hatch Width    0.0 to ?
uniform         bool    u_wavy;// = false;                  // Wavy                 True / False
uniform         bool    u_wireframe;// = false;             // Wireframe            True / False
uniform mediump float   u_wireframe_width;// = 1.0          // Wireframe Width      0.0 to ? (1.0 is good, 6.0 is fat)

// Constants
const   highp   float   THRESHOLD = 0.75;                   // Alpha threshold for our occlusion map
const   highp   float   PI =  3.14159;                      // Pi
const   highp   float   RAD = 6.2831853;                    // 2.0 * PI is 360 degrees in radians
const   highp   float   PI180 = PI / 180.0;                 // To convert Degrees to Radians

// Pixel Textures
const           float   TextureNone         = 0.0;
const           float   TextureAscii        = 1.0;
const           float   TextureBrick        = 2.0;
const           float   TextureTile         = 3.0;
const           float   TextureCross        = 4.0;
const           float   TextureGridBlocks   = 5.0;
const           float   TextureGridLines    = 6.0;
const           float   TextureKnitted      = 7.0;
const           float   TextureWoven        = 8.0;
const           float   TextureWood         = 9.0;


//####################################################################################
//##    Fast Rgb / Hsv Conversion Functions
//####################################################################################
// ***** Convert red/green/blue to hue/saturation/vibrance
vec3 rgbToHsv(vec3 c) {
    vec4  K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4  p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4  q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// ***** Convert hue/saturation/vibrance to red/green/blue
vec3 hsvToRgb(vec3 c) {
    vec4  K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3  p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


//####################################################################################
//##    Calculates how close point is to edge of triangle (used for wirefame)
//####################################################################################
float edgeFactor(float width) {
    vec3  d =  fwidth(vert_bary);
    vec3  a3 = smoothstep(vec3(0.0), d * width * sqrt(u_zoom), vert_bary);
    return min(min(a3.x, a3.y), a3.z);
}


//####################################################################################
//##    Cartoon Filter Functions
//####################################################################################
float edge_thres  = 0.15;
float edge_thres2 = u_cartoon_width;//5.0;

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
    delta = (abs(pix[1] - pix[7]) + abs(pix[5] - pix[3]) + abs(pix[0] - pix[8]) + abs(pix[2] - pix[6]) ) / 4.0;
    return clamp(edge_thres2 * delta, 0.0, 1.0);
}


//####################################################################################
//##    Pseudo-random number, that is between 0.0 and 0.999999 inclusive
//####################################################################################
float random (vec2 xy) {
    return fract(sin(dot(xy, vec2(11.2345, 81.456))) * 42758.12);
}


//####################################################################################
//##    2D Rotation / Translation
//####################################################################################
vec2 translate(vec2 v, float x, float y) {
    v.x = v.x + x;
    v.y = v.y + y;
    return v;
}

vec2 rotate(vec2 v, vec2 center_point, float angle) {
    v = translate(v, -(center_point.x), -(center_point.y));         // Translate to center point
    v.x *= (u_width / u_height);                                    // Account for texture ratio
    float a = angle * PI180;                                        // Convert to radians
    float s = sin(a);
    float c = cos(a);

    mat2 m = mat2(c, s, -s, c);
    v = m * v;

    v.x /= (u_width / u_height);                                    // Account for texture ratio
    v = translate(v,  (center_point.x),  (center_point.y));         // Remove center translation
    return v;
}



//####################################################################################
//##    Main Shader Function
//####################################################################################
void main( void ) {

    // Keeps colors through this shader
    highp vec4  texture_color;
    highp vec4  final_color;

    // Move coordinates into a vec2 that is not read-only
    highp vec2  coords = coordinates.st;
    highp float time   = u_time;


    // ***** WAVY
    if (u_wavy) {
        //time = 100.0;                                         // !!! Disables imported time (turns off animation)
        vec2  tc =  coords.xy;
        vec2  p =   -1.0 + 2.0 * tc;
        float len = length(p);
        coords = tc + (p / len) * cos(len*12.0 - time*4.0) * 0.03;
    }


    // ***** 2D SHOCKWAVE
//    if (u_wavy) {
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
//    }


    // ***** PIXELATED    
    highp float pattern_x = 0.0;
    highp float pattern_y = 0.0;
    if (u_pixel_x > 1.0 || u_pixel_y > 1.0) {
        // Pixelation Variables
        highp float pix_size_x =    u_pixel_x;// * u_zoom;
        highp float pix_size_y =    u_pixel_y;// * u_zoom;
        highp float pixel_width =   1.0 / u_width;
        highp float pixel_height =  1.0 / u_height;

        // Texture Spanning (For textures that require more than 1 "pixel" to show texture)
        highp float spread =    1.0;
        highp float x_offset =  0.0;
        highp float y_offset =  0.0;
        if      (u_pixel_type == TextureBrick) spread = 2.0;
        else if (u_pixel_type == TextureWoven) spread = 2.0;
        else if (u_pixel_type == TextureWood)  spread = 5.0;

        // Calculate Current Color
        if (spread > 1.0) {
            x_offset = (-mod(u_pixel_offset.x * u_zoom, pix_size_x*spread) - (pix_size_x*u_zoom/spread) ) * pixel_width;
            y_offset = ( mod(u_pixel_offset.y * u_zoom, pix_size_y*spread) - (pix_size_y*u_zoom/spread) ) * pixel_height;
        } else {
            x_offset = (-mod(u_pixel_offset.x * u_zoom, pix_size_x) - (pix_size_x/2.0) ) * pixel_width;
            y_offset = ( mod(u_pixel_offset.y * u_zoom, pix_size_y) - (pix_size_y/2.0) ) * pixel_height;
        }
        // For pixelation using shrunken fbo's:
        //x_offset = 0.0;
        //y_offset = 0.0;

        highp float real_pixel_x =        (coords.x  * u_width );
        highp float real_pixel_y = ((1.0 - coords.y) * u_height);
        highp float pixel_x =       (x_offset + ((pix_size_x * floor(real_pixel_x / pix_size_x) + pix_size_x) * pixel_width));
        highp float pixel_y = 1.0 - (y_offset + ((pix_size_y * floor(real_pixel_y / pix_size_y) + pix_size_y) * pixel_height));

        // Adjust Pattern spread "pixels" to between 0.0 to 1.0
        highp float relative_x = mod(coords.x * u_width, pix_size_x*spread) / (pix_size_x*spread);
        highp float relative_y = (pix_size_y - (mod((1.0 - coords.y) * u_height, pix_size_y*spread))) / (pix_size_y*spread);

        // ***** Ceramic Tile, Cross Stitch, Wood Blocks
        pattern_x = (relative_x);// / 1.0;
        pattern_y = (relative_y);// / 1.0;

        // ***** Stitching
        if (u_pixel_type == TextureKnitted) {
            pattern_x = (relative_x)         / 2.0;         // 2.0 == 2 stitches across
            pattern_y = (relative_y - 0.625) / 3.0;         // 3.0 == 3 stitches up and down

            // Left Half
            if (relative_x < 0.5) {
                // Top of Stitches (heart humps)
                if (relative_y < 0.55) {
                    if (relative_y*0.5 < ((0.5 - relative_x) - 0.1)) {
                        pixel_y -= (pix_size_y * pixel_height);
                    }
                // Bottom of tip
                } else if (relative_y + relative_x*2.0 > 1.8) {
                    pixel_y += (pix_size_y * pixel_height);
                }

            // Right Half
            } else {
                // Top of Stitches (heart humps)
                if (relative_y < 0.55) {
                    if (relative_y*0.5 < ((relative_x - 0.5) - 0.1)) {
                        pixel_y -= (pix_size_y * pixel_height);
                    }
                // Bottom of tip
                } else if (relative_y + (1.0 - relative_x)*2.0 > 1.8) {
                    pixel_y += (pix_size_y * pixel_height);
                }
            }

        // ***** Woven
        } else if (u_pixel_type == TextureWoven) {
            // Adjust 2 "pixels" to between 0.0 to 1.0
            relative_x = mod(coords.x * u_width, pix_size_x*spread) / (pix_size_x*spread);
            relative_y = (pix_size_y*2.0 - (mod((1.0 - coords.y) * u_height, pix_size_y*spread))) / (pix_size_y*spread);
            pattern_x = (relative_x) / 1.0;                 // 1.0 == 2 stitches spread across two pixels across
            pattern_y = (relative_y) / 1.0;                 // 1.0 == 2 stitches spread across two pixels up and down

            if (relative_y > 0.5) {
                // Upper Left Stitch
                if (relative_x < 0.5) {
                    if (relative_x < 0.043) pixel_x -= (pix_size_x * pixel_width);
                    if (relative_x > 0.475) pixel_x += (pix_size_x * pixel_width);
                // Upper Right Stich
                } else {
                    if (relative_y > 0.955) pixel_y += (pix_size_y * pixel_height);
                    if (relative_y < 0.530) pixel_y -= (pix_size_y * pixel_height);
                }
            } else {
                // Lower Right Stitch
                if (relative_x > 0.5) {
                    if (relative_x < 0.530) pixel_x -= (pix_size_x * pixel_width);
                    if (relative_x > 0.965) pixel_x += (pix_size_x * pixel_width);
                // Lower Left Stich
                } else {
                    if (relative_y > 0.470) pixel_y += (pix_size_y * pixel_height);
                    if (relative_y < 0.030) pixel_y -= (pix_size_y * pixel_height);
                }
            }

        // ***** Brick
        } else if (u_pixel_type == TextureBrick) {
            // Adjust 2 "pixels" to between 0.0 to 1.0
            relative_x = mod(coords.x * u_width, pix_size_x*spread) / (pix_size_x*spread);
            relative_y = (pix_size_y*2.0 - (mod((1.0 - coords.y) * u_height, pix_size_y*spread))) / (pix_size_y*spread);
            pattern_x = (relative_x) / 1.0;
            pattern_y = (relative_y) / 1.0;
        }

        // Pixelation Color
        vec4 p1 = texture2D(u_texture, vec2(pixel_x, pixel_y)).rgba;
        vec4 p2 = texture2D(u_texture, vec2(pixel_x + pixel_width, pixel_y)).rgba;
        vec4 p3 = texture2D(u_texture, vec2(pixel_x - pixel_width, pixel_y)).rgba;
        vec4 p4 = texture2D(u_texture, vec2(pixel_x, pixel_y + pixel_height)).rgba;
        vec4 p5 = texture2D(u_texture, vec2(pixel_x, pixel_y - pixel_height)).rgba;
        texture_color = (p1 + p2 + p3 + p4 + p5) / 5.0;

        // ***** Ascii
        if (u_pixel_type == TextureAscii) {
            // Font texture is 256x256
            // Original 8 x 6 Characters
            /**
            // 8 characters wide == 32 pixels width  per character (256 pixels total)
            // 6 characters tall == 40 pixels height per character (240 pixels total)
            float cx_offset = (-mod(u_pixel_offset.x*u_zoom, pix_size_x));
            float cy_offset = ( mod(u_pixel_offset.y*u_zoom, pix_size_y));
            float nx = floor((real_pixel_x + u_pixel_offset.x*u_zoom + cx_offset) / (pix_size_x));      // X Coordinate for Character
            float ny = floor((real_pixel_y - u_pixel_offset.y*u_zoom + cy_offset) / (pix_size_y));      // Y Coordinate for Character
            float r1 = random(vec2( (ny*ny) - nx, (ny*nx) - nx ));
            float r2 = random(vec2( (nx*nx) - ny, (ny*ny) + ny ));
            float rx = (floor(r1 * 8.0));
            float ry = (floor(r2 * 6.0));
            float char_pixel =  (1.0 / 256.0);
            float char_width  = char_pixel * 32.0;
            float char_height = char_pixel * 40.0;
            pattern_x = (char_width  * rx) + (relative_x * char_width);
            pattern_y = (char_height * ry) + (relative_y * char_height);// + (char_pixel * 16.0);
            */

            // Font texture is 256x256
            // Newer 16 x 12 Characters
            // 16 characters wide == 16 pixels width  per character (256 pixels total)
            // 12 characters tall == 20 pixels height per character (240 pixels total)
            float lumens = (0.35*texture_color.r + 0.65*texture_color.g + 0.25*texture_color.b);        // Luminosity formula
                  lumens = clamp(lumens, 0.0, 1.0);
            //float lumens =    max(max(texture_color.r, texture_color.g), texture_color.b);            // Luminosity by max red / green / blue value
            //float lumens =    rgbToHsv(texture_color.rgb).b;                                          // Luminosity by hsv value
            float row_start = floor(lumens * 10.999) - 2.0;                                             // Split characters up by 8 starts by luminosity,
                  row_start = clamp(row_start, 0.0, 8.0);                                               // Each row can use characters from next 4 higher rows

            float cx_offset = (-mod(u_pixel_offset.x*u_zoom, pix_size_x));
            float cy_offset = ( mod(u_pixel_offset.y*u_zoom, pix_size_y));
            float nx = floor((real_pixel_x + u_pixel_offset.x + cx_offset) / (pix_size_x));      // X Coordinate for Character
            float ny = floor((real_pixel_y - u_pixel_offset.y + cy_offset) / (pix_size_y));      // Y Coordinate for Character
            float r1 = random(vec2( (ny*ny) - nx, (ny*nx) - nx ));
            float r2 = random(vec2( (nx*nx) - ny, (ny*ny) + ny ));
            float rx = (floor(r1 * 16.0));                                                              // 16 columns
            float ry = (floor(r2 *  4.0));                                                              // Was 12 (12 rows), but now 4 (4 rows for each luminosity)

            float char_pixel =  (1.0 / 256.0);
            float char_width  = char_pixel * 16.0;
            float char_height = char_pixel * 20.0;
            pattern_x = (char_width  * rx) + (relative_x * char_width);
            pattern_y = (char_height * ry) + (char_height * row_start) + (relative_y * char_height);
        }

    // If not pixelated, grab initial texture color at the current location
    } else {
        texture_color = texture2D(u_texture, coords.st).rgba;
    }



    // ********** INITIALIZE COLOR VARIABLES
    highp vec4 alpha_in = vec4(u_alpha, u_alpha, u_alpha, u_alpha);                 // For adding in existing opacity of object
    highp vec3 frag_rgb = texture_color.rgb;                                        // Save rgb as a vec3 for working with


    // If texture is premultiplied...
    // Remove alpha first, then apply filters, then add it back later
    if (u_premultiplied) frag_rgb /= texture_color.a;


    // If 3D, add in average color, set pixel to opaque
    if (u_shade_away) {
        if (u_alpha < 0.05) discard;
        frag_rgb = mix(u_average_color, frag_rgb, texture_color.a);
        texture_color.a = 1.0;
    } else {
        // Don't draw fragment to depth buffer if 2D and mostly invisible
        if ((texture_color.a * u_alpha) < 0.05) discard;
    }


    // ***** NEGATIVE
    if (u_negative) {
        frag_rgb = 1.0 - frag_rgb;
    }

    // ***** GRAYSCALE
    if (u_grayscale) {
        highp float average = 0.2126 * frag_rgb.r + 0.7152 * frag_rgb.g + 0.0722 * frag_rgb.b;
        frag_rgb = vec3(average, average, average);
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

    // ***** BITRATE ADJUSTMENT (16 bit (pow(2.0, 16.0) == 256.0) down to 1 bit)
    if (u_bitrate < 256.0) {
        // ***** Method 1
        highp float bit_depth = u_bitrate;
        frag_rgb = vec3(floor(frag_rgb.r * bit_depth), floor(frag_rgb.g * bit_depth), floor(frag_rgb.b * bit_depth)) / bit_depth;

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
        vec3 v_hsv = rgbToHsv(original_color.rgb);
        v_hsv.x = clamp(0.05 * (floor(v_hsv.x /  0.05) + 1.0), 0.0, 1.0);
        v_hsv.y = clamp(0.05 * (floor(v_hsv.y /  0.05) + 1.0), 0.0, 1.0);
        v_hsv.z = clamp(0.05 * (floor(v_hsv.z /  0.05) + 2.0), 0.0, 1.0);
        float edg = isEdge(coords.xy);
        vec3 v_rgb = (edg >= edge_thres) ? vec3(0.0) : hsvToRgb(v_hsv.xyz);
        frag_rgb = vec3(v_rgb.x, v_rgb.y, v_rgb.z);
    }


    // ***** CROSS HATCH
    if (u_cross_hatch) {
        float hatch_width =    u_cross_hatch_width;
        float hatch_y_offset = hatch_width / 2.0;//5.0;//u_cross_hatch_width;
        float lum_threshold_1 = 1.05;
        float lum_threshold_2 = 0.8;
        float lum_threshold_3 = 0.55;
        float lum_threshold_4 = 0.3;
        float lum = length(frag_rgb);
        vec3 tc = vec3(1.0, 1.0, 1.0);
        if (lum < lum_threshold_1) {
            if (mod(gl_FragCoord.x + gl_FragCoord.y, hatch_width) == 0.0) tc = vec3(0.0, 0.0, 0.0);
        }
        if (lum < lum_threshold_2) {
            if (mod(gl_FragCoord.x - gl_FragCoord.y, hatch_width) == 0.0) tc = vec3(0.0, 0.0, 0.0);
        }
        if (lum < lum_threshold_3) {
            if (mod(gl_FragCoord.x + gl_FragCoord.y - hatch_y_offset, hatch_width) == 0.0) tc = vec3(0.0, 0.0, 0.0);
        }
        if (lum < lum_threshold_4) {
            if (mod(gl_FragCoord.x - gl_FragCoord.y - hatch_y_offset, hatch_width) == 0.0) tc = vec3(0.0, 0.0, 0.0);
        }
        frag_rgb = tc;
    }


    // ***** If texture is premultiplied, add back alpha
    if (u_premultiplied) frag_rgb *= texture_color.a;


    // ***** If triangle is facing away from camera, darken it, and don't use transparent pixels for the extruded object
    if (u_shade_away) {
        highp float dp = dot(normalize(vert_normal), normalize(vert - u_camera_pos)) + 0.15;
                    dp = clamp(dp, 0.0, 1.0);
        frag_rgb = mix(vec3(0.0), frag_rgb, dp);
        //final_color = vec4(frag_rgb, 1.0) * alpha_in;
        final_color = vec4(frag_rgb, texture_color.a) * alpha_in;

    // ***** Otherwise we're drawing image in 2D and we do want transparent borders
    } else {
        final_color = vec4(frag_rgb, texture_color.a) * alpha_in;
    }


    // ***** WIREFRAME
    if (u_wireframe) {
        float wireframe_percent = 0.2;
        final_color *= (1.0 - edgeFactor(u_wireframe_width));

        // If not on edge, draw texture faded, or just maybe just discard
        if ( all(lessThan(final_color, vec4(0.02))) ) {
            // Texture is slightly there
            final_color = vec4(frag_rgb * 0.15, 0.15) * alpha_in;
            // Or gone completely
            //discard;
        }
    }


    // ***** PATTERN COLOR (Ascii, Stitch, Woven, Wood, etc)
    if (u_pixel_type != TextureNone) {
        vec4 pattern_color;
        if (u_pixel_type == TextureGridBlocks || u_pixel_type == TextureGridLines) {
            // Round off pattern_x/_y for smoothness
            pattern_x = clamp(floor((pattern_x * 100.0) + 0.5) / 100.0, 0.0, 1.0);
            pattern_y = clamp(floor((pattern_y * 100.0) + 0.5) / 100.0, 0.0, 1.0);
            // Check that pixel is inside grid or not
            float x_min = step(0.10,       pattern_x);
            float x_max = step(0.10, 1.0 - pattern_x);
            float y_min = step(0.10,       pattern_y);
            float y_max = step(0.10, 1.0 - pattern_y);
            pattern_color = vec4(floor((x_min + x_max + y_min + y_max) / 4.0));
            if (u_pixel_type == TextureGridLines) pattern_color = 1.0 - pattern_color;
            final_color.rgba = mix(vec4(0.0), final_color.rgba, pattern_color.r);
        } else {
            pattern_color = texture2D(u_texture_pixel, vec2(pattern_x, pattern_y)).rgba;
            final_color.rgb  = mix(vec3(0.0), final_color.rgb,  pattern_color.r);
        }
        // Tint Stitch Design Into Image
        //final_color += (stitch_color * 0.10);
        //final_color /= 1.10;
    }


    // ***** FINAL COLOR TO SCREEN
    gl_FragColor = final_color;
}













