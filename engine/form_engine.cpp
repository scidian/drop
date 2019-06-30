//
//      Created by Stephens Nunnally on 5/1/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>

#include <QApplication>
#include <QCloseEvent>
#include <QGraphicsLineItem>
#include <QLayout>
#include <QMessageBox>
#include <QOpenGLWidget>
#include <QRandomGenerator>
#include <QTimer>
#include <QWindow>

#include "engine.h"
#include "engine_camera.h"
#include "engine_thing_object.h"
#include "engine_world.h"
#include "form_engine.h"
#include "opengl/opengl.h"
#include "style/style.h"

//####################################################################################
//##    Constructor / Destructor
//####################################################################################
FormEngine::FormEngine(DrProject *project, QWidget *parent) : QMainWindow(parent), m_project(project) {

    // ***** Set up initial window
    this->setAttribute( Qt::WA_DeleteOnClose, true );               // Make sure this form is deleted when it closes
    this->setAttribute( Qt::WA_QuitOnClose, false );                // We don't want Drop to stay open when FormMain is closed and this is still open
    this->setObjectName("Drop Player");
    this->resize(1200, 900);
    Dr::ApplyCustomStyleSheetFormatting(this);
    Dr::CenterFormOnScreen(parent, this);

    // ***** Create an instance of the game engine
    m_engine = new DrEngine(this, project);

    // ***** Build this form
    centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        layout->setContentsMargins(3, 3, 3, 3);

        QWidget *upperWidget = new QWidget();
        upperWidget->setFixedHeight(100);
        pushSpawn =   new QToolButton(upperWidget);     pushSpawn->setObjectName("pushSpawn");      pushSpawn->setGeometry(  QRect(  5,  5, 140, 20));
        pushCar =     new QToolButton(upperWidget);     pushCar->setObjectName("pushCar");          pushCar->setGeometry(    QRect(  5, 30, 140, 20));
        pushJump =    new QToolButton(upperWidget);     pushJump->setObjectName("pushJump");        pushJump->setGeometry(   QRect(  5, 55, 140, 20));
        pushCamera =  new QToolButton(upperWidget);     pushCamera->setObjectName("pushCamera");    pushCamera->setGeometry( QRect( 35, 80,  80, 20));

        pushLine1 =   new QToolButton(upperWidget);     pushLine1->setObjectName("pushLine1");      pushLine1->setGeometry(  QRect(150,  5, 140, 20));
        pushLine2 =   new QToolButton(upperWidget);     pushLine2->setObjectName("pushLine2");      pushLine2->setGeometry(  QRect(150, 30, 140, 20));
        pushBlocks =  new QToolButton(upperWidget);     pushBlocks->setObjectName("pushBlocks");    pushBlocks->setGeometry( QRect(150, 55, 140, 20));
        pushProject = new QToolButton(upperWidget);     pushProject->setObjectName("pushProject");  pushProject->setGeometry(QRect(150, 80, 140, 20));

        pushStart =  new QToolButton(upperWidget);      pushStart->setObjectName("pushStart");      pushStart->setGeometry(  QRect(300, 24, 140, 20));
        pushStop =   new QToolButton(upperWidget);      pushStop->setObjectName("pushStop");        pushStop->setGeometry(   QRect(300, 48, 140, 20));
        pushClose =   new QToolButton(upperWidget);     pushClose->setObjectName("pushClose");      pushClose->setGeometry(  QRect(320, 72, 100, 20));

        pushPersp =   new QToolButton(upperWidget);     pushPersp->setObjectName("pushPersp");      pushPersp->setGeometry(  QRect(450, 24, 140, 20));
        pushOrtho =   new QToolButton(upperWidget);     pushOrtho->setObjectName("pushOrtho");      pushOrtho->setGeometry(  QRect(450, 48, 140, 20));

        pushDebug1 =  new QToolButton(upperWidget);     pushDebug1->setObjectName("pushDebug1");    pushDebug1->setGeometry( QRect(600,  5, 140, 45));
        pushDebug2 =  new QToolButton(upperWidget);     pushDebug2->setObjectName("pushDebug2");    pushDebug2->setGeometry( QRect(600, 55, 140, 45));

        pushSpawn->setText(QApplication::translate(  "MainWindow", "Spawning Demo",     nullptr));  pushSpawn->setStyleSheet("color: white");
        pushCar->setText(QApplication::translate(    "MainWindow", "Car Demo",          nullptr));  pushCar->setStyleSheet("color: white");
        pushJump->setText(QApplication::translate(   "MainWindow", "Jump Demo",         nullptr));  pushJump->setStyleSheet("color: white");
        pushCamera->setText(QApplication::translate( "MainWindow", "Cam Switch",        nullptr));  pushCamera->setStyleSheet("color: white");

        pushLine1->setText(QApplication::translate(  "MainWindow", "Line1 World",       nullptr));  pushLine1->setStyleSheet("color: white");
        pushLine2->setText(QApplication::translate(  "MainWindow", "Line2 World",       nullptr));  pushLine2->setStyleSheet("color: white");
        pushBlocks->setText(QApplication::translate( "MainWindow", "Blocks World",      nullptr));  pushBlocks->setStyleSheet("color: white");
        pushProject->setText(QApplication::translate("MainWindow", "Load Project",      nullptr));  pushProject->setStyleSheet("color: white");

        pushStart->setText(QApplication::translate(  "MainWindow", "Start Scene",       nullptr));  pushStart->setStyleSheet("color: white");
        pushStop->setText(QApplication::translate(   "MainWindow", "Stop Scene",        nullptr));  pushStop->setStyleSheet("color: white");
        pushClose->setText(QApplication::translate(  "MainWindow", "Close",             nullptr));  pushClose->setStyleSheet("color: white");

        pushPersp->setText(QApplication::translate(  "MainWindow", "Perspective View",  nullptr));  pushPersp->setStyleSheet("color: white");
        pushOrtho->setText(QApplication::translate(  "MainWindow", "Orthographic View", nullptr));  pushOrtho->setStyleSheet("color: white");

        pushDebug1->setText(QApplication::translate( "MainWindow", "Debug Shapes",     nullptr));   pushDebug1->setStyleSheet("color: white");
        pushDebug2->setText(QApplication::translate( "MainWindow", "Debug Collision",  nullptr));   pushDebug2->setStyleSheet("color: white");

        updateCheckedButtons();
        layout->addWidget(upperWidget);


        connect(pushSpawn,   SIGNAL(clicked()), this, SLOT(on_pushSpawn_clicked()));
        connect(pushCar,     SIGNAL(clicked()), this, SLOT(on_pushCar_clicked()));
        connect(pushJump,    SIGNAL(clicked()), this, SLOT(on_pushJump_clicked()));
        connect(pushCamera,  SIGNAL(clicked()), this, SLOT(on_pushCamera_clicked()));

        connect(pushLine1,   SIGNAL(clicked()), this, SLOT(on_pushLines1_clicked()));
        connect(pushLine2,   SIGNAL(clicked()), this, SLOT(on_pushLines2_clicked()));
        connect(pushBlocks,  SIGNAL(clicked()), this, SLOT(on_pushBlocks_clicked()));
        connect(pushProject, SIGNAL(clicked()), this, SLOT(on_pushProject_clicked()));

        connect(pushStart,   SIGNAL(clicked()), this, SLOT(on_pushStart_clicked()));
        connect(pushStop,    SIGNAL(clicked()), this, SLOT(on_pushStop_clicked()));
        connect(pushClose,   SIGNAL(clicked()), this, SLOT(on_pushClose_clicked()));

        connect(pushPersp,   SIGNAL(clicked()), this, SLOT(on_pushPersp_clicked()));
        connect(pushOrtho,   SIGNAL(clicked()), this, SLOT(on_pushOrtho_clicked()));

        connect(pushDebug1,  SIGNAL(clicked()), this, SLOT(on_pushDebug1_clicked()));
        connect(pushDebug2,  SIGNAL(clicked()), this, SLOT(on_pushDebug2_clicked()));


        m_opengl = new DrOpenGL(centralWidget, this, m_engine);
        connect(m_opengl, SIGNAL(frameSwapped()), this, SLOT(frameSwapped()));
        connect(m_opengl, SIGNAL(aboutToCompose()), this, SLOT(aboutToCompose()));
        ///connect(m_opengl, SIGNAL(frameSwapped()), m_opengl, SLOT(update()));             // Creates non-stop updates
        layout->addWidget(m_opengl);

    this->setCentralWidget(centralWidget);


    // Create engine timer and connect
    m_update_timer = new QTimer(this);
    connect(m_update_timer, SIGNAL(timeout()), this, SLOT(updateEngine()));


    // Load demo after form finishes loading
    QTimer::singleShot( 0, this, [this] {
        loadDemo( demo_space, demo_player );
    } );
}

