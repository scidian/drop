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
#include "physics/physics_opengl.h"


//######################################################################################################
//##    Constructor / Destructor
//######################################################################################################
FormEngine::FormEngine(QWidget *parent) : QMainWindow(parent) {
    this->setObjectName("Drop Player");
    this->resize(1200, 800);

    centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        QWidget *upperWidget = new QWidget();
        upperWidget->setFixedHeight(90);
        pushButton =  new QPushButton(upperWidget);     pushButton->setObjectName("pushButton");    pushButton->setGeometry(QRect(10, 10, 141, 31));
        pushButton2 = new QPushButton(upperWidget);     pushButton2->setObjectName("pushButton2");  pushButton2->setGeometry(QRect(10, 50, 141, 31));
        pushDebug =   new QPushButton(upperWidget);     pushDebug->setObjectName("pushDebug");      pushDebug->setGeometry(QRect(830, 10, 141, 71));
        pushSpawn =   new QPushButton(upperWidget);     pushButton->setObjectName("pushSpawn");     pushSpawn->setGeometry(QRect(700, 10, 141, 31));
        pushCar =     new QPushButton(upperWidget);     pushButton2->setObjectName("pushCar");      pushCar->setGeometry(QRect(700, 50, 141, 31));
        label =       new QLabel(upperWidget);          label->setObjectName("label");              label->setGeometry( QRect(190, 10, 491, 31));
        label2 =      new QLabel(upperWidget);          label2->setObjectName("label2");            label2->setGeometry(QRect(190, 40, 491, 31));
        pushButton->setText(QApplication::translate("MainWindow", "Start Scene", nullptr));
        pushButton2->setText(QApplication::translate("MainWindow", "Stop Scene", nullptr));
        pushDebug->setText(QApplication::translate("MainWindow", "Toggle Debug", nullptr));
        pushSpawn->setText(QApplication::translate("MainWindow", "Spawning Demo", nullptr));
        pushCar->setText(QApplication::translate("MainWindow", "Car Demo", nullptr));
        layout->addWidget(upperWidget);

        connect(pushButton,  SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
        connect(pushButton2, SIGNAL(clicked()), this, SLOT(on_pushButton2_clicked()));
        connect(pushDebug,   SIGNAL(clicked()), this, SLOT(on_pushDebug_clicked()));
        connect(pushSpawn,   SIGNAL(clicked()), this, SLOT(on_pushSpawn_clicked()));
        connect(pushCar,     SIGNAL(clicked()), this, SLOT(on_pushCar_clicked()));

        opengl = new PhysicsOpenGL(centralWidget, this);
        layout->addWidget(opengl);

    this->setCentralWidget(centralWidget);


    // Create scene timer and connect it to UpdateScene
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateScene()));

}

FormEngine::~FormEngine() { }

void FormEngine::closeEvent(QCloseEvent *) {
    timer->stop();
    qApp->processEvents();
}

static void ShapeFreeWrap(cpSpace *space, cpShape *shape, void *) { cpSpaceRemoveShape(space, shape);   cpShapeFree(shape); }
static void PostShapeFree(cpShape *shape, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(ShapeFreeWrap), shape, nullptr); }
static void ConstraintFreeWrap(cpSpace *space, cpConstraint *constraint, void *) { cpSpaceRemoveConstraint(space, constraint); cpConstraintFree(constraint); }
static void PostConstraintFree(cpConstraint *constraint, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(ConstraintFreeWrap), constraint, nullptr); }
static void BodyFreeWrap(cpSpace *space, cpBody *body, void *) { cpSpaceRemoveBody(space, body); cpBodyFree(body); }
static void PostBodyFree(cpBody *body, cpSpace *space) { cpSpaceAddPostStepCallback(space, cpPostStepFunc(BodyFreeWrap), body, nullptr); }

