//
//      Created by Stephens Nunnally on 3/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QWidget>

#include "editor/editor_tree_assets.h"
#include "globals.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_font.h"
#include "widgets_layout.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrQLayoutFlow::DrQLayoutFlow(QWidget *parent, int margin_left, int margin_right, int margin_top, int margin_bottom, int hSpacing, int vSpacing)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing) {
    this->setObjectName("flowLayout");
    setContentsMargins(margin_left, margin_top, margin_right, margin_bottom);
}

DrQLayoutFlow::DrQLayoutFlow(int margin_left, int margin_right, int margin_top, int margin_bottom, int hSpacing, int vSpacing)
    : m_hSpace(hSpacing), m_vSpace(vSpacing) {
    setContentsMargins(margin_left, margin_top, margin_right, margin_bottom);
}
DrQLayoutFlow::~DrQLayoutFlow() {
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}


//####################################################################################
//##    Overrides for some basic QLayout values
//####################################################################################
void DrQLayoutFlow::addItem(QLayoutItem *item) {
    item_list.append(item);
}

int DrQLayoutFlow::horizontalSpacing() const {
    if (m_hSpace >= 0)  return m_hSpace;
    else                return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int DrQLayoutFlow::verticalSpacing() const {
    if (m_vSpace >= 0)  return m_vSpace;
    else                return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}


QLayoutItem *DrQLayoutFlow::itemAt(int index) const            { return item_list.value(index); }

QLayoutItem *DrQLayoutFlow::takeAt(int index) {
    if (index >= 0 && index < item_list.size())
        return item_list.takeAt(index);
    else
        return nullptr;
}

Qt::Orientations DrQLayoutFlow::expandingDirections() const    { return nullptr; }
int DrQLayoutFlow::count() const                               { return item_list.size(); }
QSize DrQLayoutFlow::sizeHint() const                          { return minimumSize(); }
bool DrQLayoutFlow::hasHeightForWidth() const                  { return true; }
int DrQLayoutFlow::heightForWidth(int width) const
{
    int height = doLayout(QRect(0, 0, width, 0), true);
    return height;
}

QSize DrQLayoutFlow::minimumSize() const {
    QSize size;
    for (auto item : item_list)
        size = size.expandedTo(item->minimumSize());
    size += QSize(2 * margin(), 2 * margin());
    return size;
}


//####################################################################################
//##    Rebuilds the layout on size change
//####################################################################################
void DrQLayoutFlow::setGeometry(const QRect &rect) {
    // Updates asset sizes if necessary
    if (parentWidget()->objectName() == "assetsContainer") {


    }

    QLayout::setGeometry(rect);
    m_last_size.setWidth(  rect.width() );
    m_last_size.setHeight( doLayout(rect, false) );

    // Forces an update of parent tree after resize of layout
    if (parentWidget()->objectName() == "assetsContainer") {
        parentWidget()->setFixedHeight( m_last_size.height() );
        TreeAssets *tree = dynamic_cast<TreeAssets*>(parentWidget()->parentWidget()->parentWidget());
        if (tree) tree->forceUpdateOfItemSizes();
    }
}

int DrQLayoutFlow::doLayout(const QRect &rect, bool test_only) const {
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effective_rect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effective_rect.x();
    int y = effective_rect.y();
    int line_height = 0;

    for (auto item : item_list) {
        int space_x = horizontalSpacing();
        int space_y = verticalSpacing();
        if (space_x == -1)  space_x = item->widget()->style()->layoutSpacing(QSizePolicy::Frame, QSizePolicy::Frame, Qt::Horizontal);
        if (space_y == -1)  space_y = item->widget()->style()->layoutSpacing(QSizePolicy::Frame, QSizePolicy::Frame, Qt::Vertical);

        int next_x = x + item->sizeHint().width() + space_x;
        if (next_x - space_x > effective_rect.right() && line_height > 0) {
            x = effective_rect.x();
            y = y + line_height + space_y;
            next_x = x + item->sizeHint().width() + space_x;
            line_height = 0;
        }

        if (!test_only)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

        x = next_x;
        line_height = qMax(line_height, item->sizeHint().height());
    }

    int new_height = y + line_height - rect.y() + bottom;
    return new_height;
}

int DrQLayoutFlow::smartSpacing(QStyle::PixelMetric pm) const {
    QObject *parent = this->parent();
    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget *pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    } else {
        return static_cast<QLayout *>(parent)->spacing();
    }
}


