FormEngine::~FormEngine() { }


//####################################################################################
//##    Event Handling
//####################################################################################
void FormEngine::closeEvent(QCloseEvent *event) {
    // Wait for update timer to stop
    stopTimers();
    do { qApp->processEvents(); } while (m_running);

    // Allow window close
    event->accept();
}

// Grabs all events
bool FormEngine::event(QEvent *event) {
    switch(event->type()) {
        case QEvent::WindowActivate:
            m_opengl->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
            break;
        case QEvent::WindowDeactivate:
            // #TODO: Need put sleeping OpenGL code here
            break;
        default:;
    }
    return QMainWindow::event(event);
}


//####################################################################################
//##    Loads a Demo
//####################################################################################
void FormEngine::loadDemo(Demo_Space using_space, Demo_Player using_player) {
    stopTimers();

    // ***** Reset keys
    g_keyboard_x = 0;
    g_keyboard_y = 0;
    g_jump_button = false;
    g_pedal = Pedal::None;

    // Save Demo Types
    demo_player = using_player;
    demo_space = using_space;

    // ***** The following are the steps to load a new Space
    m_engine->getCurrentWorld()->clearWorld();
    m_engine->getCurrentWorld()->buildWorld( using_space, m_engine->getCurrentEditorWorld() );
    m_engine->getCurrentWorld()->addPlayer( using_player );
    m_engine->getCurrentWorld()->updateSpace( 0 );

    // Finished loading Space, update buttons and start timer
    updateCheckedButtons();
    m_engine->getCurrentWorld()->has_scene = true;

    startTimers();
}


