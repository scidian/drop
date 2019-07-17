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

#include "enums_engine.h"

// Forward Declarations
class DrEngine;
class DrEngineLight;
class DrEngineObject;
class FormEngine;

// Type Definitions
typedef std::chrono::high_resolution_clock Clock;

// Open GL Globals - defined in opengl_initialize.cpp
extern int  g_max_texture_size;
extern int  g_max_rays;
extern int  g_max_occluder_fbo_size;
extern int  g_max_light_fbo_size;

// Rendering Constants
const float c_near_plane =     -10000.0;
const float c_far_plane =       10000.0;
const bool  c_use_cam_offset =  true;
const float c_field_of_view =   52.5f;                  // Close to Orthographic size when using standard widescreen ratio

// 2D Light Constants
const float c_occluder_scale_ortho =       1.00f;       // Scale to use for occlusion map (higher the number, less shaky the shadows)
const float c_occluder_scale_proj =        1.00f;       // Scale to use for occlusion map (higher the number, less shaky the shadows)
const int   c_desired_max_rays =           4096;        // Desired max number of rays to send out during shadow map calculations
const int   c_desired_occluder_fbo_size =  8192;        // Desired max width and height of offscreen fbo used for shadow map
const int   c_desired_light_fbo_size =     8192;        // Desired max width and height of offscreen fbo used for lights


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

    int             m_zoom =  250;                              // Zoom level of current view, 200 is 50% - 250 is 100%
    float           m_scale = 1.0;                              // Updated in zoomInOut for use during painting grid, DO NOT SET MANUALLY
    float           m_angle =   0;

    float           m_background_red = 0;
    float           m_background_green = 0;
    float           m_background_blue = 0;

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



    // ********** Shader Programs / Attributes / Uniforms **********

    // Default Shader
    QOpenGLShaderProgram m_default_shader;
    int     a_default_vertex;
    int     a_default_texture_coord;
    int     u_default_matrix;

    int     u_default_texture;
    int     u_default_width;                                    // Width of texture
    int     u_default_height;                                   // Height of texture
    int     u_default_time;                                     // Time in seconds
    int     u_default_pre;                                      // Premultiplied Alpha Texture?
    int     u_default_alpha;                                    // Opacity
    int     u_default_tint;                                     // Tint, red/green/blue (0 to 1, 0 to 1, 0 to 1)

    int     u_default_pixel_x;                                  // Pixelation X value
    int     u_default_pixel_y;                                  // Pixelation Y value
    int     u_default_negative;                                 // Negative?
    int     u_default_grayscale;                                // Grayscale?
    int     u_default_hue;                                      // Hue, 0.0 to 1.0
    int     u_default_saturation;                               // Saturation, -1.0 to 1.0
    int     u_default_contrast;                                 // Contrast,   -1.0 to 1.0
    int     u_default_brightness;                               // Brightness, -1.0 to 1.0
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


    // ********** End Shaders **********


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

    // Render Calls
    void            bindOffscreenBuffer(bool clear = true);
    void            cullingOn();
    void            cullingOff();
    void            drawCube(QVector3D center);
    void            drawDebug(QPainter &painter);
    void            drawDebugCollisions(QPainter &painter);
    void            drawDebugHealth(QPainter &painter);
    void            drawDebugHealthNative(QPainter &painter);
    void            drawDebugJoints(QPainter &painter);
    void            drawDebugShapes(QPainter &painter);
    void            drawFrameBufferUsingDefaultShader(QOpenGLFramebufferObject *fbo);
    void            drawFrameBufferUsingKernelShader(QOpenGLFramebufferObject *fbo);
    void            drawFrameBufferUsingScreenShader(QOpenGLFramebufferObject *upper, QOpenGLFramebufferObject *lower);
    bool            drawGlowBuffer();
    void            drawSpace();
    void            drawSpaceOccluder();
    QColor          objectDebugColor(DrEngineObject *object, bool text_color = false);
    QMatrix4x4      occluderMatrix(Render_Type render_type, bool use_offset);
    void            updateViewMatrix(Render_Type render_type, bool use_offset);
    void            releaseOffscreenBuffer();
    void            setShaderDefaultValues(float texture_width, float texture_height);
    void            setNumberTextureCoordinates(QString letter, std::vector<float> &texture_coordinates);
    void            setUpSpaceShader(std::vector<float> &texture_coords);
    void            setVertexFromSides(QVector<GLfloat> &vertices, float left, float right, float top, float bottom, float z);
    void            setWholeTextureCoordinates(std::vector<float> &texture_coords);

    // Soft Shadows / Lights
    void            bindGlowLightsBuffer(float ambient_light);
    void            bindOccluderMapBuffer();
    void            bindLightOcculderBuffer(DrEngineLight *light);
    void            bindLightShadowBuffer(DrEngineLight *light);
    void            checkLightBuffers();
    void            draw1DShadowMap(DrEngineLight *light);
    void            draw2DLight(DrEngineLight *light);
    void            drawGlowLights();
    void            drawShadowMaps();
    int             findNeededShadowMaps();
    void            process2DLights();

    // Getters and Setters
    float           getScale()          { return m_scale; }

};

#endif // OPENGL_H










