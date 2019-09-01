//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef OPENGL_H
#define OPENGL_H

#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <chrono>

#include "enums.h"
#include "enums_engine.h"

// Forward Declarations
class DrEngine;
class DrEngineFire;
class DrEngineFisheye;
class DrEngineLight;
class DrEngineMirror;
class DrEngineObject;
class DrEngineSwirl;
class DrEngineThing;
class DrEngineVertexData;
class DrEngineWater;
class FormEngine;

// Type Definitions
typedef std::chrono::high_resolution_clock Clock;

// Open GL Globals - defined in opengl_initialize.cpp
extern int  g_max_texture_size;
extern int  g_max_rays;
extern int  g_max_occluder_fbo_size;
extern int  g_max_light_fbo_size;

// VBO Constants
#define PROGRAM_VERTEX_ATTRIBUTE    0
#define PROGRAM_TEXCOORD_ATTRIBUTE  1
#define PROGRAM_NORMAL_ATTRIBUTE    2

// Rendering Constants
const bool  c_use_cam_offset =  true;
const float c_near_plane =     -10000.0;
const float c_far_plane =       10000.0;
const float c_field_of_view =   52.5f;                  // Close to Orthographic size when using standard widescreen ratio

// 2D Light Constants
const float c_occluder_scale_ortho =       1.00f;       // Scale to use for occlusion map (higher the number, less shaky the shadows), MUST be 1.0 for now
const float c_occluder_scale_proj =        1.00f;       // Scale to use for occlusion map (higher the number, less shaky the shadows), MUST be 1.0 for now
const int   c_desired_max_rays =           4096;        // Desired max number of rays to send out during shadow map calculations
const int   c_desired_occluder_fbo_size =  8192;        // Desired max width and height of offscreen fbo used for shadow map
const int   c_desired_light_fbo_size =     4096;        // Desired max width and height of offscreen fbo used for lights (and max size of lights themselves)


//####################################################################################
//##    DrOpenGL
//##        A modern OpenGL Widget
//############################
class DrOpenGL : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    FormEngine     *m_form_engine;                              // Pointer to parent FormEngine
    DrEngine       *m_engine;                                   // Pointer to Engine instance that will run in this OpenGLWidget

    // Local Variables
    QMatrix4x4      m_model_view;
    QMatrix4x4      m_projection;

    QVector3D       m_eye;
    QVector3D       m_look_at;
    QVector3D       m_up;

    int             m_zoom =  250;                              // Zoom level of current view, 200 is 50% - 250 is 100%
    float           m_scale = 1.0;                              // Updated in zoomInOut for use during painting grid, DO NOT SET MANUALLY
    float           m_angle =   0;

    float           m_background_red = 0;
    float           m_background_green = 0;
    float           m_background_blue = 0;

    std::vector<float>       m_whole_texture_coordinates;       // Used to keep the coordinates of rendering an entire texture

    // VBO's
    std::map<long, QOpenGLBuffer*>      m_texture_vbos;         // Stores extruded texture vbo's
    std::map<long, DrEngineVertexData*> m_texture_data;         // Stores extruded texture vertex data

    // Frame Buffers
    QOpenGLFramebufferObject *m_render_fbo = nullptr;           // Used for offscreen rendering
    QOpenGLFramebufferObject *m_texture_fbo = nullptr;          // m_render_fbo must be copied to a non-multisampled fbo before being used as a texture
    QOpenGLFramebufferObject *m_glow_fbo = nullptr;             // Used for rendering Glow Lights onto scene all at once
    QOpenGLFramebufferObject *m_occluder_fbo = nullptr;         // Used for rendering an occlusion map for use with lights

    // Frame Buffer Objects for use during Render
    std::map<long, QOpenGLFramebufferObject*> m_occluders;      // Off screen Frame Buffer Objects for Light Occluder Maps
    std::map<long, QOpenGLFramebufferObject*> m_shadows;        // Off screen Frame Buffer Objects for Light 1D Shadow Maps

    // Vectors to keep track of lights
    QVector<DrEngineLight*> m_shadow_lights;
    QVector<DrEngineLight*> m_glow_lights;


public:
    // Constructor / Destructor
    DrOpenGL(QWidget *parent, FormEngine *form_engine, DrEngine *engine);
    ~DrOpenGL() override;

    // Event Overrides
    virtual void    keyPressEvent(QKeyEvent *event) override;
    virtual void    keyReleaseEvent(QKeyEvent *event) override;
    virtual void    mousePressEvent(QMouseEvent *event) override;
    virtual void    mouseReleaseEvent(QMouseEvent *event) override;
    virtual void    mouseMoveEvent(QMouseEvent *event) override;

    // OpenGL Overrides
    virtual void    initializeGL() override;
    virtual void    resizeGL(int w, int h) override;
    virtual void    paintGL() override;