//####################################################################################
//##    Update Engine
//####################################################################################
void FormEngine::startTimers() {
    m_time_update =  Clock::now();
    m_time_render =  Clock::now();
    m_time_camera =  Clock::now();
    m_time_physics = Clock::now();
    m_time_frame =   Clock::now();
    m_update_timer->start( 0 );                         // Timeout of zero will call this timeout every pass of the event loop
}
void FormEngine::stopTimers() {
    m_update_timer->stop();
}
double FormEngine::getTimerMilliseconds(Engine_Timer time_since_last) {
    switch (time_since_last) {
        case Engine_Timer::Update:  return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_update).count() /  1000000.0;
        case Engine_Timer::Render:  return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_render).count() /  1000000.0;
        case Engine_Timer::Camera:  return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_camera).count() /  1000000.0;
    }
}
void FormEngine::resetTimer(Engine_Timer timer_to_reset) {
    switch (timer_to_reset) {
        case Engine_Timer::Update:  m_time_update =  Clock::now();      break;
        case Engine_Timer::Render:  m_time_render =  Clock::now();      break;
        case Engine_Timer::Camera:  m_time_camera =  Clock::now();      break;
    }
}

// ***** Seperate Camera Update
void FormEngine::moveCameras() {
    double camera_milliseconds = getTimerMilliseconds(Engine_Timer::Camera);
    resetTimer(Engine_Timer::Camera);
    m_engine->getCurrentWorld()->moveCameras(camera_milliseconds);
    ++fps_count_camera;
}

// ***** MAIN UPDATE LOOP: Space (Physics), Rendering
void FormEngine::updateEngine() {
    if (!m_engine->getCurrentWorld()->has_scene) return;
    m_running = true;

    // ***** Calculates Render Frames per Second
    double fps_milli = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_fps).count() / 1000000.0;
    if (fps_milli > 1000.0) {
        fps_render =  fps_count_render;         fps_count_render =  0;
        fps_physics = fps_count_physics;        fps_count_physics = 0;
        fps_camera =  fps_count_camera;         fps_count_camera =  0;
        m_time_fps =  Clock::now();
    }

    // ***** Update Physics and Render
#if defined (Q_OS_MACOS)
    double update_milliseconds = getTimerMilliseconds(Engine_Timer::Update);
    if (update_milliseconds > m_engine->getCurrentWorld()->getTimeStepAsMilliseconds()) {
        resetTimer(Engine_Timer::Update);
        m_engine->getCurrentWorld()->updateSpace(update_milliseconds);                      // Physics Engine
        m_engine->getCurrentWorld()->updateWorld(update_milliseconds);                      // Additional World / Thing Updating
        ++m_engine->getFormEngine()->fps_count_physics;                                     // Updates Physics Frames per Second
        m_engine->getCurrentWorld()->updateCameras();                                       // Update Camera Targets
        moveCameras();                                                                      // Move Cameras
        m_opengl->update();                                                                 // Render
    }

    // Additional render on MacOS (smooths more with vsync being disabled)
    if (m_engine->getCurrentWorld()->getThings().count() < 250 && m_engine->getCurrentWorld()->light_count <= 0) {
        double render_milliseconds = getTimerMilliseconds(Engine_Timer::Render);
        if (render_milliseconds > (1000.0 / m_ideal_frames_per_second)) {
            resetTimer(Engine_Timer::Render);
            m_opengl->update();                                                             // Render
        }
    }
