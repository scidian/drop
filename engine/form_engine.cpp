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

#include "editor/style/style.h"
#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/form_engine.h"
#include "editor/globals_editor.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
FormEngine::FormEngine(DrProject *project, long stage_key, QWidget *parent) : QMainWindow(parent), m_project(project) {

    // ***** Set up initial window
    this->setAttribute( Qt::WA_DeleteOnClose, true );               // Make sure this form is deleted when it closes
    this->setAttribute( Qt::WA_QuitOnClose, false );                // We don't want Drop to stay open when FormMain is closed and this is still open
    this->setObjectName("Drop Player");
    this->setWindowIcon(QIcon(":/assets/icon/icon256_play.png"));

    this->resize(1200, 900);                                        // !!!!! FIX: Change to Project size options
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );
    Dr::CenterFormOnScreen(parent, this);

    // ***** Create an instance of the game engine
    m_engine = new DrEngine(project, stage_key);

    // ***** Build this form
    centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        layout->setContentsMargins(3, 3, 3, 3);

        QWidget *upperWidget = new QWidget();
        upperWidget->setFixedHeight(100);

        pushSpawn =   new QToolButton(upperWidget);     pushSpawn->setObjectName("engineButton");   pushSpawn->setGeometry(  QRect(  5,  5, 140, 20));
        pushCar =     new QToolButton(upperWidget);     pushCar->setObjectName("engineButton");     pushCar->setGeometry(    QRect(  5, 30, 140, 20));
        pushJump =    new QToolButton(upperWidget);     pushJump->setObjectName("engineButton");    pushJump->setGeometry(   QRect(  5, 55, 140, 20));
        pushLight =   new QToolButton(upperWidget);     pushLight->setObjectName("engineButton");   pushLight->setGeometry(  QRect(  5, 80, 140, 20));

        pushPlayer =  new QToolButton(upperWidget);     pushPlayer->setObjectName("engineButton");  pushPlayer->setGeometry( QRect(155,  5, 140, 45));
        pushPause =   new QToolButton(upperWidget);     pushPause->setObjectName("engineButton");   pushPause->setGeometry(  QRect(155, 55,  65, 20));
        pushRestart = new QToolButton(upperWidget);     pushRestart->setObjectName("engineButton"); pushRestart->setGeometry(QRect(225, 55,  65, 20));
        pushClose =   new QToolButton(upperWidget);     pushClose->setObjectName("engineButton");   pushClose->setGeometry(  QRect(175, 80, 100, 20));

        pushPersp =   new QToolButton(upperWidget);     pushPersp->setObjectName("engineButton");   pushPersp->setGeometry(  QRect(305,  5, 140, 20));
        pushOrtho =   new QToolButton(upperWidget);     pushOrtho->setObjectName("engineButton");   pushOrtho->setGeometry(  QRect(305, 30, 140, 20));
        push2D =      new QToolButton(upperWidget);     push2D->setObjectName("engineButton");      push2D->setGeometry(     QRect(320, 55,  50, 20));
        push3D =      new QToolButton(upperWidget);     push3D->setObjectName("engineButton");      push3D->setGeometry(     QRect(378, 55,  50, 20));
        pushCamera =  new QToolButton(upperWidget);     pushCamera->setObjectName("engineButton");  pushCamera->setGeometry( QRect(335, 80,  80, 20));

        pushDebug1 =  new QToolButton(upperWidget);     pushDebug1->setObjectName("engineButton");  pushDebug1->setGeometry( QRect(455,  5, 140, 45));
        pushDebug2 =  new QToolButton(upperWidget);     pushDebug2->setObjectName("engineButton");  pushDebug2->setGeometry( QRect(455, 55, 140, 45));

        pushSpawn->setText(QApplication::translate(  "MainWindow", "Spawning Demo",     nullptr));
        pushCar->setText(QApplication::translate(    "MainWindow", "Car Demo",          nullptr));
        pushJump->setText(QApplication::translate(   "MainWindow", "Jumping Demo",      nullptr));
        pushLight->setText(QApplication::translate(  "MainWindow", "Flashlight Demo",   nullptr));

        pushPlayer->setText(QApplication::translate( "MainWindow", "Project Player",    nullptr));
        pushPause->setText(QApplication::translate(  "MainWindow", "Pause",             nullptr));
        pushRestart->setText(QApplication::translate("MainWindow", "Restart",           nullptr));
        pushClose->setText(QApplication::translate(  "MainWindow", "Close",             nullptr));

        pushPersp->setText(QApplication::translate(  "MainWindow", "Perspective View",  nullptr));
        pushOrtho->setText(QApplication::translate(  "MainWindow", "Orthographic View", nullptr));
        push2D->setText(QApplication::translate(     "MainWindow", "2D",                nullptr));
        push3D->setText(QApplication::translate(     "MainWindow", "3D",                nullptr));
        pushCamera->setText(QApplication::translate( "MainWindow", "Cam Switch",        nullptr));

        QLabel *debug1_label_image = new QLabel(pushDebug1);
            debug1_label_image->setPixmap(QPixmap(":/assets/form_engine_icons/debug_shape.png").scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            debug1_label_image->setGeometry(0, 3, 140, 28);
            debug1_label_image->setAlignment(Qt::AlignCenter);
        QLabel *debug1_label_text = new QLabel(pushDebug1);
            debug1_label_text->setText("Collision Shapes");
            debug1_label_text->setGeometry(0, 28, 140, 15);
            debug1_label_text->setAlignment(Qt::AlignCenter);

        QLabel *debug2_label_image = new QLabel(pushDebug2);
            debug2_label_image->setPixmap(QPixmap(":/assets/form_engine_icons/debug_collide.png").scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            debug2_label_image->setGeometry(0, 1, 140, 30);
            debug2_label_image->setAlignment(Qt::AlignCenter);
        QLabel *debug2_label_text = new QLabel(pushDebug2);
            debug2_label_text->setText("Collision Points");
            debug2_label_text->setGeometry(0, 28, 140, 15);
            debug2_label_text->setAlignment(Qt::AlignCenter);

        updateCheckedButtons();
        layout->addWidget(upperWidget);


        connect(pushSpawn,      SIGNAL(clicked()), this, SLOT(on_pushSpawn_clicked()));
        connect(pushCar,        SIGNAL(clicked()), this, SLOT(on_pushCar_clicked()));
        connect(pushJump,       SIGNAL(clicked()), this, SLOT(on_pushJump_clicked()));
        connect(pushLight,      SIGNAL(clicked()), this, SLOT(on_pushLight_clicked()));

        connect(pushPlayer,     SIGNAL(clicked()), this, SLOT(on_pushPlayer_clicked()));
        connect(pushPause,      SIGNAL(clicked()), this, SLOT(on_pushPause_clicked()));
        connect(pushRestart,    SIGNAL(clicked()), this, SLOT(on_pushRestart_clicked()));
        connect(pushClose,      SIGNAL(clicked()), this, SLOT(on_pushClose_clicked()));

        connect(pushPersp,      SIGNAL(clicked()), this, SLOT(on_pushPersp_clicked()));
        connect(pushOrtho,      SIGNAL(clicked()), this, SLOT(on_pushOrtho_clicked()));
        connect(push2D,         SIGNAL(clicked()), this, SLOT(on_push2D_clicked()));
        connect(push3D,         SIGNAL(clicked()), this, SLOT(on_push3D_clicked()));
        connect(pushCamera,     SIGNAL(clicked()), this, SLOT(on_pushCamera_clicked()));

        connect(pushDebug1,     SIGNAL(clicked()), this, SLOT(on_pushDebug1_clicked()));
        connect(pushDebug2,     SIGNAL(clicked()), this, SLOT(on_pushDebug2_clicked()));


        m_opengl = new DrOpenGL(centralWidget, this, m_engine);
        m_engine->setOpenGl(m_opengl);
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
        loadDemo( demo_player );
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
void FormEngine::loadDemo(Demo_Player using_player) {
    stopTimers();

    // ***** Reset keys
    g_keyboard_x =      0.0;
    g_keyboard_y =      0.0;
    g_jump_button =     false;
    g_shoot_button =    false;
    g_pedal =           Pedal::None;

    // Save Demo Types
    demo_player = using_player;

    // ***** The following are the steps to load a new Space
    m_engine->getCurrentWorld()->clearWorld();
    m_engine->getCurrentWorld()->buildWorld( m_engine->getCurrentEditorWorld(), using_player );
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
    pushPause->setText(QApplication::translate(  "MainWindow", "Pause",       nullptr));
    m_time_update =  Clock::now();
    m_time_render =  Clock::now();
    m_time_camera =  Clock::now();
    m_time_physics = Clock::now();
    m_time_frame =   Clock::now();
    m_time_fps =     Clock::now();
    m_time_input =   Clock::now();
    m_update_timer->start( 0 );                         // Timeout of zero will call this timeout every pass of the event loop
}
void FormEngine::stopTimers() {
    pushPause->setText(QApplication::translate(  "MainWindow", "Resume",       nullptr));
    m_update_timer->stop();
}
double FormEngine::getTimerMilliseconds(Engine_Timer time_since_last) {
    switch (time_since_last) {
        case Engine_Timer::Update:  return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_update).count() /  1000000.0;
        case Engine_Timer::Render:  return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_render).count() /  1000000.0;
        case Engine_Timer::Camera:  return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_camera).count() /  1000000.0;
        case Engine_Timer::Fps:     return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_fps   ).count() /  1000000.0;
        case Engine_Timer::Input:   return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_time_input ).count() /  1000000.0;
    }
}
void FormEngine::resetTimer(Engine_Timer timer_to_reset) {
    switch (timer_to_reset) {
        case Engine_Timer::Update:  m_time_update =  Clock::now();      break;
        case Engine_Timer::Render:  m_time_render =  Clock::now();      break;
        case Engine_Timer::Camera:  m_time_camera =  Clock::now();      break;
        case Engine_Timer::Fps:     m_time_fps =     Clock::now();      break;
        case Engine_Timer::Input:   m_time_input =   Clock::now();      break;
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
    if (m_engine->getCurrentWorld()->has_scene == false) return;
    m_running = true;

    // ***** Calculates Render Frames per Second
    double fps_milliseconds = getTimerMilliseconds(Engine_Timer::Fps);
    if (fps_milliseconds > 1000.0) {
        fps_render =  getOpenGL()->getFpsCount();   getOpenGL()->resetFpsCount();
        fps_physics = fps_count_physics;            fps_count_physics = 0;
        fps_camera =  fps_count_camera;             fps_count_camera =  0;
        resetTimer(Engine_Timer::Fps);

        bool cam_enabled = pushCamera->isEnabled();
        bool can_enable_cam = (m_engine->getCurrentWorld()->getCameraMap().size() > 1);
        if (cam_enabled != can_enable_cam) pushCamera->setEnabled( can_enable_cam );
    }


    // ***** Update Mouse Position
    double input_milliseconds = getTimerMilliseconds(Engine_Timer::Input);
    if (input_milliseconds > 30.0) {
        QMouseEvent *event = new QMouseEvent(QMouseEvent::MouseMove, m_opengl->mapFromGlobal(QCursor::pos()), Qt::MouseButton::NoButton, { }, { });
        m_opengl->mouseMoveEvent(event);
        resetTimer(Engine_Timer::Input);
    }


    // ***** Update Physics and Render
    double update_milliseconds = getTimerMilliseconds(Engine_Timer::Update);

    if (Dr::GetPreference(Preferences::Limit_Frames_Rendered).toBool() == false) {
        if (update_milliseconds > 4.0) {                                            // This limits max update time around 250 times per second,
            processFrame(update_milliseconds);                                      //      but accounts for system clocks with low precision
        }
    } else {
        #if defined (Q_OS_MACOS)
            if (update_milliseconds > m_engine->getCurrentWorld()->getTimeStepAsMilliseconds()) processFrame(update_milliseconds);
        #else
            if (m_wait_vsync == false) processFrame(update_milliseconds);
        #endif
    }

    m_running = false;
}

void FormEngine::processFrame(double milliseconds) {
    m_wait_vsync = true;
    resetTimer(Engine_Timer::Update);
    m_engine->getCurrentWorld()->updateSpace(milliseconds);                         // Physics Engine
    m_engine->getCurrentWorld()->updateWorld(milliseconds);                         // Additional World / Thing Updating
    fps_count_physics++;                                                            // Updates Physics Frames per Second
    m_engine->getCurrentWorld()->updateCameras();                                   // Update Camera Targets
    moveCameras();                                                                  // Move Cameras
    m_opengl->update();
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
void FormEngine::on_pushSpawn_clicked() {   loadDemo( Demo_Player::Spawn ); }
void FormEngine::on_pushCar_clicked() {     loadDemo( Demo_Player::Car ); }
void FormEngine::on_pushJump_clicked() {    loadDemo( Demo_Player::Jump ); }
void FormEngine::on_pushLight_clicked() {   loadDemo( Demo_Player::Light ); }
void FormEngine::on_pushPlayer_clicked() {  loadDemo( Demo_Player::Player ); }

void FormEngine::on_pushCamera_clicked() {
    bool only_switch_to_character_cameras = false;
    bool switch_player_controls = true;
    m_engine->getCurrentWorld()->switchCameraToNext(only_switch_to_character_cameras, switch_player_controls);
}


void FormEngine::on_pushPause_clicked() {
    if (m_engine->getCurrentWorld()->has_scene == false) return;
    if (m_update_timer->isActive()) {
        stopTimers();
    } else {
        startTimers();
    }

}
void FormEngine::on_pushRestart_clicked() {
    loadDemo(demo_player);
}
void FormEngine::on_pushClose_clicked() {
    this->close();
}

void FormEngine::on_pushPersp_clicked() { m_engine->getCurrentWorld()->render_type = Render_Type::Perspective;  updateCheckedButtons(); }
void FormEngine::on_pushOrtho_clicked() { m_engine->getCurrentWorld()->render_type = Render_Type::Orthographic; updateCheckedButtons(); }

void FormEngine::on_push2D_clicked() { m_engine->getCurrentWorld()->render_mode = Render_Mode::Mode_2D; updateCheckedButtons(); }
void FormEngine::on_push3D_clicked() { m_engine->getCurrentWorld()->render_mode = Render_Mode::Mode_3D; updateCheckedButtons(); }

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
    if (demo_player == Demo_Player::Spawn)  pushSpawn->setDown(true);   else pushSpawn->setDown(false);
    if (demo_player == Demo_Player::Car)    pushCar->setDown(true);     else pushCar->setDown(false);
    if (demo_player == Demo_Player::Jump)   pushJump->setDown(true);    else pushJump->setDown(false);
    if (demo_player == Demo_Player::Light)  pushLight->setDown(true);   else pushLight->setDown(false);
    if (demo_player == Demo_Player::Player) pushPlayer->setDown(true);  else pushPlayer->setDown(false);

    if (m_engine->getCurrentWorld()->render_type == Render_Type::Perspective)  pushPersp->setDown(true); else pushPersp->setDown(false);
    if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) pushOrtho->setDown(true); else pushOrtho->setDown(false);

    if (m_engine->getCurrentWorld()->render_mode == Render_Mode::Mode_2D) push2D->setDown(true); else push2D->setDown(false);
    if (m_engine->getCurrentWorld()->render_mode == Render_Mode::Mode_3D) push3D->setDown(true); else push3D->setDown(false);

    if (debug_shapes)     pushDebug1->setDown(true); else pushDebug1->setDown(false);
    if (debug_collisions) pushDebug2->setDown(true); else pushDebug2->setDown(false);
}
















