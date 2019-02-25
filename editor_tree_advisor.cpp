//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Advisor Definitions
//
//
#include <QLabel>

#include "colors.h"

#include "editor_tree_advisor.h"

#include "interface_relay.h"

#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

//####################################################################################
//##        SLOT, Sets the Advisor Dock text
//####################################################################################
void TreeAdvisor::changeAdvisor(QString header, QString body)
{
    if (advisor_mutex.tryLock() == false) return;           // Try and lock function to make this thread safe

    this->clear();                                          // Clear / delete all from advisor tree

    advisor_header = header;
    QString body_text = body;

    // Insert top level item to act as header
    QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(this);
    topLevelItem->setText(0, advisor_header);
    this->addTopLevelItem(topLevelItem);

    // Create child tree item for body
    QTreeWidgetItem *sub_item = new QTreeWidgetItem(topLevelItem);
    topLevelItem->addChild(sub_item);

    // Create a label to display body text and format
    QLabel *body_label = new QLabel(body_text);
    QFont font_label;
    font_label.setPointSize(Dr::FontSize());
    body_label->setFont(font_label);
    body_label->setWordWrap(true);
    body_label->setScaledContents(true);
    body_label->setAlignment(Qt::AlignTop);

    // Apply label to tree, expand all
    this->setItemWidget(sub_item, 0, body_label);           // Apply text label to tree
    this->expandAll();                                      // Expand the tree

    // Adjust height to maximize advisor text in advisor window
    body_label->setFixedHeight(body_label->rect().height() + (this->rect().bottom() - body_label->rect().bottom() - 1) );

    advisor_mutex.unlock();                                 // Unlock function for other threads
}


//####################################################################################
//##        EVENT, Handles changing the Advisor on Mouse Enter
//####################################################################################
void TreeAdvisor::enterEvent(QEvent *event)
{
    m_relay->setAdvisorInfo(Advisor_Info::Advisor_Window);
    QTreeWidget::enterEvent(event);
}







