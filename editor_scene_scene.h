//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QGraphicsScene so we can override events for our Scene
//
//
#ifndef EDITOR_SCENE_SCENE_H
#define EDITOR_SCENE_SCENE_H

#include <QtWidgets>

#include <05_item.h>

class DrProject;
class InterfaceRelay;


class SceneGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

private:
    DrProject      *m_project;                  // Pointer to currently loaded project
    InterfaceRelay *m_interface;                // Pointer to interface class of parent form

public:
    // Constructor
    explicit SceneGraphicsScene(QWidget *parent, DrProject *project, InterfaceRelay *interface) :
                                QGraphicsScene(parent = nullptr), m_project(project), m_interface(interface) {
        connect(this, SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));
    }

    // Event Overrides, start at Qt Docs for QGraphicsScene Class to find more
    virtual void keyPressEvent(QKeyEvent *event) override;                              // Inherited from QGraphicsScene
    virtual void keyReleaseEvent(QKeyEvent *event) override;                            // Inherited from QGraphicsScene

    // Functions
    void addSquare(qreal new_x, qreal new_y, double new_width, double new_height, double z_order,
                   QColor color = QColor::fromRgb(QRandomGenerator::global()->generate()).light(100));

public slots:
    void    sceneChanged(QList<QRectF> region);


};

#endif // EDITOR_SCENE_SCENE_H
