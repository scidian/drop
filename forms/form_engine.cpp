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
#include "engine/engine_camera.h"
#include "style/style.h"

//######################################################################################################
//##    Constructor / Destructor
//######################################################################################################
FormEngine::FormEngine(DrProject *project, QWidget *parent) : QMainWindow(parent), m_project(project) {
    this->setObjectName("Drop Player");
    this->resize(1200, 900);
    Dr::ApplyCustomStyleSheetFormatting(this);
    Dr::CenterFormOnScreen(parent, this);

    m_engine = new DrEngine(project);

    centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        layout->setContentsMargins(3, 3, 3, 3);

        QWidget *upperWidget = new QWidget();
        upperWidget->setFixedHeight(100);
        pushSpawn =   new QToolButton(upperWidget);     pushSpawn->setObjectName("pushSpawn");      pushSpawn->setGeometry(  QRect(  5,  5, 140, 20));
        pushCar =     new QToolButton(upperWidget);     pushCar->setObjectName("pushCar");          pushCar->setGeometry(    QRect(  5, 30, 140, 20));
        pushJump =    new QToolButton(upperWidget);     pushJump->setObjectName("pushJump");        pushJump->setGeometry(   QRect(  5, 55, 140, 20));

        pushLine1 =   new QToolButton(upperWidget);     pushLine1->setObjectName("pushLine1");      pushLine1->setGeometry(  QRect(150,  5, 140, 20));
        pushLine2 =   new QToolButton(upperWidget);     pushLine2->setObjectName("pushLine2");      pushLine2->setGeometry(  QRect(150, 30, 140, 20));
        pushBlocks =  new QToolButton(upperWidget);     pushBlocks->setObjectName("pushBlocks");    pushBlocks->setGeometry( QRect(150, 55, 140, 20));
        pushProject = new QToolButton(upperWidget);     pushProject->setObjectName("pushProject");  pushProject->setGeometry(QRect(150, 80, 140, 20));

        pushStart =  new QToolButton(upperWidget);      pushStart->setObjectName("pushStart");      pushStart->setGeometry(  QRect(300, 24, 140, 20));
        pushStop =   new QToolButton(upperWidget);      pushStop->setObjectName("pushStop");        pushStop->setGeometry(   QRect(300, 48, 140, 20));

        pushPersp =   new QToolButton(upperWidget);     pushPersp->setObjectName("pushPersp");      pushPersp->setGeometry(  QRect(450, 24, 140, 20));
        pushOrtho =   new QToolButton(upperWidget);     pushOrtho->setObjectName("pushOrtho");      pushOrtho->setGeometry(  QRect(450, 48, 140, 20));

        pushDebug1 =  new QToolButton(upperWidget);     pushDebug1->setObjectName("pushDebug1");    pushDebug1->setGeometry( QRect(600,  5, 140, 45));
        pushDebug2 =  new QToolButton(upperWidget);     pushDebug2->setObjectName("pushDebug2");    pushDebug2->setGeometry( QRect(600, 55, 140, 45));

        label =       new QLabel(upperWidget);          label->setObjectName("label");              label->setGeometry(      QRect(750,  0, 330, 20));
        label2 =      new QLabel(upperWidget);          label2->setObjectName("label2");            label2->setGeometry(     QRect(750, 25, 330, 20));
        labelOpenGL = new QLabel(upperWidget);          labelOpenGL->setObjectName("labelOpenGL");  labelOpenGL->setGeometry(QRect(750, 50, 330, 20));

        pushSpawn->setText(QApplication::translate(  "MainWindow", "Spawning Demo",     nullptr));  pushSpawn->setStyleSheet("color: white");
        pushCar->setText(QApplication::translate(    "MainWindow", "Car Demo",          nullptr));  pushCar->setStyleSheet("color: white");
        pushJump->setText(QApplication::translate(   "MainWindow", "Jump Demo",         nullptr));  pushJump->setStyleSheet("color: white");

        pushLine1->setText(QApplication::translate(  "MainWindow", "Line1 World",       nullptr));  pushLine1->setStyleSheet("color: white");
        pushLine2->setText(QApplication::translate(  "MainWindow", "Line2 World",       nullptr));  pushLine2->setStyleSheet("color: white");
        pushBlocks->setText(QApplication::translate( "MainWindow", "Blocks World",      nullptr));  pushBlocks->setStyleSheet("color: white");
        pushProject->setText(QApplication::translate("MainWindow", "Load Project",      nullptr));  pushProject->setStyleSheet("color: white");

        pushStart->setText(QApplication::translate(  "MainWindow", "Start Scene",       nullptr));  pushStart->setStyleSheet("color: white");
        pushStop->setText(QApplication::translate(   "MainWindow", "Stop Scene",        nullptr));  pushStop->setStyleSheet("color: white");

        pushPersp->setText(QApplication::translate(  "MainWindow", "Perspective View",  nullptr));  pushPersp->setStyleSheet("color: white");
        pushOrtho->setText(QApplication::translate(  "MainWindow", "Orthographic View", nullptr));  pushOrtho->setStyleSheet("color: white");

        pushDebug1->setText(QApplication::translate(  "MainWindow", "Debug Shapes",     nullptr));  pushDebug1->setStyleSheet("color: white");
        pushDebug2->setText(QApplication::translate(  "MainWindow", "Debug Collision",  nullptr));  pushDebug2->setStyleSheet("color: white");

        updateCheckedButtons();
        layout->addWidget(upperWidget);


        connect(pushSpawn,   SIGNAL(clicked()), this, SLOT(on_pushSpawn_clicked()));
        connect(pushCar,     SIGNAL(clicked()), this, SLOT(on_pushCar_clicked()));
        connect(pushJump,    SIGNAL(clicked()), this, SLOT(on_pushJump_clicked()));

        connect(pushLine1,   SIGNAL(clicked()), this, SLOT(on_pushLines1_clicked()));
        connect(pushLine2,   SIGNAL(clicked()), this, SLOT(on_pushLines2_clicked()));
        connect(pushBlocks,  SIGNAL(clicked()), this, SLOT(on_pushBlocks_clicked()));
        connect(pushProject, SIGNAL(clicked()), this, SLOT(on_pushProject_clicked()));

        connect(pushStart,   SIGNAL(clicked()), this, SLOT(on_pushStart_clicked()));
        connect(pushStop,    SIGNAL(clicked()), this, SLOT(on_pushStop_clicked()));

        connect(pushPersp,   SIGNAL(clicked()), this, SLOT(on_pushPersp_clicked()));
        connect(pushOrtho,   SIGNAL(clicked()), this, SLOT(on_pushOrtho_clicked()));

        connect(pushDebug1,  SIGNAL(clicked()), this, SLOT(on_pushDebug1_clicked()));
        connect(pushDebug2,  SIGNAL(clicked()), this, SLOT(on_pushDebug2_clicked()));


        m_opengl = new OpenGL(centralWidget, m_engine);
        connect(m_opengl, SIGNAL(updateInfo(QString)), this, SLOT(updateLabels(QString)));
        layout->addWidget(m_opengl);

    this->setCentralWidget(centralWidget);


    // Create engine timer and connect
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateEngine()));


    // Load demo after form finishes loading
    QTimer::singleShot( 0, this, [this] {
        loadDemo( m_engine->demo_space, m_engine->demo_player );
    } );
}

