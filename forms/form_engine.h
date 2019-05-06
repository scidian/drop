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
#include <QGraphicsPixmapItem>
#include <QTime>
#include <QTimer>

#include <stdio.h>
#include "chipmunk/chipmunk.h"
#include "physics/physics_sprite.h"

enum class RenderType {
    Perspective,
    Orthographic,
};

enum class Demo {
    Spawn,
    Car,
};

enum class Pedal {
    Brake,
    None,
    Clockwise,
    CounterClockwise,
};

class PhysicsOpenGL;
class PhysicsScene;
class PhysicsView;


constexpr int c_texture_border = 5;


class FormEngine : public QMainWindow
{
    Q_OBJECT

private:
    QTimer     *timer;
    cpSpace    *space;                      // A physics space (scene / world / whatever)
    cpFloat     timeStep = 1 / 60.0;        // It is *highly* recommended to use a fixed size time step.

    cpVect      gravity;                    // Global gravity applied to the space. Defaults to cpvzero. Can be overridden on a per body basis
                                            //  by writing custom integration functions. Changing the gravity will activate all sleeping bodies in the space.
    cpFloat     damping;                    // A value of 0.9 means that each body will lose 10% of its velocity per second. Defaults to 1.
                                            // Like gravity, it can be overridden on a per body basis.


public:
    QWidget         *centralWidget;
    QPushButton     *pushButton,    *pushButton2,   *pushDebug;
    QPushButton     *pushSpawn,     *pushCar;
    QPushButton     *pushPersp,     *pushOrtho;
    QLabel          *label,         *label2;
  //  QLabel          *labelOpenGL;

    PhysicsOpenGL   *opengl;

    QVector<SceneObject*>   objects;

    Demo        demo = Demo::Spawn;
    QTime       fps_timer;
    int         fps;
    bool        debug = false;

    RenderType  render_type = RenderType::Orthographic;          // Should render perspective or orthographic?
    QPointF     camera_pos = QPointF(0, 0);                     // Tells renderer where to center view

    bool        has_scene = false;
    Pedal       gas_pedal = Pedal::None;


    // Textures
    QOpenGLTexture     *t_ball;
    QOpenGLTexture     *t_metal_block;
    QOpenGLTexture     *t_moon_plant;
    QOpenGLTexture     *t_rover_body;
    QOpenGLTexture     *t_rover_wheel;
    QOpenGLTexture     *t_spare_wheel;


public:
    explicit FormEngine(QWidget *parent = nullptr);
    ~FormEngine() override;

    // Event Overrides
    virtual void closeEvent(QCloseEvent *event) override;

    //  Friction: 0 = frictionless, unknown limit
    //  Bounce:   0 = no bounce, 1.0 will give a “perfect” bounce. However due to inaccuracies in the simulation using 1.0 or greater is not recommended
    //  Mass:     Not sure
    SceneObject*    addLine(BodyType body_type, QPointF p1, QPointF p2, double friction, double bounce, double mass);
    SceneObject*    addCircle(BodyType body_type, QOpenGLTexture *txt, double x, double y, double friction, double bounce, double mass, QPointF velocity);
    SceneObject*    addBlock(BodyType body_type, QOpenGLTexture *txt, double x, double y, double friction, double bounce, double mass, QPointF velocity);
    SceneObject*    addPolygon(BodyType body_type, QOpenGLTexture *txt, double x, double y, QVector<QPointF> points, double friction, double bounce, double mass, QPointF velocity);

    bool            debugOn() { return debug; }
    void            showMessageBox(QString new_message, QPixmap pixmap = QPixmap());

private slots:
    void on_pushButton_clicked();
    void on_pushButton2_clicked();
    void on_pushDebug_clicked();
    void on_pushSpawn_clicked();
    void on_pushCar_clicked();
    void on_pushPersp_clicked();
    void on_pushOrtho_clicked();

    void buildScene();
    void clearScene();
    void updateScene();

};




#endif // FORMENGINE_H
















