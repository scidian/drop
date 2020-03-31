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

#include "3rd_party/chipmunk/chipmunk.h"
#include "3rd_party/fontstash/fontstash.h"
#include "3rd_party/fontstash/glfontstash.h"
#include "3rd_party/glm/ext.hpp"
#include "3rd_party/glm/glm.hpp"
#include "core/dr_string.h"
#include "core/types/dr_point.h"
#include "core/types/dr_pointf.h"
#include "core/types/dr_vec3.h"
#include "engine/constants_engine.h"
#include "engine/enums_engine.h"
#include "project/enums_entity_types.h"

// Forward Class Declarations
class DebugVertex;
class DrColor;
class DrEngine;
class DrEngineObject;
class DrEngineThing;
class DrEngineVertexData;
class DrImage;
class FormEngine;

// Type Definitions
typedef std::chrono::high_resolution_clock Clock;

// OpenGL Constants
const  int      c_float_size =      static_cast<int>(sizeof(GLfloat));

// Open GL Globals - defined in opengl_initialize.cpp
extern int      g_max_texture_size;
extern int      g_max_rays;
extern int      g_max_occluder_fbo_size;
extern int      g_max_light_fbo_size;

// VBO Constants
#define PROGRAM_VERTEX_ATTRIBUTE        0
#define PROGRAM_NORMAL_ATTRIBUTE        1
#define PROGRAM_TEXCOORD_ATTRIBUTE      2
#define PROGRAM_BARYCENTRIC_ATTRIBUTE   3


//####################################################################################
//##    DrOpenGL
//##        A modern OpenGL Widget
//############################
class DrOpenGL : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    // Constructor / Destructor
    DrOpenGL(QWidget *parent, FormEngine *form_engine, DrEngine *engine);
    ~DrOpenGL() override;


    // #################### VARIABLES ####################
private:
    // External Borrowed Pointers
    FormEngine     *m_form_engine;                                  // Pointer to parent FormEngine
    DrEngine       *m_engine;                                       // Pointer to Engine instance that will run in this OpenGLWidget

    // Fontstash
    FONScontext    *fs = nullptr;
    int             font_normal = FONS_INVALID;

    // Local Variables
    QMatrix4x4      m_view;
    QMatrix4x4      m_projection;

    QVector3D       m_eye;
    QVector3D       m_look_at;
    QVector3D       m_up;

    QVector3D       m_origin;                                       // Used to calculate relative camera position, for pixelation and other things

    int             m_zoom =  250;                                  // Zoom level, 200 is 50% - 250 is 100%
    double          m_zoom_scale = 1.0;                             // Zoom level represented as magification multiplier

    float           m_background_red = 0;
    float           m_background_green = 0;
    float           m_background_blue = 0;

    DrThingType     m_last_thing = DrThingType::None;               // This variable stores last draw type, so that multiple effects drawn back to back
                                                                    // will use the same copy of the render fbo as it currently was, this saves lots of blit calls,
                                                                    // and stops some vertical fragments from appearing as they would try to refract each other

    double          m_fps_count_render = 0.0;                       // Tracks frames per second
    long            m_triangles = 0;                                // Tracks how many triangles are drawn every frame, including occluder map draws
    double          m_add_z;                                        // Used to stop z fighting

    std::vector<float>      m_quad_vertices;                        // Used to keep standard 2D textured quad coordinates
    std::vector<float>      m_quad_texture_coordinates;             // Used to keep the coordinates of rendering an entire texture as a quad
    std::vector<float>      m_quad_barycentric;                     // Used to keep standard 2D textured quad barycentric coords

    // VBO's
    std::map<long, QOpenGLBuffer*>      m_texture_vbos;             // Stores extruded texture vbo's
    std::map<long, DrEngineVertexData*> m_texture_data;             // Stores extruded texture vertex data
    QOpenGLBuffer                      *m_cone_vbo;                 // Stores cone vbo, to use to turn a texture into a cone
    QOpenGLBuffer                      *m_cube_vbo;                 // Stores cube vbo, to use to turn a texture into a cube
    QOpenGLBuffer                      *m_quad_vbo;                 // Stores quad vbo, to use to render 2d quads

    // Frame Buffers
    QOpenGLFramebufferObject *m_render_fbo =            nullptr;    // Used for offscreen rendering
    QOpenGLFramebufferObject *m_texture_fbo =           nullptr;    // m_render_fbo must be copied to a non-multisampled fbo before being used as a texture
    QOpenGLFramebufferObject *m_glow_fbo =              nullptr;    // Used for rendering Glow Lights onto scene all at once
    QOpenGLFramebufferObject *m_occluder_fbo =          nullptr;    // Used for rendering an occlusion map for use with lights

    int                       m_desired_fbo_width =     1;          // Desired width  of off screen render target
    int                       m_desired_fbo_height =    1;          // Desired height of off screen render target
    float                     m_fbo_scale_x =           1.0f;       // Scale between off screen fbo and screen size
    float                     m_fbo_scale_y =           1.0f;       // Scale between off screen fbo and screen size

    // Frame Buffer Objects for use during Render
    std::map<long, QOpenGLFramebufferObject*> m_occluders;          // Off screen Frame Buffer Objects for Light Occluder Maps
    std::map<long, QOpenGLFramebufferObject*> m_shadows;            // Off screen Frame Buffer Objects for Light 1D Shadow Maps

    // Vectors to keep track of lights
    std::vector<DrEngineThing*> m_shadow_lights;
    std::vector<DrEngineThing*> m_glow_lights;


    // #################### FUNCTIONS ####################