FormEngine::~FormEngine() { }

void FormEngine::closeEvent(QCloseEvent *) {
    stopTimers();
    qApp->processEvents();
}


void FormEngine::loadDemo(Demo_Space using_space, Demo_Player using_player ) {
    stopTimers();

    // ***** The following are the steps to load a new Space
    m_engine->clearSpace();
    m_engine->buildSpace( using_space );
    m_engine->addPlayer( using_player );
    m_engine->updateSpace( 0 );

    // Finished loading Space, update buttons and start timer
    updateCheckedButtons();
    m_engine->has_scene = true;
    startTimers();
}


//######################################################################################################
//##    Update Engine /  Labels
//######################################################################################################
void FormEngine::startTimers() {
    m_timer->start( 1 );
    m_time_update = Clock::now();
    m_time_camera = Clock::now();
    m_engine->fps_timer.restart();
    m_engine->fps = 0;
}
void FormEngine::stopTimers() {
     m_timer->stop();
}

void FormEngine::updateEngine() {

    if (!m_engine->has_scene) return;

    // ***** Move Cameras
    double camera_milliseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_camera).count() / 1000000.0;
    if (camera_milliseconds > 1) {
        m_engine->moveCameras(static_cast<float>(camera_milliseconds));
        m_time_camera = Clock::now();
    }

    // ***** MAIN UPDATE LOOP: Space (Physics), Render, Cameras
    double update_milliseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_update).count() / 1000000.0;
    if (update_milliseconds > (m_engine->getTimeStep() * 1000)) {
        m_engine->updateSpace(update_milliseconds);     // Physics
        m_engine->updateCameras();                      // Cameras
        m_opengl->update();                             // Render
        m_time_update = Clock::now();
    }
}