#if QT_CONFIG(wheelevent)
    virtual void    wheelEvent(QWheelEvent *event) override;
#endif

    // Function Calls
    QVector3D       mapFromScreen(double x, double y);
    QVector3D       mapFromScreen(float x, float y);
    QVector3D       mapFromScreen(QPointF point);
    QPointF         mapToScreen(double x, double y, double z);
    QPointF         mapToScreen(float x, float y, float z);
    QPointF         mapToScreen(QVector3D point3D);
    QPointF         mapToFBO(QVector3D point3D, QOpenGLFramebufferObject *fbo, QMatrix4x4 matrix);
    void            zoomInOut(int level);

    // Initialization Calls
    void            importTexture(long texture_id, QString from_asset_string);
    void            importTexture(long texture_id, QPixmap &pixmap);
    void            loadBuiltInTextures();
    void            loadProjectTextures();
    void            loadShaders();

    // Render Calls
    void            bindOffscreenBuffer(bool clear = true);
    void            cullingOn();
    void            cullingOff();
    void            drawDebug(QPainter &painter);
    void            drawDebugCollisions(QPainter &painter);
    void            drawDebugHealth(QPainter &painter);
    void            drawDebugHealthNative(QPainter &painter);
    void            drawDebugJoints(QPainter &painter);
    void            drawDebugShapes(QPainter &painter);
    bool            drawEffect(DrEngineThing *thing, DrThingType &last_thing);
    void            drawFrameBufferUsingDefaultShader(QOpenGLFramebufferObject *fbo);
    bool            drawFrameBufferUsingFisheyeShader(QOpenGLFramebufferObject *fbo, DrEngineFisheye *lens);
    void            drawFrameBufferUsingKernelShader(QOpenGLFramebufferObject *fbo);
    bool            drawFrameBufferUsingMirrorShader(QOpenGLFramebufferObject *fbo, DrEngineMirror *mirror);
    void            drawFrameBufferUsingScreenShader(QOpenGLFramebufferObject *upper, QOpenGLFramebufferObject *lower, Blend_Mode mode);
    bool            drawFrameBufferUsingSwirlShader(QOpenGLFramebufferObject *fbo, DrEngineSwirl *swirl);
    bool            drawFrameBufferUsingWaterShader(QOpenGLFramebufferObject *fbo, DrEngineWater *water);
    bool            drawGlowBuffer();
    void            drawObject(DrEngineThing *thing, DrThingType &last_thing);
    void            drawObjectExtrude(DrEngineThing *thing, DrThingType &last_thing);
    bool            drawObjectFire(DrEngineThing *thing, DrThingType &last_thing);
    bool            drawObjectOccluder(DrEngineThing *thing, bool need_init_shader = true);
    void            drawSpace();
    bool            getEffectPosition(QOpenGLFramebufferObject *fbo, DrEngineThing *thing,
                                      double &top, double &bottom, double &left, double &right, float &angle);
    void            getThingVertices(QVector<GLfloat> &vertices, DrEngineThing *thing, float angle);
    QColor          objectDebugColor(DrEngineObject *object, bool text_color = false);
    QMatrix4x4      occluderMatrix(Render_Type render_type, bool use_offset);
    QMatrix4x4      orthoMatrix(float width, float height);
    void            updateViewMatrix(Render_Type render_type, bool use_offset);
    void            releaseOffscreenBuffer();
    void            setShaderDefaultValues(float texture_width, float texture_height);
    void            setNumberTextureCoordinates(QString letter, std::vector<float> &texture_coordinates);
    void            setQuadVertices(QVector<GLfloat> &vertices, float width, float height, QPointF center, float z);
    void            setQuadRotatedVertices(QVector<GLfloat> &vertices, QVector3D &top_right, QVector3D &top_left,
                                           QVector3D &bot_left, QVector3D &bot_right, QVector3D position);

    // Soft Shadows / Lights
    void            bindGlowLightsBuffer(float ambient_light);
    void            bindOccluderMapBuffer();
    void            bindLightOcculderBuffer(DrEngineLight *light);
    void            bindLightShadowBuffer(DrEngineLight *light);
    void            checkLightBuffers();
    void            draw1DShadowMap(DrEngineLight *light);
    bool            draw2DLight(DrEngineLight *light);
    void            drawGlowLights();
    void            drawShadowMaps();
    int             findNeededShadowMaps();
    void            process2DLights();

    // Getters and Setters
    float           getScale()          { return m_scale; }


