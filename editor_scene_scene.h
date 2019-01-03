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
                                QGraphicsScene(parent = nullptr), m_project(project), m_interface(interface) { }

    // Event Overrides, start at Qt Docs for QGraphicsScene Class to find more
    virtual void keyPressEvent(QKeyEvent *event) override;                              // Inherited from QGraphicsScene
    virtual void keyReleaseEvent(QKeyEvent *event) override;                            // Inherited from QGraphicsScene


    // Function calls
    void addSquare(qreal new_x, qreal new_y, QColor color = QColor::fromRgb(QRandomGenerator::global()->generate()).light(100));

};

#endif // EDITOR_SCENE_SCENE_H
