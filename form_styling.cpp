//
//
//
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"

#include "form_main.h"
#include "form_styling.h"

//#######################################################################
//##
//##    SceneTreeHighlightStyle - A sub classed QProxyStyle so we can overwrite
//##                              drawing of TreeWidget list divider
//##
//#######################################################################
SceneTreeHighlightProxy::~SceneTreeHighlightProxy()
{
    // Must include definition of a virtual destructor
}


void SceneTreeHighlightProxy::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == QStyle::PE_IndicatorItemViewItemDrop)
    {
        if (!m_parent_tree->canWeDrop()) { return; }

        painter->setRenderHint(QPainter::Antialiasing, true);

        QPalette palette;
        QPen    pen(QColor(0, 180, 175, 200));
        QBrush  brush(QColor(0, 180, 175, 50));
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(brush);

        if(option->rect.height() == 0)
        {
            //painter->drawEllipse(option->rect.topLeft(), 3, 3);
            painter->drawLine(QPoint(0, option->rect.top()), QPoint(option->rect.right() + 50, option->rect.top()));
        }
        else {
            //painter->drawRoundedRect(option->rect, 5, 5);
            painter->drawLine(QPoint(0, option->rect.top()), QPoint(option->rect.right() + 50, option->rect.top()));
        }

        m_parent_tree->getMainWindow()->label_2->setText(QString::fromStdString("TLX: ") + QString::number(option->rect.topLeft().x()) +
                                                         QString::fromStdString(", TLY: ") + QString::number(option->rect.topLeft().y()));

    }
    else
    {
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}








