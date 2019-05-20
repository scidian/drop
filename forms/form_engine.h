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

#include "engine/engine.h"

// Forward declarations
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
    DrEngine           *m_engine;
    OpenGL             *m_opengl;
    DrProject          *m_project;

    QTimer             *m_timer;
    Clock::time_point   m_time_update;
    Clock::time_point   m_time_camera;
    Clock::time_point   m_time_render;

    const double        m_ideal_frames_per_second = 60;

public:
    QWidget         *centralWidget;
    QToolButton     *pushSpawn,     *pushCar,       *pushJump,      *pushPlay1, *pushPlay2;
    QToolButton     *pushLine1,     *pushLine2,     *pushBlocks,    *pushProject;
    QToolButton     *pushStart,     *pushStop;
    QToolButton     *pushPersp,     *pushOrtho;
    QToolButton     *pushDebug1,    *pushDebug2;

    QLabel          *label,         *label2;
    QLabel          *labelOpenGL,   *labelInfo;


public:
    // Constructor / Destructor
    explicit FormEngine(DrProject *project, QWidget *parent = nullptr);
    ~FormEngine() override;

    // Event Overrides
    virtual void closeEvent(QCloseEvent *event) override;

    // Function Calls
    void loadDemo(Demo_Space using_space, Demo_Player using_player);
    void startTimers();
    void stopTimers();
    void updateCheckedButtons();


private slots:
    void updateEngine();
    void updateLabels(QString info);

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

    void on_pushPersp_clicked();
    void on_pushOrtho_clicked();

    void on_pushDebug1_clicked();
    void on_pushDebug2_clicked();

};




#endif // FORMENGINE_H
















