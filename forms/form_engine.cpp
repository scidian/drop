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

//######################################################################################################
//##    Constructor / Destructor
//######################################################################################################
FormEngine::FormEngine(QWidget *parent) : QMainWindow(parent) {
    this->setObjectName("Drop Player");
    this->resize(1200, 800);

    m_engine = new DrEngine();

    centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        QWidget *upperWidget = new QWidget();
        upperWidget->setFixedHeight(90);
        pushButton =  new QPushButton(upperWidget);     pushButton->setObjectName("pushButton");    pushButton->setGeometry( QRect( 10, 10, 141, 31));
        pushButton2 = new QPushButton(upperWidget);     pushButton2->setObjectName("pushButton2");  pushButton2->setGeometry(QRect( 10, 50, 141, 31));
        pushDebug =   new QPushButton(upperWidget);     pushDebug->setObjectName("pushDebug");      pushDebug->setGeometry(  QRect(830, 10, 141, 71));
        pushSpawn =   new QPushButton(upperWidget);     pushSpawn->setObjectName("pushSpawn");      pushSpawn->setGeometry(  QRect(700, 10, 141, 31));
        pushCar =     new QPushButton(upperWidget);     pushCar->setObjectName("pushCar");          pushCar->setGeometry(    QRect(700, 50, 141, 31));
        pushPersp =   new QPushButton(upperWidget);     pushPersp->setObjectName("pushPersp");      pushPersp->setGeometry(  QRect(520, 10, 141, 31));
        pushOrtho =   new QPushButton(upperWidget);     pushOrtho->setObjectName("pushOrtho");      pushOrtho->setGeometry(  QRect(520, 50, 141, 31));
        label =       new QLabel(upperWidget);          label->setObjectName("label");              label->setGeometry(      QRect(190, 05, 330, 20));
        label2 =      new QLabel(upperWidget);          label2->setObjectName("label2");            label2->setGeometry(     QRect(190, 25, 330, 20));
        labelOpenGL = new QLabel(upperWidget);          labelOpenGL->setObjectName("labelOpenGL");  labelOpenGL->setGeometry(QRect(190, 50, 330, 20));

        pushButton->setText(QApplication::translate( "MainWindow", "Start Scene",       nullptr));
        pushButton2->setText(QApplication::translate("MainWindow", "Stop Scene",        nullptr));
        pushDebug->setText(QApplication::translate(  "MainWindow", "Toggle Debug",      nullptr));
        pushSpawn->setText(QApplication::translate(  "MainWindow", "Spawning Demo",     nullptr));
        pushCar->setText(QApplication::translate(    "MainWindow", "Car Demo",          nullptr));
        pushPersp->setText(QApplication::translate(  "MainWindow", "Perspective View",  nullptr));
        pushOrtho->setText(QApplication::translate(  "MainWindow", "Orthographic View", nullptr));
        layout->addWidget(upperWidget);

        connect(pushButton,  SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
        connect(pushButton2, SIGNAL(clicked()), this, SLOT(on_pushButton2_clicked()));
        connect(pushDebug,   SIGNAL(clicked()), this, SLOT(on_pushDebug_clicked()));
        connect(pushSpawn,   SIGNAL(clicked()), this, SLOT(on_pushSpawn_clicked()));
        connect(pushCar,     SIGNAL(clicked()), this, SLOT(on_pushCar_clicked()));
        connect(pushPersp,   SIGNAL(clicked()), this, SLOT(on_pushPersp_clicked()));
        connect(pushOrtho,   SIGNAL(clicked()), this, SLOT(on_pushOrtho_clicked()));

        m_opengl = new OpenGL(centralWidget, m_engine);
        connect(m_opengl, SIGNAL(updateInfo(QString)), this, SLOT(updateLabels(QString)));
        layout->addWidget(m_opengl);

    this->setCentralWidget(centralWidget);


    // Create scene timer and connect it to UpdateScene
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateEngine()));

}

FormEngine::~FormEngine() { }

void FormEngine::closeEvent(QCloseEvent *) {
    m_timer->stop();
    qApp->processEvents();
}




//######################################################################################################
//##    Button Handling
//######################################################################################################
void FormEngine::on_pushButton_clicked() {
    if (!m_engine->has_scene) return;
    m_timer->start( int(1000 * m_engine->getTimeStep()) );
    m_engine->fps_timer.restart();
    m_engine->fps = 0;
}
void FormEngine::on_pushButton2_clicked() {   m_timer->stop();  }

void FormEngine::on_pushDebug_clicked() {
    m_engine->debug = !m_engine->debug;
    m_opengl->update();
}

void FormEngine::on_pushSpawn_clicked() {
    m_timer->stop();
    m_engine->clearSpace();
    m_engine->demo = Demo::Spawn;
    m_engine->buildSpace();
}

void FormEngine::on_pushCar_clicked() {
    m_timer->stop();
    m_engine->clearSpace();
    m_engine->demo = Demo::Car;
    m_engine->buildSpace();
}

void FormEngine::on_pushPersp_clicked() { m_engine->render_type = Render_Type::Perspective;  }
void FormEngine::on_pushOrtho_clicked() { m_engine->render_type = Render_Type::Orthographic; }


//######################################################################################################
//##    Update Engine /  Labels
//######################################################################################################
void FormEngine::updateEngine() {

    // ***** Update engine and render
    m_engine->updateSpace();
    m_opengl->update();

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