// Safe and future proof way to remove and free all objects that have been added to the space.
void ChipmunkFreeSpaceChildren(cpSpace *space) {
    // Must remove these BEFORE freeing the body or you will access dangling pointers.
    cpSpaceEachShape(space, cpSpaceShapeIteratorFunc(PostShapeFree), space);
    cpSpaceEachConstraint(space, cpSpaceConstraintIteratorFunc(PostConstraintFree), space);
    cpSpaceEachBody(space, cpSpaceBodyIteratorFunc(PostBodyFree), space);
}

void FormEngine::clearScene() {
    if (has_scene) {
        ChipmunkFreeSpaceChildren(space);
        cpSpaceFree(space);
        objects.clear();
        has_scene = false;
    }
}


//######################################################################################################
//##    Button Handling
//######################################################################################################
void FormEngine::on_pushButton_clicked() {
    if (!has_scene) return;
    timer->start( int(1000 * timeStep) );
    fps_timer.restart();
    fps = 0;
}
void FormEngine::on_pushButton2_clicked() {   timer->stop();  }

void FormEngine::on_pushDebug_clicked() {
    debug = !debug;
    opengl->update();
}

void FormEngine::on_pushSpawn_clicked() { timer->stop();  clearScene();   demo = Demo::Spawn;     buildScene();   }
void FormEngine::on_pushCar_clicked() {   timer->stop();  clearScene();   demo = Demo::Car;       buildScene();   }

