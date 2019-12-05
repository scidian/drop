//
//      Created by Stephens Nunnally on 12/4/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPainter>

#include "editor/trees/tree_assets.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
AssetCategoryButton::AssetCategoryButton(const QString &text, QColor text_color, QColor disabled_color, QWidget *parent, QTreeWidgetItem *parent_tree_item) :
    QPushButton(text, parent) {
    // Forwards user button click to function that expands / contracts
    connect(this, SIGNAL(clicked()), this, SLOT(buttonPressed()));

    m_parent_item =     parent_tree_item;

    m_text_color =      text_color;
    m_disabled_color =  disabled_color;
}
AssetCategoryButton::~AssetCategoryButton() { }


//####################################################################################
//##    Called by click signal, expands or contracts category after user click
//####################################################################################
void AssetCategoryButton::buttonPressed() {
    m_parent_item->setExpanded( !(m_parent_item->isExpanded()) );
}

//####################################################################################
//##    Override paint event to draw tree expansion decoration
//####################################################################################
void AssetCategoryButton::paintEvent(QPaintEvent *event) {
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











