//
//      Created by Stephens Nunnally on 5/1/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORMENGINE_H
#define FORMENGINE_H

#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QTimer>
#include <chrono>

#include "globals_engine.h"

enum class Engine_Timer {
    Update,
    Render,
    Camera,
    Fps,
    Input,
};


//####################################################################################
//##    FormEngine
//##        Form that displays the Game Engine
//############################
class FormEngine : public QMainWindow
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject          *m_project;

    // Local Variables
    DrEngine           *m_engine;
    DrOpenGL           *m_opengl;

    // Timer Variables
    QTimer             *m_update_timer;
    Clock::time_point   m_time_update;                          // Checks time between SpaceStep calls
    Clock::time_point   m_time_render;                          // Checks time between paintGL calls
    Clock::time_point   m_time_camera;                          // Checks time between moveCamera calls
    Clock::time_point   m_time_input;                           // Checks time between Input Update calls
    Clock::time_point   m_time_physics;                         // Stores time since last physics call
    Clock::time_point   m_time_frame;                           // Stores time since last paint call
    bool                m_running = false;

public:
    // Widgets
    QWidget        *centralWidget;
    QToolButton    *pushSpawn,      *pushCar,       *pushJump,      *pushLight,     *pushPlayer;
    QToolButton    *pushPause,      *pushRestart,   *pushClose,     *pushCamera;
    QToolButton    *pushPersp,      *pushOrtho,     *push2D,        *push3D;
    QToolButton    *pushDebug1,     *pushDebug2;

    const double        m_ideal_frames_per_second = 60;
    bool                m_wait_vsync = false;

    // FPS Variables
    Clock::time_point   m_time_fps = Clock::now();
    double              fps_count_camera =   0.0;
    double              fps_count_physics =  0.0;
    double              fps_render =        60.0;
    double              fps_physics =       60.0;
    double              fps_camera =        60.0;

    // Debug Variables
    bool                debug_shapes =      false;
    bool                debug_collisions =  false;

    // Demo Variables
    Demo_Player         demo_player = Demo_Player::Player;

public:
    // Constructor / Destructor
    explicit FormEngine(DrProject *project, long stage_key = c_no_key, QWidget *parent = nullptr);
    ~FormEngine() override;

    // Event Overrides
    virtual void closeEvent(QCloseEvent *event) override;
    virtual bool event(QEvent *event) override;

    // Function Calls
    DrEngine*   getEngine() { return m_engine; }
    DrOpenGL*   getOpenGL() { return m_opengl; }
    double      getTimerMilliseconds(Engine_Timer time_since_last);
    bool        isTimerActive() { return m_update_timer->isActive(); }
    void        loadDemo(Demo_Player using_player);
    void        resetTimer(Engine_Timer timer_to_reset);
    void        startTimers();
    void        stopTimers();
    void        updateCheckedButtons();


private slots:
    void aboutToCompose();
    void frameSwapped();
    void moveCameras();
    void processFrame(double milliseconds);
    void updateEngine();

    void on_pushSpawn_clicked();
    void on_pushCar_clicked();
    void on_pushJump_clicked();
    void on_pushLight_clicked();

    void on_pushPlayer_clicked();
    void on_pushPause_clicked();
    void on_pushRestart_clicked();
    void on_pushClose_clicked();

    void on_pushPersp_clicked();
    void on_pushOrtho_clicked();
    void on_push2D_clicked();
    void on_push3D_clicked();
    void on_pushCamera_clicked();

    void on_pushDebug1_clicked();
    void on_pushDebug2_clicked();
};




#endif // FORMENGINE_H
















