//
//      Created by Stephens Nunnally on 5/26/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QEvent>
#include <QPainter>
#include <QPushButton>

#include "core/colors/colors.h"
#include "editor/event_filters/event_filters.h"
#include "editor/widgets/widgets_trees.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
TreeCategoryButton::TreeCategoryButton(const QString &text, QColor text_color, QColor disabled_color, QWidget *parent, QTreeWidgetItem *parent_tree_item) :
    QPushButton(text, parent) {

    // Forwards user button click to function that expands / contracts
    this->connect(this, &QPushButton::clicked, [this] () { this->buttonPressed(); });

    // Possible QT515 fix for broken QPushButton signals
    // {
    //  DrFilterClick *filter_click = new DrFilterClick(this);
    //  installEventFilter(filter_click);
    //  connect(filter_click, SIGNAL(mouseClick()), this, SLOT(buttonPressed()));
    // }

    m_parent_item =     parent_tree_item;

    m_text_color =      text_color;
    m_disabled_color =  disabled_color;
}
TreeCategoryButton::~TreeCategoryButton() { }


//####################################################################################
//##    Called by click signal, expands or contracts category after user click
//####################################################################################
void TreeCategoryButton::buttonPressed() {
    m_parent_item->setExpanded( !(m_parent_item->isExpanded()) );
}


//####################################################################################
//##    Override paint event to draw tree expansion decoration
//####################################################################################
void TreeCategoryButton::paintEvent(QPaintEvent *event) {
    QPushButton::paintEvent(event);

    // Don't draw little triangle if it's going to interfere with category label
    QFontMetrics font_metrics { this->font() };
    int width =  font_metrics.horizontalAdvance( this->text() );
    if (this->width() < width + 54) return;

    QPainter painter(this);
    QColor   color = this->isEnabled() ? m_text_color : m_disabled_color;
    painter.setPen( color );
    painter.setBrush( QBrush(color, Qt::BrushStyle::SolidPattern) );

    double x = this->geometry().width() - 18;
    double y = this->geometry().height() / 2 - 1;

    QPolygonF triangle;
    if (!(m_parent_item->isExpanded())) {
        triangle.append( QPointF(x - Dr::Scale(2), y - Dr::Scale(4)) );       // To the right
        triangle.append( QPointF(x + Dr::Scale(2), y + Dr::Scale(0)) );
        triangle.append( QPointF(x - Dr::Scale(2), y + Dr::Scale(4)) );
    } else {
        triangle.append( QPointF(x - Dr::Scale(4), y - Dr::Scale(2)) );       // To the bottom
        triangle.append( QPointF(x + Dr::Scale(4), y - Dr::Scale(2)) );
        triangle.append( QPointF(x - Dr::Scale(0), y + Dr::Scale(2)) );
    }
    painter.drawPolygon(triangle, Qt::FillRule::OddEvenFill);
}













