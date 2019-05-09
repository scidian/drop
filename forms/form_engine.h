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
#include <QTime>
#include <QTimer>

class DrEngine;
class DrProject;
class OpenGL;

//####################################################################################
//##    FormEngine
//##        Form that displays the Game Engine
//############################
class FormEngine : public QMainWindow
{
    Q_OBJECT

private:
    DrEngine   *m_engine;
    OpenGL     *m_opengl;
    DrProject  *m_project;

    QTimer     *m_timer;
    QTime       m_time_last_update;
    QTime       m_time_last_render;

    const qreal m_ideal_frames_per_second = 60;

public:
    QWidget         *centralWidget;
    QPushButton     *pushSpawn,     *pushCar,       *pushJump,      *pushProject;
    QPushButton     *pushStart,     *pushStop;
    QPushButton     *pushPersp,     *pushOrtho;
    QPushButton     *pushDebug;

    QLabel          *label,         *label2;
    QLabel          *labelOpenGL;


public:
    explicit FormEngine(DrProject *project, QWidget *parent = nullptr);
    ~FormEngine() override;

    // Event Overrides
    virtual void closeEvent(QCloseEvent *event) override;

private slots:
    void on_pushButton_clicked();
    void on_pushButton2_clicked();
    void on_pushDebug_clicked();
    void on_pushSpawn_clicked();
    void on_pushCar_clicked();
    void on_pushJump_clicked();
    void on_pushProject_clicked();
    void on_pushPersp_clicked();
    void on_pushOrtho_clicked();

    void startTimers();
    void stopTimers();
    void updateEngine();
    void updateLabels(QString info);
};




#endif // FORMENGINE_H
















