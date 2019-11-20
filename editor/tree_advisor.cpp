//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Advisor Definitions
//
//
#include <QLabel>

#include "colors/colors.h"
#include "editor/tree_advisor.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"

#include "helper.h"
#include "helper_qt.h"
#include "interface_editor_relay.h"


//####################################################################################
//##    SLOT, Sets the Advisor Dock text
//####################################################################################
void TreeAdvisor::changeAdvisor(QString header, QString body) {
    if (advisor_mutex.tryLock() == false) return;           // Try and lock mutex to make this thread safe

    this->clear();                                          // Clear / delete all from advisor tree
    m_advisor_header = header;                              // Save Header for reference later
    m_advisor_body = body;

    // Set font sizes
    QFont header_font = Dr::CustomFont();
    QFont body_font = Dr::CustomFont();
    header_font.setPointSize(Dr::FontSize() + 1);
    header_font.setBold(true);

    // Insert top level item to act as header
    QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(this);
    topLevelItem->setFont(0, header_font);
    topLevelItem->setText(0, header);
    this->addTopLevelItem(topLevelItem);

    // Create child tree item for body
    QTreeWidgetItem *sub_item = new QTreeWidgetItem(topLevelItem);
    topLevelItem->addChild(sub_item);

    // Create a label to display body text and format
    QLabel *body_label = new QLabel(body);
    body_label->setFont(body_font);
    body_label->setWordWrap(true);
    body_label->setScaledContents(true);
    body_label->setAlignment(Qt::AlignTop);
    body_label->setContentsMargins(0, 0, 5, 0);

    // Apply label to tree, expand all
    this->setItemWidget(sub_item, 0, body_label);           // Apply text label to tree
    this->expandAll();                                      // Expand the tree

    // Adjust height to maximize advisor text in advisor window
    body_label->setFixedHeight(body_label->rect().height() + (this->rect().bottom() - body_label->rect().bottom() - 1) );

    advisor_mutex.unlock();                                 // Unlock function for other threads
}


//####################################################################################
//##    EVENT, Handles changing the Advisor on Mouse Enter
//####################################################################################
void TreeAdvisor::enterEvent(QEvent *event) {
    m_editor_relay->setAdvisorInfo(Advisor_Info::Advisor_Window);
    QTreeWidget::enterEvent(event);
}







