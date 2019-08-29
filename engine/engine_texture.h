//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_TEXTURE_H
#define ENGINE_TEXTURE_H

#include <QOpenGLTexture>

//####################################################################################
//##    DrEngineTexture
//##        Holds an OpenGL texture for use in the Engine
//############################
class DrEngineTexture
{
private:
    // Local Variables
    QOpenGLTexture     *m_texture;                      // Stores a pointer to an OpenGLTexture
    bool                m_texture_loaded = false;       // True once a texture has been loaded
    int                 m_width;                        // Width of texture
    int                 m_height;                       // Height of texture

public:
    // Constructor / Destructor
    DrEngineTexture(QPixmap &from_pixmap);

    // Function Calls
    void            deleteTexture()   { delete m_texture; }
    void            loadTexture(QPixmap &from_pixmap);

    // Getters & Setters
    const int&      height() { return m_height; }
    const int&      width()  { return m_width; }
    const bool&     isTextureLoaded() { return m_texture_loaded; }
    QOpenGLTexture* texture()         { return m_texture; }
};



#endif // ENGINE_TEXTURE_H





