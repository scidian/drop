//
//      Created by Stephens Nunnally on 5/22/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QStylePainter>

#include "core/colors/colors.h"
#include "editor/widgets/widgets_view.h"
#include "editor/helper_library.h"


//####################################################################################
//##    PAINT: Draw our Rubber Band selection box with custom colors
//####################################################################################
void ViewRubberBand::paintEvent(QPaintEvent *) {
    QStylePainter painter(this);

    QColor bg = Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light));
    bg.setAlpha(48);
    QBrush brush;
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    brush.setColor(bg);

    painter.setBrush(brush);
    painter.setPen(QPen(QBrush(Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light))), 2, Qt::PenStyle::SolidLine));
    painter.drawRect(this->rect());
}





