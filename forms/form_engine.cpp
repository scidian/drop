//
//      Created by Stephens Nunnally on 5/1/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>

#include <QApplication>
#include <QGraphicsLineItem>
#include <QLayout>
#include <QMessageBox>
#include <QOpenGLWidget>
#include <QRandomGenerator>
#include <QTimer>
#include <QWindow>

#include "forms/form_engine.h"
#include "opengl/opengl.h"
#include "engine/engine.h"
#include "style/style.h"

//######################################################################################################
//##    Constructor / Destructor
//######################################################################################################
FormEngine::FormEngine(DrProject *project, QWidget *parent) : QMainWindow(parent), m_project(project) {
    this->setObjectName("Drop Player");
    this->resize(1200, 800);
    Dr::ApplyCustomStyleSheetFormatting(this);

    m_engine = new DrEngine(project);

    centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        layout->setContentsMargins(3, 3, 3, 3);

        QWidget *upperWidget = new QWidget();
        upperWidget->setFixedHeight(100);
        pushSpawn =   new QPushButton(upperWidget);     pushSpawn->setObjectName("pushSpawn");      pushSpawn->setGeometry(  QRect(  5,  0, 140, 24));
        pushCar =     new QPushButton(upperWidget);     pushCar->setObjectName("pushCar");          pushCar->setGeometry(    QRect(  5, 24, 140, 24));
        pushJump =    new QPushButton(upperWidget);     pushJump->setObjectName("pushJump");        pushJump->setGeometry(   QRect(  5, 48, 140, 24));
        pushProject = new QPushButton(upperWidget);     pushProject->setObjectName("pushProject");  pushProject->setGeometry(QRect(  5, 72, 140, 24));

        pushStart =  new QPushButton(upperWidget);      pushStart->setObjectName("pushStart");      pushStart->setGeometry(  QRect(150, 24, 140, 24));
        pushStop =   new QPushButton(upperWidget);      pushStop->setObjectName("pushStop");        pushStop->setGeometry(   QRect(150, 48, 140, 24));

        pushPersp =   new QPushButton(upperWidget);     pushPersp->setObjectName("pushPersp");      pushPersp->setGeometry(  QRect(300, 24, 140, 24));
        pushOrtho =   new QPushButton(upperWidget);     pushOrtho->setObjectName("pushOrtho");      pushOrtho->setGeometry(  QRect(300, 48, 140, 24));

        pushDebug =   new QPushButton(upperWidget);     pushDebug->setObjectName("pushDebug");      pushDebug->setGeometry(  QRect(450, 24, 140, 50));

        label =       new QLabel(upperWidget);          label->setObjectName("label");              label->setGeometry(      QRect(600,  0, 330, 20));
        label2 =      new QLabel(upperWidget);          label2->setObjectName("label2");            label2->setGeometry(     QRect(600, 25, 330, 20));
        labelOpenGL = new QLabel(upperWidget);          labelOpenGL->setObjectName("labelOpenGL");  labelOpenGL->setGeometry(QRect(600, 50, 330, 20));

        pushSpawn->setText(QApplication::translate(  "MainWindow", "Spawning Demo",     nullptr));
        pushCar->setText(QApplication::translate(    "MainWindow", "Car Demo",          nullptr));
        pushJump->setText(QApplication::translate(   "MainWindow", "Jump Demo",         nullptr));
        pushProject->setText(QApplication::translate("MainWindow", "Project Demo",      nullptr));

        pushStart->setText(QApplication::translate(  "MainWindow", "Start Scene",       nullptr));
        pushStop->setText(QApplication::translate(   "MainWindow", "Stop Scene",        nullptr));

        pushPersp->setText(QApplication::translate(  "MainWindow", "Perspective View",  nullptr));
        pushOrtho->setText(QApplication::translate(  "MainWindow", "Orthographic View", nullptr));
        pushDebug->setText(QApplication::translate(  "MainWindow", "Toggle Debug",      nullptr));
        layout->addWidget(upperWidget);

        connect(pushSpawn,   SIGNAL(clicked()), this, SLOT(on_pushSpawn_clicked()));
        connect(pushCar,     SIGNAL(clicked()), this, SLOT(on_pushCar_clicked()));
        connect(pushJump,    SIGNAL(clicked()), this, SLOT(on_pushJump_clicked()));
        connect(pushProject, SIGNAL(clicked()), this, SLOT(on_pushProject_clicked()));

        connect(pushStart,   SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
        connect(pushStop,    SIGNAL(clicked()), this, SLOT(on_pushButton2_clicked()));

        connect(pushPersp,   SIGNAL(clicked()), this, SLOT(on_pushPersp_clicked()));
        connect(pushOrtho,   SIGNAL(clicked()), this, SLOT(on_pushOrtho_clicked()));

        connect(pushDebug,   SIGNAL(clicked()), this, SLOT(on_pushDebug_clicked()));



        m_opengl = new OpenGL(centralWidget, m_engine);
        connect(m_opengl, SIGNAL(updateInfo(QString)), this, SLOT(updateLabels(QString)));
        layout->addWidget(m_opengl);

    this->setCentralWidget(centralWidget);


    // Create engine timer and connect
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateEngine()));
}