#else
    if (m_wait_vsync == false) {
        m_wait_vsync = true;
        double update_milliseconds = getTimerMilliseconds(Engine_Timer::Update);
        resetTimer(Engine_Timer::Update);
        m_engine->getCurrentWorld()->updateSpace(update_milliseconds);                      // Physics Engine
        m_engine->getCurrentWorld()->updateWorld(update_milliseconds);                      // Additional World / Thing Updating
        ++m_engine->getFormEngine()->fps_count_physics;                                     // Updates Physics Frames per Second
        m_engine->getCurrentWorld()->updateCameras();                                       // Update Camera Targets
        moveCameras();                                                                      // Move Cameras
        m_opengl->update();                                                                 // Render
    }
#endif

    m_running = false;
}

// Emitted by QOpenGLWidget when back buffer is swapped to screen
void FormEngine::frameSwapped() {
    m_wait_vsync = false;
}

void FormEngine::aboutToCompose() {

}


//####################################################################################
//##    Button Handling
//####################################################################################
void FormEngine::on_pushSpawn_clicked() {   loadDemo(demo_space,  Demo_Player::Spawn ); }
void FormEngine::on_pushCar_clicked() {     loadDemo(demo_space,  Demo_Player::Car ); }
void FormEngine::on_pushJump_clicked() {    loadDemo(demo_space,  Demo_Player::Jump ); }

void FormEngine::on_pushCamera_clicked() {
    m_engine->getCurrentWorld()->switchCameraToNext();
}

void FormEngine::on_pushLines1_clicked() {  loadDemo(Demo_Space::Lines1,    demo_player ); }
void FormEngine::on_pushLines2_clicked() {  loadDemo(Demo_Space::Lines2,    demo_player ); }
void FormEngine::on_pushBlocks_clicked() {  loadDemo(Demo_Space::Blocks,    demo_player ); }
void FormEngine::on_pushProject_clicked() { loadDemo(Demo_Space::Project,   demo_player ); }


void FormEngine::on_pushStart_clicked() {
    if (!m_engine->getCurrentWorld()->has_scene) return;
    startTimers();
}
void FormEngine::on_pushStop_clicked() {
    stopTimers();
}
void FormEngine::on_pushClose_clicked() {
    this->close();
}

void FormEngine::on_pushPersp_clicked() { m_engine->getCurrentWorld()->render_type = Render_Type::Perspective;  updateCheckedButtons(); }
void FormEngine::on_pushOrtho_clicked() { m_engine->getCurrentWorld()->render_type = Render_Type::Orthographic; updateCheckedButtons(); }

void FormEngine::on_pushDebug1_clicked() {
    debug_shapes = !debug_shapes;
    updateCheckedButtons();
}
void FormEngine::on_pushDebug2_clicked() {
    debug_collisions = !debug_collisions;
    updateCheckedButtons();
}


//####################################################################################
//##    Update Checked Buttons
//####################################################################################
void FormEngine::updateCheckedButtons() {
    if (demo_player == Demo_Player::Spawn) pushSpawn->setDown(true);   else pushSpawn->setDown(false);
    if (demo_player == Demo_Player::Car)   pushCar->setDown(true);     else pushCar->setDown(false);
    if (demo_player == Demo_Player::Jump)  {
        pushJump->setDown(true);
        pushCamera->setEnabled(true);
    } else {
        pushJump->setDown(false);
        pushCamera->setEnabled(false);
    }

    if (demo_space == Demo_Space::Lines1)  pushLine1->setDown(true);   else pushLine1->setDown(false);
    if (demo_space == Demo_Space::Lines2)  pushLine2->setDown(true);   else pushLine2->setDown(false);
    if (demo_space == Demo_Space::Blocks)  pushBlocks->setDown(true);  else pushBlocks->setDown(false);
    if (demo_space == Demo_Space::Project) pushProject->setDown(true); else pushProject->setDown(false);

    if (m_engine->getCurrentWorld()->render_type == Render_Type::Perspective)  pushPersp->setDown(true); else pushPersp->setDown(false);
    if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) pushOrtho->setDown(true); else pushOrtho->setDown(false);

    if (debug_shapes)     pushDebug1->setDown(true); else pushDebug1->setDown(false);
    if (debug_collisions) pushDebug2->setDown(true); else pushDebug2->setDown(false);
}
















