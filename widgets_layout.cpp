//
//      Created by Stephens Nunnally on 3/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QWidget>

#include "widgets_layout.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing) : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing) {
    setContentsMargins(margin, margin, margin, margin);
}
FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing) : m_hSpace(hSpacing), m_vSpace(vSpacing) {
    setContentsMargins(margin, margin, margin, margin);
}
FlowLayout::~FlowLayout() {
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}


//####################################################################################
//##    Overrides for some basic QLayout values
//####################################################################################
void FlowLayout::addItem(QLayoutItem *item) {
    itemList.append(item);
}

int FlowLayout::horizontalSpacing() const {
    if (m_hSpace >= 0)  return m_hSpace;
    else                return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int FlowLayout::verticalSpacing() const {
    if (m_vSpace >= 0)  return m_vSpace;
    else                return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}


QLayoutItem *FlowLayout::itemAt(int index) const            { return itemList.value(index); }

QLayoutItem *FlowLayout::takeAt(int index) {
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    else
        return nullptr;
}

Qt::Orientations FlowLayout::expandingDirections() const    { return nullptr; }
int FlowLayout::count() const                               { return itemList.size(); }
QSize FlowLayout::sizeHint() const                          { return minimumSize(); }
bool FlowLayout::hasHeightForWidth() const                  { return true; }
int FlowLayout::heightForWidth(int width) const
{
    int height = doLayout(QRect(0, 0, width, 0), true);
    return height;
}

QSize FlowLayout::minimumSize() const {
    QSize size;
    for (auto item : itemList)
        size = size.expandedTo(item->minimumSize());
    size += QSize(2 * margin(), 2 * margin());
    return size;
}


//####################################################################################
//##    Rebuilds the layout on size change
//####################################################################################
void FlowLayout::setGeometry(const QRect &rect) {
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

int FlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    for (auto item : itemList) {
        QWidget *widget = item->widget();
        int spaceX = horizontalSpacing();
        if (spaceX == -1)
            spaceX = widget->style()->layoutSpacing(QSizePolicy::Frame, QSizePolicy::Frame, Qt::Horizontal);
        int spaceY = verticalSpacing();
        if (spaceY == -1)
            spaceY = widget->style()->layoutSpacing(QSizePolicy::Frame, QSizePolicy::Frame, Qt::Vertical);

        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y() + bottom;
}

int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
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


















