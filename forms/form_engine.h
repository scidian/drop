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
#include <QTime>
#include <QTimer>
#include <chrono>

#include "enums_engine.h"

enum class Engine_Timer {
    Update,
    Render,
    Camera,
};

// Forward declarations
class DrEngine;
class DrProject;
class OpenGL;

// Type definitions
typedef std::chrono::high_resolution_clock Clock;

//####################################################################################
//##    FormEngine
//##        Form that displays the Game Engine
//############################
class FormEngine : public QMainWindow
{
    Q_OBJECT

private:
    // External Borrowed Objects
    DrProject          *m_project;

    // Local Variables
    DrEngine           *m_engine;
    OpenGL             *m_opengl;

    // Timer Variables
    QTimer             *m_update_timer;

    Clock::time_point   m_time_update;                          // Checks time between SpaceStep calls
    Clock::time_point   m_time_render;                          // Checks time between paintGL calls
    Clock::time_point   m_time_camera;                          // Checks time between moveCamera calls
    Clock::time_point   m_time_physics;                         // Stores time since last physics call
    Clock::time_point   m_time_frame;                           // Stores time since last paint call

    bool                m_running = false;


public:
    const double    m_ideal_frames_per_second =  60;
    bool            m_wait_vsync = false;

    // FPS Variables
    Clock::time_point   m_time_fps = Clock::now();
    double              fps_count_render =   0.0;
    double              fps_count_camera =   0.0;
    double              fps_count_physics =  0.0;
    double              fps_render =        60.0;
    double              fps_physics =       60.0;
    double              fps_camera =        60.0;

    // Widgets
    QWidget        *centralWidget;
    QToolButton    *pushSpawn,      *pushCar,       *pushJump,      *pushPlay1, *pushPlay2;
    QToolButton    *pushLine1,      *pushLine2,     *pushBlocks,    *pushProject;
    QToolButton    *pushStart,      *pushStop,      *pushClose;
    QToolButton    *pushPersp,      *pushOrtho;
    QToolButton    *pushDebug1,     *pushDebug2;


public:
    // Constructor / Destructor
    explicit FormEngine(DrProject *project, QWidget *parent = nullptr);
    ~FormEngine() override;

    // Event Overrides
    virtual void closeEvent(QCloseEvent *event) override;
    virtual bool event(QEvent *event) override;

    // Function Calls
    DrEngine*   getEngine() { return m_engine; }
    OpenGL*     getOpenGL() { return m_opengl; }
    double      getTimerMilliseconds(Engine_Timer time_since_last);
    void        loadDemo(Demo_Space using_space, Demo_Player using_player);
    void        resetTimer(Engine_Timer timer_to_reset);
    void        startTimers();
    void        stopTimers();
    void        updateCheckedButtons();


private slots:
    void aboutToCompose();
    void frameSwapped();
    void moveCameras();
    void updateEngine();

    void on_pushSpawn_clicked();
    void on_pushCar_clicked();
    void on_pushJump_clicked();
    void on_pushPlay1_clicked();
    void on_pushPlay2_clicked();

    void on_pushLines1_clicked();
    void on_pushLines2_clicked();
    void on_pushBlocks_clicked();
    void on_pushProject_clicked();

    void on_pushStart_clicked();
    void on_pushStop_clicked();
    void on_pushClose_clicked();

    void on_pushPersp_clicked();
    void on_pushOrtho_clicked();

    void on_pushDebug1_clicked();
    void on_pushDebug2_clicked();
};




#endif // FORMENGINE_H
