// Update helpful labels
void FormEngine::updateLabels(QString info) {
    label->setText( "Total Items: " + QString::number( m_engine->objects.count()) + " - " + info);
    label2->setText("FPS: " + QString::number(m_engine->last_fps) + " - Scale: " + QString::number(double(m_opengl->getScale())) );

    int max_sample, max_text, max_number_textures, max_layers;
    glGetIntegerv ( GL_MAX_SAMPLES, &max_sample );                                      // Finds max multi sampling available on system
    glGetIntegerv ( GL_MAX_TEXTURE_SIZE, &max_text );                                   // Finds max texture size available on system
    glGetIntegerv ( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_number_textures );        // Finds max number of textures can bind at one time
    glGetIntegerv ( GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers );
    labelOpenGL->setText( "MAX Samples: " +  QString::number(max_sample) +
                        ", Txt Size: " +   QString::number(max_text) +
                        ", Txt Layers: " + QString::number(max_layers));
}




//######################################################################################################
//##    Button Handling
//######################################################################################################
void FormEngine::on_pushSpawn_clicked() {   loadDemo(m_engine->demo_space,  Demo_Player::Spawn ); }
void FormEngine::on_pushCar_clicked() {     loadDemo(m_engine->demo_space,  Demo_Player::Car ); }
void FormEngine::on_pushJump_clicked() {    loadDemo(m_engine->demo_space,  Demo_Player::Jump ); }

void FormEngine::on_pushLines1_clicked() {  loadDemo(Demo_Space::Lines1,    m_engine->demo_player ); }
void FormEngine::on_pushLines2_clicked() {  loadDemo(Demo_Space::Lines2,    m_engine->demo_player ); }
void FormEngine::on_pushBlocks_clicked() {  loadDemo(Demo_Space::Blocks,    m_engine->demo_player ); }
void FormEngine::on_pushProject_clicked() { loadDemo(Demo_Space::Project,   m_engine->demo_player ); }


void FormEngine::on_pushStart_clicked() {
    if (!m_engine->has_scene) return;
    startTimers();
}
void FormEngine::on_pushStop_clicked() {
    stopTimers();
}

void FormEngine::on_pushPersp_clicked() { m_engine->render_type = Render_Type::Perspective;  updateCheckedButtons(); }
void FormEngine::on_pushOrtho_clicked() { m_engine->render_type = Render_Type::Orthographic; updateCheckedButtons(); }

void FormEngine::on_pushDebug1_clicked() {
    m_engine->debug_shapes = !m_engine->debug_shapes;
    m_opengl->update();
    updateCheckedButtons();
}
void FormEngine::on_pushDebug2_clicked() {
    m_engine->debug_collisions = !m_engine->debug_collisions;
    m_opengl->update();
    updateCheckedButtons();
}


//######################################################################################################
//##    Update Checked Buttons
//######################################################################################################
void FormEngine::updateCheckedButtons() {
    if (m_engine->demo_player == Demo_Player::Spawn) pushSpawn->setDown(true);   else pushSpawn->setDown(false);
    if (m_engine->demo_player == Demo_Player::Car)   pushCar->setDown(true);     else pushCar->setDown(false);
    if (m_engine->demo_player == Demo_Player::Jump)  pushJump->setDown(true);    else pushJump->setDown(false);

    if (m_engine->demo_space == Demo_Space::Lines1)  pushLine1->setDown(true);   else pushLine1->setDown(false);
    if (m_engine->demo_space == Demo_Space::Lines2)  pushLine2->setDown(true);   else pushLine2->setDown(false);
    if (m_engine->demo_space == Demo_Space::Blocks)  pushBlocks->setDown(true);  else pushBlocks->setDown(false);
    if (m_engine->demo_space == Demo_Space::Project) pushProject->setDown(true); else pushProject->setDown(false);

    if (m_engine->render_type == Render_Type::Perspective)  pushPersp->setDown(true); else pushPersp->setDown(false);
    if (m_engine->render_type == Render_Type::Orthographic) pushOrtho->setDown(true); else pushOrtho->setDown(false);

    if (m_engine->debug_shapes)     pushDebug1->setDown(true); else pushDebug1->setDown(false);
    if (m_engine->debug_collisions) pushDebug2->setDown(true); else pushDebug2->setDown(false);
}
