public:
    // Getters
    DrEngine*       getEngine() { return m_engine; }

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

    // Frames Per Second
    double          getFpsCount() { return m_fps_count_render; }
    void            resetFpsCount() { m_fps_count_render = 0; }

    // Screen Functions
    float           combinedZoomScale();
    glm::vec3       mapFromScreen(double x, double y);
    glm::vec3       mapFromScreen(float x, float y);
    glm::vec3       mapFromScreen(DrPointF point);
    DrPointF        mapToScreen(double x, double y, double z);
    DrPointF        mapToScreen(float x, float y, float z);
    DrPointF        mapToScreen(glm::vec3 point3D);
    DrPointF        mapToFBO(glm::vec3 point3D, QOpenGLFramebufferObject *fbo, QMatrix4x4 view_matrix, QMatrix4x4 proj_matrix);
    static double   zoomPowToScale(double pow);
    static double   zoomScaleToPow(double scale);
    void            zoomInOut(int level);
    void            zoomToScale(double scale);

    // Initialization Calls
    void            importTexture(long texture_id, std::string from_asset_string);
    void            importTexture(long texture_id, DrImage *image);
    void            loadBuiltInModels();
    void            loadBuiltInTextures();
    void            loadFonts();
    void            loadProjectTextures();
    void            loadShaders();


    // Debug Render Calls - New
    void            addDebugCircle(DebugVertex &vertexes, DrPointF pos, float radius, float angle, DrColor fill, DrColor border, bool draw_angle = true);
    void            addDebugLine(DebugVertex &vertexes, cpVect a, cpVect b, float radius, DrColor fill, DrColor border);
    void            addDebugPolygon(DebugVertex &vertexes, const std::vector<cpVect> &verts, const DrPointF &centroid, float radius, DrColor fill, DrColor border, bool add_outlines);
    void            drawDebug();
    void            drawDebugCollisions();
    void            drawDebugHealth();
    void            drawDebugJoints();
    void            drawDebugShapes();
    void            drawDebugTriangles(QMatrix4x4 mvp, DebugVertex &vertexes);


    // Render Calls
    void            bindOffscreenBuffer(bool clear = true);
    void            cullingOn(bool reversed = false);
    void            cullingOff();
    bool            drawEffect(DrEngineThing *thing, DrThingType thing_type);
    void            drawFrameBufferUsingDefaultShader(QOpenGLFramebufferObject *fbo);
    bool            drawFrameBufferUsingFisheyeShader(QOpenGLFramebufferObject *fbo, DrEngineThing *lens);
    void            drawFrameBufferUsingKernelShader(QOpenGLFramebufferObject *fbo);
    bool            drawFrameBufferUsingMirrorShader(QOpenGLFramebufferObject *fbo, DrEngineThing *mirror);
    void            drawFrameBufferUsingScreenShader(QOpenGLFramebufferObject *upper, QOpenGLFramebufferObject *lower, Blend_Mode mode);
    void            drawFrameBufferUsingSimpleShader(QOpenGLFramebufferObject *fbo);
    bool            drawFrameBufferUsingSwirlShader(QOpenGLFramebufferObject *fbo, DrEngineThing *swirl);
    bool            drawFrameBufferUsingWaterShader(QOpenGLFramebufferObject *fbo, DrEngineThing *water);
    bool            drawGlowBuffer();
    void            drawObject(DrEngineThing *thing, bool draw2D);
    void            drawObjectSimple(DrEngineThing *thing);
    bool            drawObjectFire(DrEngineThing *thing);
    bool            drawObjectOccluder(DrEngineThing *thing, bool need_init_shader = true);
    void            drawSpace();
    bool            getEffectPosition(QOpenGLFramebufferObject *fbo, DrEngineThing *thing,
                                      double &top, double &bottom, double &left, double &right, float &angle);
    void            getThingVertices(std::vector<GLfloat> &vertices, DrEngineThing *thing, double width, double height, float extra_scale_x = 1.0, float extra_scale_y = 1.0);
    static DrColor  objectDebugColor(Collision_Type collision_type, bool sleeping = false);
    void            occluderMatrix(Render_Type render_type, QMatrix4x4 &view_matrix, QMatrix4x4 &proj_matrix);
    QMatrix4x4      orthoMatrix(float width, float height);
    void            updateViewMatrix(Render_Type render_type, float width, float height);
    void            releaseOffscreenBuffer();
    void            releaseDefaultAttributeBuffer();
    void            setDefaultAttributeBuffer(QOpenGLBuffer *buffer);
    void            setShaderDefaultValues(float texture_width, float texture_height);
    void            setNumberTextureCoordinates(std::string letter, std::vector<float> &texture_coordinates);
    void            setQuadVertices(std::vector<float> &vertices, float width, float height, DrPointF center, float z);
    void            setQuadRotatedVertices(std::vector<float> &vertices, DrVec3 &top_right, DrVec3 &top_left,
                                           DrVec3 &bot_left, DrVec3 &bot_right, DrVec3 position);

    // Soft Shadows / Lights
    void            bindGlowLightsBuffer(float ambient_light);
    void            bindOccluderMapBuffer();
    void            bindLightOcculderBuffer(DrEngineThing *light);
    void            bindLightShadowBuffer(DrEngineThing *light);
    void            checkLightBuffers();
    void            draw1DShadowMap(DrEngineThing *light);
    bool            draw2DLight(DrEngineThing *light);
    void            drawGlowLights();
    void            drawShadowMaps();
    int             findNeededShadowMaps();
    void            process2DLights();

    // Triangles
    void            addTriangles(long triangles)    { m_triangles += triangles; }
    long            getTriangleCount()              { return m_triangles; }



