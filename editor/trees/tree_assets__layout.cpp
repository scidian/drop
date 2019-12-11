//
//      Created by Stephens Nunnally on 3/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QWidget>

#include "editor/globals_editor.h"
#include "editor/trees/tree_assets.h"
#include "model/project/project.h"
#include "model/project/project_asset.h"
#include "model/project/project_font.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
AssetFlowLayout::AssetFlowLayout(DrProject *project, QWidget *parent,
                                 int margin_left, int margin_right, int margin_top, int margin_bottom,
                                 int hSpacing, int vSpacing) : QLayout(parent), m_project(project), m_hSpace(hSpacing), m_vSpace(vSpacing) {
    this->setObjectName("flowLayout");
    setContentsMargins(margin_left, margin_top, margin_right, margin_bottom);
}

AssetFlowLayout::AssetFlowLayout(DrProject *project,
                                 int margin_left, int margin_right, int margin_top, int margin_bottom,
                                 int hSpacing, int vSpacing) : m_project(project), m_hSpace(hSpacing), m_vSpace(vSpacing) {
    setContentsMargins(margin_left, margin_top, margin_right, margin_bottom);
}

AssetFlowLayout::~AssetFlowLayout() {
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}


//####################################################################################
//##    Overrides for some basic QLayout values
//####################################################################################
void AssetFlowLayout::addItem(QLayoutItem *item) {
    item_list.append(item);
}

int AssetFlowLayout::horizontalSpacing() const {
    if (m_hSpace >= 0)  return m_hSpace;
    else                return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int AssetFlowLayout::verticalSpacing() const {
    if (m_vSpace >= 0)  return m_vSpace;
    else                return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}


QLayoutItem *AssetFlowLayout::itemAt(int index) const           { return item_list.value(index); }

QLayoutItem *AssetFlowLayout::takeAt(int index) {
    if (index >= 0 && index < item_list.size())
        return item_list.takeAt(index);
    else
        return nullptr;
}

Qt::Orientations AssetFlowLayout::expandingDirections() const   { return nullptr; }
int AssetFlowLayout::count() const                              { return item_list.size(); }
QSize AssetFlowLayout::sizeHint() const                         { return minimumSize(); }
bool AssetFlowLayout::hasHeightForWidth() const                 { return true; }

int AssetFlowLayout::heightForWidth(int width) const {
    int row_width;
    int height = doLayout(QRect(0, 0, width, 0), row_width, true);
    return height;
}

QSize AssetFlowLayout::minimumSize() const {
    QSize size;
    for (auto item : item_list)
        size = size.expandedTo(item->minimumSize());
    size += QSize(2 * margin(), 2 * margin());
    return size;
}


//####################################################################################
//##    Sort Items by Name
//####################################################################################
void AssetFlowLayout::sortItems() {
    std::sort(item_list.begin(), item_list.end(), [this](QLayoutItem *a, QLayoutItem *b) {
        long key_a = a->widget()->property(User_Property::Key).toLongLong();
        long key_b = b->widget()->property(User_Property::Key).toLongLong();
        DrSettings *asset_a = this->m_project->findSettingsFromKey(key_a);
        DrSettings *asset_b = this->m_project->findSettingsFromKey(key_b);
        if (asset_a == nullptr || asset_b == nullptr) return false;
        return QString::fromStdString(asset_a->getName()).toLower() < QString::fromStdString(asset_b->getName()).toLower();
    });
}


//####################################################################################
//##    Rebuilds the layout on size change
//####################################################################################
void AssetFlowLayout::setGeometry(const QRect &rect) {
    // Updates asset sizes if necessary
    if (parentWidget() != nullptr) {
        if (parentWidget()->objectName() == "assetsContainer") {

        }
    }

    QLayout::setGeometry(rect);
    m_last_size.setWidth(  rect.width() );
    m_last_size.setHeight( doLayout(rect, m_row_width, false) );

    // Forces an update of parent tree after resize of layout
    if (parentWidget() != nullptr) {
        if (parentWidget()->objectName() == "assetsContainer") {
            parentWidget()->setFixedHeight( m_last_size.height() );
            TreeAssets *tree = dynamic_cast<TreeAssets*>(parentWidget()->parentWidget()->parentWidget());
            if (tree != nullptr) tree->forceUpdateOfItemSizes();
        }
    }
}

int AssetFlowLayout::doLayout(const QRect &rect, int &row_width, bool test_only) const {
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effective_rect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effective_rect.x();
    int y = effective_rect.y();
    int line_height = 0;
    row_width = 0;

    int column_count = 0;
    for (auto item : item_list) {
        int space_x = horizontalSpacing();
        int space_y = verticalSpacing();
        if (space_x == -1) space_x = item->widget()->style()->layoutSpacing(QSizePolicy::Frame, QSizePolicy::Frame, Qt::Horizontal);
        if (space_y == -1) space_y = item->widget()->style()->layoutSpacing(QSizePolicy::Frame, QSizePolicy::Frame, Qt::Vertical);

        int next_x = x + item->sizeHint().width() + space_x;
        row_width = qMax(row_width, column_count);
        column_count++;
        if (next_x - space_x > effective_rect.right() && line_height > 0) {
            x = effective_rect.x();
            y = y + line_height + space_y;
            next_x = x + item->sizeHint().width() + space_x;
            line_height = 0;
            column_count = 0;
        }

        if (!test_only) {
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        }

        x = next_x;
        line_height = qMax(line_height, item->sizeHint().height());
    }

    row_width = qMax(row_width, column_count);
    int new_height = y + line_height - rect.y() + bottom;
    return new_height;
}

int AssetFlowLayout::smartSpacing(QStyle::PixelMetric pm) const {
    QObject *parent = this->parent();
    if (parent == nullptr) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget *pw = static_cast<QWidget*>(parent);
        if (pw == nullptr) return -1;
        return pw->style()->pixelMetric(pm, nullptr, pw);
    } else {
        return static_cast<QLayout*>(parent)->spacing();
    }
}


















