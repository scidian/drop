//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>

#include "colors/colors.h"
#include "enums.h"
#include "form_playground.h"
#include "helper.h"
#include "playground.h"
#include "playground_toy.h"

//####################################################################################
//##    Updates GraphicsItems to match physics scene
//####################################################################################
void FormPlayground::updateGraphicsView() {
    for (auto toy : m_playground->objects) {
        if (qFuzzyCompare(toy->graphic->pos().x(),  toy->m_position.x()) == false ||
            qFuzzyCompare(toy->graphic->pos().y(), -toy->m_position.y()) == false) {
            toy->graphic->setPos( QPointF(toy->m_position.x(), -toy->m_position.y()) );
        }
        if (qFuzzyCompare(toy->graphic->rotation(), -toy->m_angle) == false ) {
            toy->graphic->setRotation( -toy->m_angle );
        }
    }
}


//####################################################################################
//##    Mouse Handling
//####################################################################################
void DrPlaygroundView::mousePressEvent(QMouseEvent *event) {
    QRect   box { -990, -990, 1980, 1980 };
    QPointF pos = mapToScene( event->pos() );
    QGraphicsItem *item = itemAt(event->pos());

    // ********** Left Mouse Button
    if (event->button() == Qt::MouseButton::LeftButton) {

        // Deselect all items, if was selected, repaint
        for (auto item: scene()->items()) {
            bool was_true = item->data(User_Roles::Selected).toBool();
            item->setData(User_Roles::Selected, false);
            if (was_true) item->update();
        }

        // Find item at mouse position in Space
        cpVect cp_pos = cpv(mapToScene(event->pos()).x(), -mapToScene(event->pos()).y());
        DrToy *toy = m_playground->findToyAtPoint(cp_pos, 5);
        if (toy) {
            toy->graphic->setData(User_Roles::Selected, true);
            toy->graphic->update();
            m_form_playground->clickedItem(toy);
            if (toy->body_type != Body_Type::Static) {
                mouse_joint = cpPivotJointNew(m_playground->mouse_body, toy->body, cpBodyGetPosition(toy->body));
                cpSpaceAddConstraint( m_playground->getSpace(), mouse_joint );
            }

        // It's possible findToyAtPoint may not work while Space is inactive, in that case select item at mouse position
        } else if (item != nullptr) {
            toy = item->data(User_Roles::Toy).value<DrToy*>();
            toy->graphic->setData(User_Roles::Selected, true);
            m_form_playground->clickedItem(toy);

        // Otherwwise no item is selected
        } else {
            m_form_playground->clickedItem(nullptr);
        }

    // ********** Right Mouse Button
    } else if (event->button() == Qt::MouseButton::RightButton) {
        // If clicked within gray lines, add toy to playground
        if (box.contains(pos.toPoint())) {
            DrToy *toy = m_playground->addCircle(Body_Type::Dynamic, Dr::GetColor(Window_Colors::Icon_Light), pos.x(), -pos.y(), 0, 50,
                                                 c_friction, c_bounce, true, true);
            // If timer isnt running, allow QGraphicItem movement on new item
            if (m_playground->getFormPlayground()->isTimerActive() == false) {
                toy->graphic->setFlag(QGraphicsItem::ItemIsMovable, true);
                toy->graphic->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
                toy->graphic->setFlag(QGraphicsItem::ItemIsSelectable, false);
                toy->graphic->setFlag(QGraphicsItem::ItemIsFocusable, false);
            }
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void DrPlaygroundView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MouseButton::LeftButton) {
        // On mouse up, if timer is running and there was an object linked to the mouse_body, destroy the mouse_joint
        if (mouse_joint) {
            cpSpaceRemoveConstraint(m_playground->getSpace(), mouse_joint);
            cpConstraintFree(mouse_joint);
            mouse_joint = nullptr;
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void DrPlaygroundView::mouseMoveEvent(QMouseEvent *event) {
    if (m_playground->getFormPlayground()->isTimerActive()) {
        cpVect pos = cpv(mapToScene(event->pos()).x(), -mapToScene(event->pos()).y());
        cpBodySetPosition(m_playground->mouse_body, pos);
    }
    QGraphicsView::mouseMoveEvent(event);
}


//####################################################################################
//##    Wheel Event / Zoom Functions
//####################################################################################
// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void DrPlaygroundView::wheelEvent(QWheelEvent *event) {
    // Allow for scene scrolling if ctrl (cmd) is down
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    if (event->delta() > 0)
        zoomInOut(10);
    else
        zoomInOut(-10);
    event->accept();
}
#endif

void DrPlaygroundView::zoomInOut(int level) {
    m_zoom += level;
    if (m_zoom > 500) m_zoom = 500;
    if (m_zoom < -40) m_zoom = -40;
    m_zoom_scale = qPow(qreal(2), (m_zoom - 250) / qreal(50));

    QMatrix matrix;
    matrix.scale(m_zoom_scale, m_zoom_scale);
    this->setMatrix(matrix);
}










