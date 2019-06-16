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
#include "form_playground.h"
#include "playground.h"
#include "playground_toy.h"

//####################################################################################
//##        Updates GraphicsItems to match physics scene
//####################################################################################
void FormPlayground::updateGraphicsView() {
    for (auto toy : m_playground->objects) {
        toy->graphic->setPos( QPointF(toy->m_position.x(), -toy->m_position.y()) );
        toy->graphic->setRotation( -toy->m_angle );
    }
}


//####################################################################################
//##        Custom Item Painting
//####################################################################################
void DrPlaygroundCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QGraphicsEllipseItem::paint(painter, option, widget);
    painter->setPen( this->pen() );
    painter->drawLine( 0, 0, 0, -static_cast<int>(this->rect().height() / 2));
}

void DrPlaygroundBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QGraphicsRectItem::paint(painter, option, widget);
    painter->setPen( this->pen() );
    painter->drawLine( 0, 0, 0, -static_cast<int>(this->rect().height() / 2));
}


//####################################################################################
//##        Mouse Handling
//####################################################################################
void DrPlaygroundView::mousePressEvent(QMouseEvent *event) {

    QRect box { -990, -990, 1980, 1980 };
    QPointF pos = mapToScene( event->pos() );

    if (event->button() == Qt::MouseButton::LeftButton) {
        if (box.contains(pos.toPoint())) {
            m_playground->addCircle(Body_Type::Dynamic, Dr::GetColor(Window_Colors::Icon_Light), pos.x(), -pos.y(), 0, 50, c_friction, c_bounce, true, true);
        }
    } else if (event->button() == Qt::MouseButton::RightButton) {
        if (box.contains(pos.toPoint())) {
            m_playground->addBlock(Body_Type::Dynamic, Dr::GetColor(Window_Colors::Text), pos.x(), -pos.y(), 0, 50, 50, c_friction, c_bounce, true, true);
        }
    }

}

//####################################################################################
//##        Wheel Event / Zoom Functions
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










