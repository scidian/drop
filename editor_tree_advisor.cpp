//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Advisor Definitions
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include "editor_tree_advisor.h"
#include "interface_relay.h"


// SLOT, Sets the Advisor Dock text
void TreeAdvisor::changeAdvisor(HeaderBodyList header_body_list)
{
    if (advisor_mutex.tryLock() == false) return;           // Try and lock function to make this thread safe

    this->clear();                                          // Clear / delete all from advisor tree

    advisor_header = header_body_list[0];
    QString body_text = QString::fromStdString(header_body_list[1]);

    // Insert top level item to act as header
    QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(this);
    topLevelItem->setText(0, QString::fromStdString(advisor_header));
    this->addTopLevelItem(topLevelItem);

    // Create child tree item for body
    QTreeWidgetItem *sub_item = new QTreeWidgetItem(topLevelItem);
    topLevelItem->addChild(sub_item);

    // Create a label to display body text and format
    QLabel *body_label = new QLabel(body_text);
    QFont font_label;
    font_label.setPointSize(11);
    body_label->setFont(font_label);
    body_label->setWordWrap(true);
    body_label->setAlignment(Qt::AlignTop);

    // Apply label to tree, expand all
    this->setItemWidget(sub_item, 0, body_label);           // Apply text label to tree
    this->expandAll();                                      // Expand the tree

    advisor_mutex.unlock();                                 // Unlock function for other threads
}


// Handles changing the Advisor on Mouse Enter
void TreeAdvisor::enterEvent(QEvent *event)
{
    m_interface->setAdvisorInfo(Advisor_Info::Advisor_Window);
    QTreeWidget::enterEvent(event);
}




