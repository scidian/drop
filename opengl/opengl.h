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

// 2D Light Constants
const float c_occluder_scale =                3.0f;     // Scale to use for occlusion map (higher the number, less shaky the shadows)
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
    QOpenGLFramebufferObject *m_occluder_fbo = nullptr;         // Used for rendering an occlusion map for use with lights

    // Shader Variables
    QOpenGLShaderProgram m_shader;
    int     m_attribute_vertex;
    int     m_attribute_tex_coord;
    int     m_uniform_matrix;

    // Custom Shader Input
    int     m_uniform_texture;
    int     m_uniform_width;                                    // Width of texture
    int     m_uniform_height;                                   // Height of texture
    int     m_uniform_alpha;                                    // Opacity
    int     m_uniform_bitrate;                                  // Bitrate
    int     m_uniform_pixel_x;                                  // Pixelation X value
    int     m_uniform_pixel_y;                                  // Pixelation Y value
    int     m_uniform_negative;                                 // Negative?
    int     m_uniform_grayscale;                                // Grayscale?
    int     m_uniform_hue;                                      // Hue, 0.0 to 1.0
    int     m_uniform_saturation;                               // Saturation, -1.0 to 1.0
    int     m_uniform_contrast;                                 // Contrast,   -1.0 to 1.0
    int     m_uniform_brightness;                               // Brightness, -1.0 to 1.0
    int     m_uniform_tint;                                     // Tint, red/green/blue (0 to 1, 0 to 1, 0 to 1)
    int     m_uniform_kernel;                                   // Kernel Effects?

    // Shadow Map Shader
    QOpenGLShaderProgram m_occluder_shader;
    int     m_attribute_occluder_vertex;
    int     m_attribute_occluder_tex_coord;
    int     m_uniform_occluder_matrix;

    int     m_uniform_occluder_texture;
    int     m_uniform_occluder_alpha;
    int     m_uniform_occluder_depth;

    // Shadow Map Shader
    QOpenGLShaderProgram m_shadow_shader;
    int     m_attribute_shadow_vertex;
    int     m_attribute_shadow_tex_coord;
    int     m_uniform_shadow_matrix;

    int     m_uniform_shadow_texture;
    int     m_uniform_shadow_resolution;
    int     m_uniform_shadow_ray_count;
    int     m_uniform_shadow_depth;

    // 2D Light Shader
    QOpenGLShaderProgram m_light_shader;
    int     m_attribute_light_vertex;
    int     m_attribute_light_tex_coord;
    int     m_uniform_light_matrix;

    int     m_uniform_light_texture;
    int     m_uniform_light_diameter;
    int     m_uniform_light_fitted;
    int     m_uniform_light_alpha;
    int     m_uniform_light_color;                              // Color of light, red/green/blue (0 to 1, 0 to 1, 0 to 1)
    int     m_uniform_light_cone;
    int     m_uniform_light_shadows;
    int     m_uniform_light_intensity;
    int     m_uniform_light_blur;
    int     m_uniform_light_draw_shadows;


public:
    // Constructor / Destructor
    DrOpenGL(QWidget *parent, FormEngine *form_engine, DrEngine *engine);
    ~DrOpenGL() override;

    // Event Overrides
    virtual void    keyPressEvent(QKeyEvent *event) override;
    virtual void    keyReleaseEvent(QKeyEvent *event) override;
    virtual void    mousePressEvent(QMouseEvent *event) override;
    virtual void    mouseReleaseEvent(QMouseEvent *event) override;

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
    QPointF         mapToOccluder(QVector3D point3D);
    void            zoomInOut(int level);

    // Render Calls
    void            bindOffscreenBuffer();
    void            cullingOn();
    void            cullingOff();
    void            drawCube(QVector3D center);
    void            drawDebug(QPainter &painter);
    void            drawDebugCollisions(QPainter &painter);
    void            drawDebugHealth(QPainter &painter);
    void            drawDebugHealthNative(QPainter &painter);
    void            drawDebugJoints(QPainter &painter);
    void            drawDebugShapes(QPainter &painter);
    void            drawFrameBufferToScreenBuffer(QOpenGLFramebufferObject *fbo, bool use_kernel = false);
    void            drawSpace();
    void            drawSpaceOccluder();
    QColor          objectDebugColor(DrEngineObject *object, bool text_color = false);
    void            updateViewMatrix();
    QMatrix4x4      occluderMatrix();
    void            setShaderDefaultValues(float texture_width, float texture_height);
    void            setNumberTextureCoordinates(QString letter, std::vector<float> &texture_coordinates);
    void            setVertexFromSides(QVector<GLfloat> &vertices, float left, float right, float top, float bottom);
    void            setWholeTextureCoordinates(std::vector<float> &texture_coords);

    // Soft Shadows
    void            bindOccluderMapBuffer();
    void            bindLightOcculderBuffer(DrEngineLight *light);
    void            bindLightShadowBuffer(DrEngineLight *light);
    void            draw1DShadowMap(DrEngineLight *light);
    void            draw2DLight(DrEngineLight *light);
    void            drawShadowMaps();

    // Getters and Setters
    float           getScale()          { return m_scale; }

};

#endif // OPENGL_H










