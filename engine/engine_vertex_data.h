//
//      Created by Stephens Nunnally on 8/29/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef OPENGL_VBO_H
#define OPENGL_VBO_H

#include <QOpenGLWidget>
#include <QVector>
#include <QVector2D>
#include <QVector3D>


// Local Constants
const int c_vertex_length = 8;          // 8 is (3) for xyz, (3) for normal, and (2) for texture coordinate


//####################################################################################
//##    DrEngineVertexData
//##        Stores a list of triangles for rendering
//############################
class DrEngineVertexData
{
private:
    QVector<GLfloat>    m_data;
    int                 m_count;

public:
    // Constructor
    DrEngineVertexData();

    // Properties
    const GLfloat  *constData() const {     return m_data.constData(); }
    int             count() const {         return m_count; }
    int             triangleCount() const { return vertexCount() / 3; }
    int             vertexCount() const {   return m_count / c_vertex_length; }

    // Creation Functions
    void    initializeExtrudedPixmap(QPixmap &pixmap);
    void    initializeTextureCube();
    void    initializeTextureQuad();

    // Building Functions
    void    add(const QVector3D &v, const QVector3D &n, const QVector2D &t);
    void    extrude(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                    GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2);
    void    cube(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                 GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                 GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3,
                 GLfloat x4, GLfloat y4, GLfloat tx4, GLfloat ty4);
    void    quad(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                 GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                 GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3,
                 GLfloat x4, GLfloat y4, GLfloat tx4, GLfloat ty4);
    void    triangle(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                     GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                     GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3);
};


#endif // OPENGL_VBO_H