//######################################################################################################
//##    Build Scene
//######################################################################################################
void FormEngine::buildScene() {

    if (demo == Demo::Spawn) {
        // Set up physics world
        gravity = cpv(0, -500);                 // cpVect is a 2D vector and cpv() is a shortcut for initializing them
        space = cpSpaceNew();                   // Creates an empty space
        cpSpaceSetIterations(space, 10);        // 10 is default and should be good enough for most games
        cpSpaceSetGravity(space, gravity);

        // Add a static line segment shapes for the ground.
        this->addLine(BodyType::Static, QPointF(-800, -0),   QPointF( 300,  250), 1, .99, 1);
        this->addLine(BodyType::Static, QPointF( 250, -50),  QPointF(1000, -200), 1, .99, 1);
        this->addLine(BodyType::Static, QPointF(-1100, 300), QPointF(-900,  300), 1, .99, 1);

        // Add one ball
        this->addCircle(BodyType::Dynamic,   t_ball, -100, -100, .7, .5, 2, QPointF( 0, 0));
        this->addCircle(BodyType::Kinematic, t_ball, -300, -150, .7, .5, 2, QPointF(25, 0));


    } else if (demo == Demo::Car) {
        // Set up physics world
        gravity = cpv(0, -500);                 // cpVect is a 2D vector and cpv() is a shortcut for initializing them
        space = cpSpaceNew();                   // Creates an empty space
        damping = .7;                           // Kind of like air drag
        cpSpaceSetIterations(space, 10);        // 10 is default and should be good enough for most games
        cpSpaceSetGravity(space, gravity);
        cpSpaceSetDamping(space, damping);


        // Add a static line segment shapes for the ground.
        this->addLine(BodyType::Static, QPointF(-1000,    0), QPointF( 2500,    0), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF(-1000,    0), QPointF(-1000, -100), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF( 2500,    0), QPointF( 2500, -100), 1, .75, 1);

        // Big ramp
        this->addLine(BodyType::Static, QPointF(    0,    0), QPointF(300,  -50), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF(  300,  -50), QPointF(600,    0), 1, .75, 1);

        // Little bumps
        this->addLine(BodyType::Static, QPointF( 1090,    0), QPointF(1120,  -4), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF( 1120,   -4), QPointF(1150,   0), 1, .75, 1);

        this->addLine(BodyType::Static, QPointF( 1170,    0), QPointF(1200,  -4), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF( 1200,   -4), QPointF(1230,   0), 1, .75, 1);

        this->addLine(BodyType::Static, QPointF( 1250,    0), QPointF(1280,  -4), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF( 1280,   -4), QPointF(1310,   0), 1, .75, 1);

        this->addLine(BodyType::Static, QPointF( 1330,    0), QPointF(1360,  -4), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF( 1360,   -4), QPointF(1390,   0), 1, .75, 1);

        this->addLine(BodyType::Static, QPointF( 1410,    0), QPointF(1440,  -4), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF( 1440,   -4), QPointF(1470,   0), 1, .75, 1);

        this->addLine(BodyType::Static, QPointF( 1490,    0), QPointF(1520,  -4), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF( 1520,   -4), QPointF(1550,   0), 1, .75, 1);

        this->addLine(BodyType::Static, QPointF( 1570,    0), QPointF(1600,  -4), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF( 1600,   -4), QPointF(1630,   0), 1, .75, 1);

        this->addLine(BodyType::Static, QPointF( 1650,    0), QPointF(1680,  -4), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF( 1680,   -4), QPointF(1710,   0), 1, .75, 1);

        this->addLine(BodyType::Static, QPointF( 1730,    0), QPointF(1760,  -4), 1, .75, 1);
        this->addLine(BodyType::Static, QPointF( 1760,   -4), QPointF(1790,   0), 1, .75, 1);

        // Sample object to find category and mask numbers
        SceneObject  *base = this->addBlock(BodyType::Kinematic, t_metal_block,  300,   0, 1, .75, 100, QPointF(0, 0));
        cpShapeFilter test = cpShapeGetFilter( base->shape);

        // Add body
        SceneObject *rover = this->addBlock(  BodyType::Dynamic, t_rover_body, -450, -75,   .5, .1, 4, QPointF(0, 0));

        // Add wheels
        SceneObject *wheel1 = this->addCircle(BodyType::Dynamic, t_rover_wheel, -490, -45, 3.4, .7,  2, QPointF(0, 0));
        SceneObject *wheel2 = this->addCircle(BodyType::Dynamic, t_rover_wheel, -450, -45, 3.4, .7,  2, QPointF(0, 0));
        SceneObject *wheel3 = this->addCircle(BodyType::Dynamic, t_rover_wheel, -410, -45, 3.4, .7,  2, QPointF(0, 0));
        wheel1->is_wheel = true;    wheel1->wheel_speed = 80;
        wheel2->is_wheel = true;    wheel2->wheel_speed = 40;
        wheel3->is_wheel = true;    wheel3->wheel_speed = 60;
        SceneObject *spare1 = this->addCircle(BodyType::Dynamic, t_spare_wheel, -510, -35, 3.4, .7, .5, QPointF(0, 0));

        //this->addCircle(BodyType::Dynamic, t_ball, -450, -80, 4, 0, 2, QPointF(0, 0));

        // Set body and wheels to same group so they don't collide
        //EX:
        //#define GRABBABLE_MASK_BIT (1<<31)
        //cpShapeFilter GRAB_FILTER = {CP_NO_GROUP, GRABBABLE_MASK_BIT, GRABBABLE_MASK_BIT};
        //cpShapeFilter NOT_GRABBABLE_FILTER = {CP_NO_GROUP, ~GRABBABLE_MASK_BIT, ~GRABBABLE_MASK_BIT};
        cpShapeFilter filter;
        filter.group = 43;
        filter.mask = test.mask;
        filter.categories = test.categories;
        cpShapeSetFilter( rover->shape,  filter);
        cpShapeSetFilter( wheel1->shape, filter);
        cpShapeSetFilter( wheel2->shape, filter);
        cpShapeSetFilter( wheel3->shape, filter);

        // Old solid pin joint
        //cpSpaceAddConstraint( space, cpPivotJointNew(rover->body, wheel1->body, cpBodyGetPosition(wheel1->body));
        //cpSpaceAddConstraint( space, cpPivotJointNew(rover->body, wheel2->body, cpBodyGetPosition(wheel2->body));

        // New bouncy shocks joint, Grooves a/b are relative to the car, anchor point B is on the wheel
        cpSpaceAddConstraint(space, cpGrooveJointNew( rover->body, wheel1->body, cpv(-40, -15), cpv(-40, -28), cpvzero));
        cpSpaceAddConstraint(space, cpGrooveJointNew( rover->body, wheel2->body, cpv(  0, -15), cpv(  0, -28), cpvzero));
        cpSpaceAddConstraint(space, cpGrooveJointNew( rover->body, wheel3->body, cpv( 40, -15), cpv( 40, -28), cpvzero));

        cpSpaceAddConstraint(space, cpGrooveJointNew( rover->body, spare1->body, cpv(-40, -15), cpv(-65, -30), cpvzero));
        //cpSpaceAddConstraint(space, cpGrooveJointNew( rover->body, spare2->body, cpv( 40, -15), cpv( 65, -30), cpvzero));

        cpSpaceAddConstraint(space, cpDampedSpringNew(rover->body, wheel1->body, cpv(-40, 0), cpvzero, 50.0, 90.0, 50.0)); // Higher damp = less mushy, 100 = pretty stiff
        cpSpaceAddConstraint(space, cpDampedSpringNew(rover->body, wheel2->body, cpv(  0, 0), cpvzero, 50.0, 90.0, 25.0));
        cpSpaceAddConstraint(space, cpDampedSpringNew(rover->body, wheel3->body, cpv( 40, 0), cpvzero, 50.0, 90.0, 40.0));

        cpSpaceAddConstraint(space, cpDampedSpringNew(rover->body, spare1->body, cpv(-40, 0), cpvzero, 40.0, 30.0,  5.0));
        //cpSpaceAddConstraint(space, cpDampedSpringNew(rover->body, spare2->body, cpv( 40, 0), cpvzero, 40.0, 30.0,  5.0));

        // Applies constant speed to joint
        //cpConstraint *wheel_motor_1 = cpSimpleMotorNew(rover->body, wheel1->body, 25);
        //cpSpaceAddConstraint( space, wheel_motor_1);





        //cpConstraint *wheel_motor_2 = cpSimpleMotorNew(rover->body, wheel2->body, .2);
        //cpSpaceAddConstraint( space, wheel_motor_2);
    }

    has_scene = true;
}

