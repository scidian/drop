//
//
//      Handles dealing with the Scene View
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"
#include "05_item.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include "form_main.h"



// Mouse entered widget area event
void SceneGraphicsView::enterEvent(QEvent *event)
{
    setFocus(Qt::FocusReason::MouseFocusReason);                        // Set focus on mouse enter to allow for space bar pressing hand grab
    getMainWindow()->setAdvisorInfo(Advisor_Info::Scene_Area);          // Set Advisor text on mouse enter
    QGraphicsView::enterEvent(event);
    update();
}
void SceneGraphicsView::keyPressEvent(QKeyEvent *event)
{
    // When space bar is down, enabled mouse press and move to translate viewable area
    if (event->key() == Qt::Key::Key_Space) {
        setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
        setInteractive(false);
    }
    QGraphicsView::keyPressEvent(event);
    update();
}
void SceneGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    // When space bar is released, change mode back to select / move items
    if (event->key() == Qt::Key::Key_Space) {
        setDragMode(QGraphicsView::DragMode::RubberBandDrag);
        setInteractive(true);
    }
    QGraphicsView::keyReleaseEvent(event);
    update();
}
void SceneGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    update();
}



#if QT_CONFIG(wheelevent)
void SceneGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QGraphicsView::wheelEvent(event);
    } else {
        if (event->delta() > 0)
            zoomInOut(8);
        else
            zoomInOut(-8);
        event->accept();
    }
    update();
}
#endif


void SceneGraphicsView::zoomInOut(int level)
{
    m_zoom += level;
    applyUpdatedMatrix();
}

void SceneGraphicsView::applyUpdatedMatrix()
{
    qreal scale = qPow(qreal(2), (m_zoom - 250) / qreal(50));

    QMatrix matrix;
    matrix.scale(scale, scale);
    matrix.rotate(m_rotate);
    this->setMatrix(matrix);
}






