FormEngine::~FormEngine() { }

void FormEngine::closeEvent(QCloseEvent *) {
    stopTimers();
    qApp->processEvents();
}




//######################################################################################################
//##    Button Handling
//######################################################################################################
void FormEngine::on_pushButton_clicked() {
    if (!m_engine->has_scene) return;
    startTimers();
}
void FormEngine::on_pushButton2_clicked() {
    stopTimers();
}

void FormEngine::on_pushDebug_clicked() {
    m_engine->debug = !m_engine->debug;
    m_opengl->update();
}

void FormEngine::on_pushSpawn_clicked() {
    stopTimers();
    m_engine->clearSpace();
    m_engine->demo = Demo::Spawn;
    m_engine->buildSpace();
}

void FormEngine::on_pushCar_clicked() {
    stopTimers();
    m_engine->clearSpace();
    m_engine->demo = Demo::Car;
    m_engine->buildSpace();
}

void FormEngine::on_pushJump_clicked() {
    stopTimers();
    m_engine->clearSpace();
    m_engine->demo = Demo::Jump;
    m_engine->buildSpace();
}

void FormEngine::on_pushProject_clicked() {
    stopTimers();
    m_engine->clearSpace();
    qApp->processEvents();
    m_engine->demo = Demo::Project;
    m_engine->buildSpace();
}


void FormEngine::on_pushPersp_clicked() { m_engine->render_type = Render_Type::Perspective;  }
void FormEngine::on_pushOrtho_clicked() { m_engine->render_type = Render_Type::Orthographic; }


//######################################################################################################
//##    Update Engine /  Labels
//######################################################################################################
void FormEngine::startTimers() {
    m_timer->start( 1 );
    m_time_last_render.restart();
    m_time_last_update.restart();
    m_engine->fps_timer.restart();
    m_engine->fps = 0;
}
void FormEngine::stopTimers() {
     m_timer->stop();
}

void FormEngine::updateEngine() {
    if (m_time_last_update.elapsed() > (m_engine->getTimeStep() * 1000)) {
        m_engine->updateSpace(m_time_last_update.elapsed());

        m_opengl->cube_angle += static_cast<float>(m_time_last_update.elapsed()) * 0.5f;
        if (m_opengl->cube_angle > 360) m_opengl->cube_angle = 0;

        m_time_last_update.restart();
    }

    // This would limit to a particular frames per second, for now calling it as much as possible
    ///if (m_time_last_render.elapsed() > 1000 / m_ideal_frames_per_second) {
        m_opengl->update();
    ///    m_time_last_render.restart(); }
}

// Update helpful labels
void FormEngine::updateLabels(QString info) {
    label->setText( "Total Items: " + QString::number( m_engine->objects.count()) + info);
    label2->setText("FPS: " + QString::number(m_engine->fps) + " - Scale: " + QString::number(double(m_opengl->getScale())) );

    int max_sample, max_text, max_number_textures, max_layers;
    glGetIntegerv ( GL_MAX_SAMPLES, &max_sample );                                      // Finds max multi sampling available on system
    glGetIntegerv ( GL_MAX_TEXTURE_SIZE, &max_text );                                   // Finds max texture size available on system
    glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_number_textures );        // Finds max number of textures can bind at one time
    glGetIntegerv ( GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers );
    labelOpenGL->setText( "MAX Samples: " +  QString::number(max_sample) +
                        ", Txt Size: " +   QString::number(max_text) +
                        ", Txt Layers: " + QString::number(max_layers));
}









