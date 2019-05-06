//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef PHYSICS_OPENGL_H
#define PHYSICS_OPENGL_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QMatrix4x4>

class FormEngine;

class PhysicsOpenGL : public QOpenGLWidget, protected QOpenGLFunctions
{
private:
    FormEngine         *m_parent;

    QMatrix4x4          m_model_view;
    int                 m_zoom = 250;                                     // Zoom level of current view, 200 is 50% - 250 is 100%
    float               m_scale = 1.0;                                    // Updated in zoomInOut for use during painting grid, DO NOT SET MANUALLY
    float               m_angle = 0;


    QPixmap     p_ball = QPixmap(":/assets/test_images/ball_1.png");


    QOpenGLShaderProgram m_program;
    int     m_vertexAttr;
    int     m_texCoordAttr;
    int     m_matrixUniform;
    int     m_texUniform;

    // Flags
    bool          m_flag_spacebar = false;


public:
    // Constructor / Destructor
    PhysicsOpenGL(QWidget *parent, FormEngine *main_window);
    ~PhysicsOpenGL() override;

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
    void            drawCube();
    QOpenGLTexture* initTexture(QString image_name);
    QPointF         mapToScreen(QVector3D point3D);
    void            zoomInOut(int level);


public slots:
    void            cleanUp();

};

#endif // PHYSICS_OPENGL_H

















