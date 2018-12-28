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


void FormMain::populateScene()
{
    scene = new QGraphicsScene(this);

    QImage image(":/qt4logo.png");

    // Populate scene
    int xx = 0;
    int nitems = 0;
    for (int i = -11000; i < 11000; i += 110) {
        ++xx;
        int yy = 0;
        for (int j = -7000; j < 7000; j += 70) {
            ++yy;
            qreal x = (i + 11000) / 22000.0;
            qreal y = (j + 7000) / 14000.0;

            QColor color(image.pixel(int(image.width() * x), int(image.height() * y)));
            QGraphicsItem *item = new DrItem(color, xx, yy);
            item->setPos(QPointF(i, j));
            scene->addItem(item);

            ++nitems;
        }
    }
}


#if QT_CONFIG(wheelevent)
void SceneGraphicsView::wheelEvent(QWheelEvent *e)
{
    //if (e->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        if (e->delta() > 0)
            zoomInOut(8);
        else
            zoomInOut(-8);
        e->accept();
    //} else {
    //    QGraphicsView::wheelEvent(e);
    //}
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






