//####################################################################################
//##    Shader Programs / Attributes / Uniforms
//####################################################################################
private:
    // Default Shader
    QOpenGLShaderProgram m_default_shader;
    int     a_default_vertex;
    int     a_default_normal;
    int     a_default_texture_coord;
    int     a_default_barycentric;
    int     u_default_matrix;
    int     u_default_matrix_model;

    int     u_default_texture;
    int     u_default_texture_pixel;
    int     u_default_width;                                    // Width of texture
    int     u_default_height;                                   // Height of texture
    int     u_default_time;                                     // Time in seconds
    int     u_default_pre;                                      // Premultiplied Alpha Texture?
    int     u_default_alpha;                                    // Opacity
    int     u_default_average_color;                            // Average texture color
    int     u_default_tint;                                     // Tint, red/green/blue (0 to 1, 0 to 1, 0 to 1)
    int     u_default_zoom;                                     // Zoom (m_scale)

    int     u_default_pixel_x;                                  // Pixelation X value
    int     u_default_pixel_y;                                  // Pixelation Y value
    int     u_default_pixel_offset;                             // Offset for pixelation
    int     u_default_pixel_type;                               // Pixelation texture
    int     u_default_negative;                                 // Negative?
    int     u_default_grayscale;                                // Grayscale?
    int     u_default_hue;                                      // Hue, 0.0 to 1.0
    int     u_default_saturation;                               // Saturation, -1.0 to 1.0
    int     u_default_contrast;                                 // Contrast,   -1.0 to 1.0
    int     u_default_brightness;                               // Brightness, -1.0 to 1.0

    int     u_default_camera_type;                              // Float, Perspective == 0.0, Orthographic == 1.0
    int     u_default_shade_away;                               // Should fade triangles facing away from camera?
    int     u_default_camera_pos;                               // Current camera position (m_eye)
    int     u_default_look_at_pos;                              // Current target look at position (m_look_at)

    int     u_default_bitrate;                                  // Bitrate
    int     u_default_cartoon;                                  // Cartoon? (Comic Book)
    int     u_default_cartoon_width;                            // Cartoon Width
    int     u_default_cross_hatch;                              // Cross Hatch? (Comic Book)
    int     u_default_cross_hatch_width;                        // Cross Hatch Width
    int     u_default_wavy;                                     // Wavy? (Ripple Effect)
    int     u_default_wireframe;                                // Wireframe?
    int     u_default_wireframe_width;                          // Wireframe Width


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

    // Simple Shader
    QOpenGLShaderProgram m_simple_shader;
    int     a_simple_vertex;
    int     a_simple_texture_coord;
    int     u_simple_matrix;
    int     u_simple_texture;
    int     u_simple_alpha;                                     // Opacity


    // Debug Shader
    QOpenGLShaderProgram m_debug_shader;
    int     a_debug_position;
    int     a_debug_texture_coord;
    int     a_debug_radius;
    int     a_debug_color_fill;
    int     a_debug_color_border;
    int     u_debug_matrix;


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










