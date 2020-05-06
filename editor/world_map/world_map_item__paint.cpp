//
//      Created by Stephens Nunnally on 5/6/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsPixmapItem>
#include <QWidget>

#include "core/colors/colors.h"
#include "core/dr_debug.h"
#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/world_map/world_map_item.h"
#include "editor/world_map/world_map_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/settings/settings.h"


//####################################################################################
//##    Custom Paint Handling
//####################################################################################
void WorldMapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    ///if (option->state & QStyle::State_Selected)  { fillColor = QColor(Qt::red); } //m_color.dark(150); }              // If selected
    ///if (option->state & QStyle::State_MouseOver) { fillColor = QColor(Qt::gray); } //fillColor.light(125); }          // If mouse is over

    // Turn off anti aliasing if necessary
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor)) {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    }

    // Check item is not super duper tiny, this seems to crash paint function
    QPolygonF   poly =   sceneTransform().map(boundingRect());
    QRectF      bounds = poly.boundingRect();
    if (bounds.width() < .5 || bounds.height() < .5) return;

    // Set paint option to "not selected" or paint routine will draw dotted lines around item
    ///QStyleOptionGraphicsItem my_option(*option);
    ///my_option.state &= ~QStyle::State_Selected;

    // ***** Apply the proper opacity to this item and either paint the pixmap, or paint a pattern representation of the item
    double transparency = 1.0;
    if (Dr::FuzzyCompare(painter->opacity(), transparency) == false) painter->setOpacity(transparency);

    // Our custom pixmap painting to draw slightly larger and reduce black lines between images
    painter->setRenderHint(QPainter::SmoothPixmapTransform, (this->transformationMode() == Qt::SmoothTransformation));
    QRectF dest = this->pixmap().rect();
    painter->drawPixmap(dest, this->pixmap(), this->pixmap().rect());

    // Original drawing using QtSource
    ///QGraphicsPixmapItem::paint(painter, my_option, widget);


}












