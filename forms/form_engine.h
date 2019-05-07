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
#include <QTimer>

class Engine;
class OpenGL;

//####################################################################################
//##    FormEngine
//##        Form that displays the Game Engine
//############################
class FormEngine : public QMainWindow
{
    Q_OBJECT

private:
    QTimer     *m_timer;
    Engine     *m_engine;
    OpenGL     *m_opengl;

public:
    QWidget         *centralWidget;
    QPushButton     *pushButton,    *pushButton2,   *pushDebug;
    QPushButton     *pushSpawn,     *pushCar;
    QPushButton     *pushPersp,     *pushOrtho;
    QLabel          *label,         *label2;
    QLabel          *labelOpenGL;


public:
    explicit FormEngine(QWidget *parent = nullptr);
    ~FormEngine() override;

    // Event Overrides
    virtual void closeEvent(QCloseEvent *event) override;

private slots:
    void on_pushButton_clicked();
    void on_pushButton2_clicked();
    void on_pushDebug_clicked();
    void on_pushSpawn_clicked();
    void on_pushCar_clicked();
    void on_pushPersp_clicked();
    void on_pushOrtho_clicked();

    void updateEngine();
    void updateLabels(QString info);
};




#endif // FORMENGINE_H
















