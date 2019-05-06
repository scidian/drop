//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "forms/form_engine.h"
#include "physics/physics_opengl.h"


//####################################################################################
//##        Constructor / Destructor
//####################################################################################
PhysicsOpenGL::PhysicsOpenGL(QWidget *parent, FormEngine *main_window) : QOpenGLWidget(parent), m_parent(main_window) {
    // ***** DO NOT perform any OpenGL resource initialization in constructor, use initializeGL() instead

}

PhysicsOpenGL::~PhysicsOpenGL() { }

void PhysicsOpenGL::cleanUp() {
    makeCurrent();

    // Destroy OpenGL Resources
    delete m_parent->t_ball;
    delete m_parent->t_metal_block;
    delete m_parent->t_moon_plant;
    delete m_parent->t_rover_body;
    delete m_parent->t_rover_wheel;
    delete m_parent->t_spare_wheel;

    doneCurrent();
}


//####################################################################################
//##        Handle Window Resizing
//####################################################################################
void PhysicsOpenGL::resizeGL(int w, int h) {
    glViewport( 0, 0, w, h );
    m_center_of_widget = QPoint( w / 2, h / 2);
}



















