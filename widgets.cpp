//
//      Created by Stephens Nunnally on 3/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPainter>

#include "widgets.h"


//####################################################################################
//##    InspectorCategoryButton Class Functions
//####################################################################################
// Constructor for category button, gives button a way to pass click to custom function
CategoryButton::CategoryButton(const QString &text, QColor color, QWidget *parent, QTreeWidgetItem *parent_tree_item) :
    QPushButton(text, parent)
{
    // Forwards user button click to function that expands / contracts
    connect(this, SIGNAL(clicked()), this, SLOT(buttonPressed()));

    m_parent_item = parent_tree_item;
    m_color =       color;
}

// Called by click signal, expands or contracts category after user click
void CategoryButton::buttonPressed()
{
    m_is_shrunk = !m_is_shrunk;
    m_parent_item->setExpanded(!m_is_shrunk);

}

// Override paint event to draw tree exanpsion decoration
void CategoryButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    QPainter painter(this);
    painter.setPen( m_color );
    painter.setBrush( QBrush(m_color, Qt::BrushStyle::SolidPattern) );

    double x = this->geometry().width() - 18;
    double y = this->geometry().height() / 2 - 1;

    QPolygonF triangle;
    if (m_is_shrunk) {
        triangle.append( QPointF(x - 2, y - 4) );       // To the right
        triangle.append( QPointF(x + 2, y + 0) );
        triangle.append( QPointF(x - 2, y + 4) );
    } else {
        triangle.append( QPointF(x - 4, y - 2) );       // To the bottom
        triangle.append( QPointF(x + 4, y - 2) );
        triangle.append( QPointF(x - 0, y + 2) );
    }
    painter.drawPolygon(triangle, Qt::FillRule::OddEvenFill);
}