//######################################################################################################
//##    UpdateScene
//######################################################################################################
void FormEngine::updateScene() {

    cpSpaceStep(space, timeStep);

    // Iterate through objects, delete if they go off screen
    for ( auto it = objects.begin(); it != objects.end(); ) {
        SceneObject *object = *it;

        if ((object->body_type == BodyType::Static) || (object->in_scene == false)) {
            it++;
            continue;
        }

        // Gets velocity
        cpVect  vel = cpBodyGetVelocity( object->body );
        object->velocity.setX( vel.x );
        object->velocity.setX( vel.y );

        // Gets position
        cpVect  pos = cpBodyGetPosition( object->body );
        object->position.setX( pos.x );
        object->position.setY( pos.y );

        // Gets angle
        cpFloat angle = cpBodyGetAngle( object->body );
        object->angle = qRadiansToDegrees( angle );

        // If is a wheel, apply gas pedal
        if (object->is_wheel) {
            switch (gas_pedal) {
                case Pedal::Clockwise:          cpBodySetAngularVelocity( object->body, -object->wheel_speed );     break;
                case Pedal::CounterClockwise:   cpBodySetAngularVelocity( object->body,  object->wheel_speed );     break;
                case Pedal::Brake:              cpBodySetAngularVelocity( object->body,  0 );                       break;
                case Pedal::None:               break;
            }
        }

        // Delete object if it falls below y = 400
        if (pos.y < -400) {
            object->in_scene = false;

            cpSpaceRemoveShape( space, object->shape );     cpSpaceRemoveBody(  space, object->body  );
            cpShapeFree( object->shape );                   cpBodyFree(  object->body  );

            it = objects.erase(it);
        } else {
            it++;
        }
    }

    // Show total items in objects vector
    label->setText("Total Items: " + QString::number( objects.count()) );
    opengl->update();
}




//######################################################################################################
//##    Conveinient Message Box Popup
//######################################################################################################
void FormEngine::showMessageBox(QString new_message, QPixmap pixmap)
{
    QMessageBox *msg_box = new QMessageBox(nullptr);
    msg_box->setText(new_message);
    msg_box->setIconPixmap(pixmap);
    msg_box->exec();
    delete msg_box;
}













