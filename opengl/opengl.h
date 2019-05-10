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

class DrEngine;

class OpenGL : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    DrEngine       *m_engine;

    QMatrix4x4      m_model_view;
    QMatrix4x4      m_projection;

    int             m_zoom = 250;                                     // Zoom level of current view, 200 is 50% - 250 is 100%
    float           m_scale = 1.0;                                    // Updated in zoomInOut for use during painting grid, DO NOT SET MANUALLY
    float           m_angle = 0;

    QOpenGLShaderProgram m_program;
    int     m_vertexAttr;
    int     m_texCoordAttr;
    int     m_matrixUniform;
    int     m_texUniform;


public:
    float    cube_angle = 0;


public:
    // Constructor / Destructor
    OpenGL(QWidget *parent, DrEngine *engine);
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
    void            drawCube(QVector3D center);
    void            drawDebugShapes(QPainter &painter);
    QVector3D       mapFromScreen(double x, double y);
    QVector3D       mapFromScreen(float x, float y);
    QVector3D       mapFromScreen(QPointF point);
    QPointF         mapToScreen(double x, double y, double z);
    QPointF         mapToScreen(float x, float y, float z);
    QPointF         mapToScreen(QVector3D point3D);
    void            zoomInOut(int level);

    // Getters and Setters
    float           getScale()      { return m_scale; }


public slots:
    void        cleanUp();

signals:
    void        updateInfo(QString info);

};

#endif // OPENGL_H










