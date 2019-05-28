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
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QMatrix4x4>

#include <chrono>

// Forward Declarations
class DrEngine;
class FormEngine;

// Type definitions
typedef std::chrono::high_resolution_clock Clock;


//####################################################################################
//##    OpenGL
//##        A modern OpenGL Widget
//############################
class OpenGL : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    FormEngine     *m_form_engine;                                  // Pointer to parent FormEngine
    DrEngine       *m_engine;                                       // Pointer to Engine instance that will run in this OpenGLWidget

    QMatrix4x4      m_model_view;
    QMatrix4x4      m_projection;

    int             m_zoom = 250;                                   // Zoom level of current view, 200 is 50% - 250 is 100%
    float           m_scale = 1.0;                                  // Updated in zoomInOut for use during painting grid, DO NOT SET MANUALLY
    float           m_angle = 0;

    float           m_background_red = 0;
    float           m_background_green = 0;
    float           m_background_blue = 0;

    // Shader Variables
    QOpenGLShaderProgram    m_shader;
    int     m_attribute_vertex;
    int     m_attribute_tex_coord;
    int     m_uniform_matrix;
    int     m_uniform_texture;
    int     m_uniform_alpha;

    // Timer Variables
    Clock::time_point m_time_fps = Clock::now();
    Clock::time_point m_time_frame = Clock::now();
    double  m_time_one_frame_takes_to_render = 0.0;
    double  m_time_percent = 1.0;
    int     m_fps_count = 0;                                        // Counts renders to calculate frames per second

public:
    // Constructor / Destructor
    OpenGL(QWidget *parent, FormEngine *form_engine, DrEngine *engine);
    ~OpenGL() override;

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
    void            zoomInOut(int level);

    // Render Calls
    void            cullingOn();
    void            cullingOff();
    void            drawCube(QVector3D center);
    void            drawDebugCollisions(QPainter &painter);
    void            drawDebugJoints(QPainter &painter);
    void            drawDebugShapes(QPainter &painter);
    void            drawObjects();
    void            updateViewMatrix();

    // Getters and Setters
    float               getScale()          { return m_scale; }
    Clock::time_point&  getTimeFrameStart() { return m_time_frame; }
    double              getTimePercent()    { return m_time_percent; }

    void                setTimeOneFrameTakesToRender(double milliseconds)  { m_time_one_frame_takes_to_render = milliseconds; }


};

#endif // OPENGL_H