//####################################################################################
//##    Shader Programs / Attributes / Uniforms
//####################################################################################
private:
    // Default Shader
    QOpenGLShaderProgram m_default_shader;
    int     a_default_vertex;
    int     a_default_texture_coord;
    int     a_default_normal;
    int     u_default_matrix;
    int     u_default_matrix_object;

    int     u_default_texture;
    int     u_default_width;                                    // Width of texture
    int     u_default_height;                                   // Height of texture
    int     u_default_time;                                     // Time in seconds
    int     u_default_pre;                                      // Premultiplied Alpha Texture?
    int     u_default_alpha;                                    // Opacity
    int     u_default_tint;                                     // Tint, red/green/blue (0 to 1, 0 to 1, 0 to 1)
    int     u_default_zoom;                                     // Zoom (m_scale)

    int     u_default_pixel_x;                                  // Pixelation X value
    int     u_default_pixel_y;                                  // Pixelation Y value
    int     u_default_pixel_offset;                             // Offset for pixelation
    int     u_default_negative;                                 // Negative?
    int     u_default_grayscale;                                // Grayscale?
    int     u_default_hue;                                      // Hue, 0.0 to 1.0
    int     u_default_saturation;                               // Saturation, -1.0 to 1.0
    int     u_default_contrast;                                 // Contrast,   -1.0 to 1.0
    int     u_default_brightness;                               // Brightness, -1.0 to 1.0

    int     u_default_shade_away;                               // Should fade triangles facing away from camera?
    int     u_default_camera_pos;                               // Current camera position

    int     u_default_bitrate;                                  // Bitrate
    int     u_default_cartoon;                                  // Cartoon? (Comic Book)
    int     u_default_wavy;                                     // Wavy? (Ripple Effect)


    // Occluder Map Shader
    QOpenGLShaderProgram m_occluder_shader;
    int     a_occluder_vertex;
    int     a_occluder_texture_coord;
    int     u_occluder_matrix;

    int     u_occluder_texture;
    int     u_occluder_alpha;
    int     u_occluder_depth;
    int     u_occluder_near_plane;
    int     u_occluder_far_plane;

    // Shadow Map Shader
    QOpenGLShaderProgram m_shadow_shader;
    int     a_shadow_vertex;
    int     a_shadow_texture_coord;
    int     u_shadow_matrix;

    int     u_shadow_texture;
    int     u_shadow_resolution;
    int     u_shadow_ray_count;
    int     u_shadow_depth;
    int     u_shadow_near_plane;

    // 2D Light Shader
    QOpenGLShaderProgram m_light_shader;
    int     a_light_vertex;
    int     a_light_texture_coord;
    int     u_light_matrix;

    int     u_light_texture;
    int     u_light_ray_count;
    int     u_light_diameter;
    int     u_light_fitted;
    int     u_light_alpha;
    int     u_light_color;                                      // Color of light, red/green/blue (0 to 1, 0 to 1, 0 to 1)
    int     u_light_cone;
    int     u_light_shadows;
    int     u_light_intensity;
    int     u_light_blur;
    int     u_light_draw_shadows;
    int     u_light_is_glow;

    // "Screen" (Hard Light) Shader
    QOpenGLShaderProgram m_screen_shader;
    int     a_screen_vertex;
    int     a_screen_texture_coord;
    int     u_screen_matrix;

    int     u_screen_upper;
    int     u_screen_lower;
    int     u_screen_blend;
    int     u_screen_width;
    int     u_screen_height;

    // Kernel Shader
    QOpenGLShaderProgram m_kernel_shader;
    int     a_kernel_vertex;
    int     a_kernel_texture_coord;
    int     u_kernel_matrix;

    int     u_kernel_texture;
    int     u_kernel_width;                                     // Width of texture
    int     u_kernel_height;                                    // Height of texture
    int     u_kernel_alpha;                                     // Opacity


    // Water Shader
    QOpenGLShaderProgram m_water_shader;
    int     a_water_vertex;
    int     a_water_texture_coord;
    int     u_water_matrix;

    int     u_water_width;                                      // Width of texture
    int     u_water_height;                                     // Height of texture
    int     u_water_time;                                       // Time in seconds
    int     u_water_angle;                                      // Angle of water
    int     u_water_alpha;                                      // Opacity
    int     u_water_zoom;                                       // Current zoom level (need for water shader)
    int     u_water_pos;                                        // Current camera position (need for water shader)

    int     u_water_top;
    int     u_water_bottom;
    int     u_water_left;
    int     u_water_right;

    int     u_water_start_color;
    int     u_water_end_color;
    int     u_water_color_tint;
    int     u_water_reflection;
    int     u_water_ripple_frequency;
    int     u_water_ripple_speed;
    int     u_water_ripple_amplitude;
    int     u_water_ripple_stretch;
    int     u_water_wave_frequency;
    int     u_water_wave_speed;
    int     u_water_wave_amplitude;
    int     u_water_surface_color;
    int     u_water_surface_tint;
    int     u_water_surface_height;
    int     u_water_surface_flat;
    int     u_refract_reflection;
    int     u_refract_underwater;
    int     u_refract_texture;
    int     u_refract_foam;
    int     u_water_movement_speed;

    int     u_water_pixel_x;
    int     u_water_pixel_y;
    int     u_water_bitrate;

    // Fisheye Shader
    QOpenGLShaderProgram m_fisheye_shader;
    int     a_fisheye_vertex;
    int     a_fisheye_texture_coord;
    int     u_fisheye_matrix;

    int     u_fisheye_width;                                    // Width of texture
    int     u_fisheye_height;                                   // Height of texture
    int     u_fisheye_time;                                     // Time in seconds
    int     u_fisheye_angle;                                    // Angle of fisheye
    int     u_fisheye_alpha;                                    // Opacity
    int     u_fisheye_zoom;                                     // Current zoom level (need for fisheye shader)
    int     u_fisheye_pos;                                      // Current camera position (need for fisheye shader)

    int     u_fisheye_top, u_fisheye_bottom, u_fisheye_left, u_fisheye_right;

    int     u_fisheye_start_color;
    int     u_fisheye_color_tint;
    int     u_fisheye_lens_zoom;

    int     u_fisheye_pixel_x;
    int     u_fisheye_pixel_y;
    int     u_fisheye_bitrate;

    // Fire Shader
    QOpenGLShaderProgram m_fire_shader;
    int     a_fire_vertex;
    int     a_fire_texture_coord;
    int     u_fire_matrix;

    int     u_fire_alpha;
    int     u_fire_time;                                        // Time in seconds
    int     u_fire_position;
    int     u_fire_width, u_fire_height;
    int     u_fire_shape;
    int     u_fire_start_color;
    int     u_fire_end_color;
    int     u_fire_smoke_color;
    int     u_fire_intensity;
    int     u_fire_smoothness;
    int     u_fire_wavy;
    int     u_fire_speed;

    int     u_fire_pixel_x;
    int     u_fire_pixel_y;
    int     u_fire_bitrate;


    // Mirror Shader
    QOpenGLShaderProgram m_mirror_shader;
    int     a_mirror_vertex;
    int     a_mirror_texture_coord;
    int     u_mirror_matrix;

    int     u_mirror_width;                                      // Width of texture
    int     u_mirror_height;                                     // Height of texture
    int     u_mirror_time;                                       // Time in seconds
    int     u_mirror_angle;                                      // Angle of mirror
    int     u_mirror_alpha;                                      // Opacity
    int     u_mirror_zoom;                                       // Current zoom level (need for mirror shader)
    int     u_mirror_pos;                                        // Current camera position (need for mirror shader)

    int     u_mirror_top, u_mirror_bottom, u_mirror_left, u_mirror_right;

    int     u_mirror_color_top;
    int     u_mirror_color_bottom;
    int     u_mirror_color_tint;
    int     u_mirror_blur;
    int     u_mirror_blur_stretch;
    int     u_mirror_scale;

    int     u_mirror_pixel_x;
    int     u_mirror_pixel_y;
    int     u_mirror_bitrate;


    // Swirl Shader
    QOpenGLShaderProgram m_swirl_shader;
    int     a_swirl_vertex;
    int     a_swirl_texture_coord;
    int     u_swirl_matrix;

    int     u_swirl_width;                                       // Width of texture
    int     u_swirl_height;                                      // Height of texture
    int     u_swirl_time;                                        // Time in seconds
    int     u_swirl_angle;                                       // Angle of swirl
    int     u_swirl_alpha;                                       // Opacity
    int     u_swirl_zoom;                                        // Current zoom level (need for swirl shader)
    int     u_swirl_pos;                                         // Current camera position (need for swirl shader)

    int     u_swirl_top, u_swirl_bottom, u_swirl_left, u_swirl_right;

    int     u_swirl_start_color;
    int     u_swirl_color_tint;
    int     u_swirl_rotation;
    int     u_swirl_radius;

    int     u_swirl_pixel_x;
    int     u_swirl_pixel_y;
    int     u_swirl_bitrate;

    // ********** End Shaders **********

};

#endif // OPENGL_H










