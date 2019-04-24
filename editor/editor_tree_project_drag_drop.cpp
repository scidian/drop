//
//      Created by Stephens Nunnally on 4/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QPainter>

#include "debug.h"
#include "editor_tree_project.h"
#include "globals.h"
#include "project/project.h"
#include "settings/settings.h"


//####################################################################################
//##        Drag / Drop Events
//####################################################################################
// This removes the item from under the mouse, sort of
void TreeProject::startDrag(Qt::DropActions supportedActions)
{
    // Partly copied from Qt 5.5.5 sources
    QModelIndexList indexes = selectedIndexes();
    if (indexes.count() > 0) {
        // Get a list of the supported MIMEs of the selected indexes
        QMimeData* data = model()->mimeData(indexes);
        if (!data) {
            return;
        }
        m_drag_formats = data->formats();

        // Create the drag object
        QDrag* drag = new QDrag(this);
        drag->setMimeData(data);
        drag->setPixmap(QPixmap());

        // Execute the drag
        drag->exec(supportedActions, Qt::MoveAction);
    }
}


// Fires when we start dragging
void TreeProject::dragMoveEvent(QDragMoveEvent *event)
{
    bool check_format = false;
    for (auto format : m_drag_formats) {
        if (event->mimeData()->hasFormat( format ))
            check_format = true;
    }
    if (!check_format) {
        event->ignore();
        return;
    }

    m_mouse_x = event->pos().x();
    m_mouse_y = event->pos().y();

    // !!!!! #DEBUG:    Show stage tree drag event info
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Stage_Tree_Drag)) {
        Dr::SetLabelText(Label_Names::Label_1, QString::fromStdString("MX: ") + QString::number(m_mouse_x) +
                                               QString::fromStdString(", MY: ") + QString::number(m_mouse_y) );
    }
    // !!!!! END

    // Get item under mouse, if not null lets process it to see if we will allow dropping there
    QTreeWidgetItem *item_at = this->itemAt(event->pos());
    m_can_drop = false;
    if (item_at != nullptr)
    {
        long        check_key = item_at->data(0, User_Roles::Key).toLongLong();

        // !!!!! #DEBUG:    Show stage tree drag event info
        if (Dr::CheckDebugFlag(Debug_Flags::Label_Stage_Tree_Drag)) {
            Dr::SetLabelText(Label_Names::Label_Object_3, "Selected: " + QString::number(m_selected_key) +
                                                        ", Checking: " + QString::number(check_key) );
        }
        // !!!!! END

        // Check if its the same type as already selected, if so allow possible drop
        if (m_is_dragging && m_selected_key != 0 && check_key != 0) {
            DrSettings *check_settings =    m_project->findSettingsFromKey(check_key);
            DrSettings *selected_settings = m_project->findSettingsFromKey(m_selected_key);

            if ( check_settings->getType() == selected_settings->getType() ) { m_can_drop = true; }
        }
    }

    setDropIndicatorShown(true);
    m_is_dragging = true;
    QTreeWidget::dragMoveEvent(event);
}

void TreeProject::dropEvent(QDropEvent* event)
{
    bool check_format = false;
    for (auto format : m_drag_formats) {
        if (event->mimeData()->hasFormat( format ))
            check_format = true;
    }
    if (!check_format) {
        event->ignore();
        return;
    }

    bool dropOK = false;
    DropIndicatorPosition dropIndicator = dropIndicatorPosition();

    switch (dropIndicator) {
        case QAbstractItemView::AboveItem:  dropOK = true;  break;
        case QAbstractItemView::BelowItem:  dropOK = true;  break;
        case QAbstractItemView::OnItem:     dropOK = false; break;
        case QAbstractItemView::OnViewport: dropOK = false; break;
    }

    if (dropOK && m_can_drop)
    {
        // ########## !!!!!!!!! Here we need manage the case of dropping an item
        QTreeWidget::dropEvent(event);              // Pass event through to base
    }
    else
    {
        event->ignore();
    }
    setDropIndicatorShown(false);                   // hide the drop indicator once the drop is done
    m_is_dragging = false;
}




//####################################################################################
//##    StageTreeHighlightStyle
//##        A sub classed QProxyStyle so we can overwrite events and do some custom
//##        drawing of TreeWidget list divider in Tree Stage List while dragging
//####################################################################################
StageTreeHighlightProxy::~StageTreeHighlightProxy()
{
    // Must include definition of a virtual destructor
}

void StageTreeHighlightProxy::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == QStyle::PE_IndicatorItemViewItemDrop) {
        if (!m_parent_tree->canWeDrop()) { return; }

        QPalette palette;
        QPen    pen(QColor(0, 180, 175, 200));
        QBrush  brush(QColor(0, 180, 175, 50));

        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(brush);

        if(option->rect.height() == 0) {

            painter->drawLine(QPoint(0, option->rect.top()), QPoint(option->rect.right() + 50, option->rect.top()));
        } else {

            painter->drawLine(QPoint(0, option->rect.top()), QPoint(option->rect.right() + 50, option->rect.top()));
        }

        // !!!!! #DEUBG:    Show custom highlight event data
        if (Dr::CheckDebugFlag(Debug_Flags::Label_Stage_Tree_Drag)) {
            Dr::SetLabelText(Label_Names::Label_2, QString::fromStdString("TLX: ") + QString::number(option->rect.topLeft().x()) +
                                                 QString::fromStdString(", TLY: ") + QString::number(option->rect.topLeft().y()));
        }
        // !!!!! END

    } else {
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}










