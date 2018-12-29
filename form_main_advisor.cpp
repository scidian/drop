//
//
//      Handles dealing with the Advisor
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include "form_main.h"


// Call to put in a signal to change the Advisor to the que
void FormMain::setAdvisorInfo(HeaderBodyList header_body_list)
{
    if (advisor->isHidden()) return;                        // If Advisor dock was closed, cancel
    if (advisor_header == header_body_list[0]) return;      // If Advisor header is already set to proper info, cancel
    emit sendAdvisorInfo(header_body_list);                 // Emits signal connected to changeAdvisor
}

// SLOT, Sets the Advisor Dock text
void FormMain::changeAdvisor(HeaderBodyList header_body_list)
{
    if (advisor_mutex.tryLock() == false) return;           // Try and lock function to make this thread safe

    treeAdvisor->clear();                                   // Clear / delete all from advisor tree

    advisor_header = header_body_list[0];
    QString body_text = QString::fromStdString(header_body_list[1]);

    // Insert top level item to act as header
    QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(treeAdvisor);
    topLevelItem->setText(0, QString::fromStdString(advisor_header));
    treeAdvisor->addTopLevelItem(topLevelItem);

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
    treeAdvisor->setItemWidget(sub_item, 0, body_label);    // Apply text label to tree
    treeAdvisor->expandAll();                               // Expand the tree

    advisor_mutex.unlock();                                 // Unlock function for other threads
}


// Handles changing the Advisor on Mouse Enter
void TreeAdvisorList::enterEvent(QEvent *event)
{
    getMainWindow()->setAdvisorInfo(Advisor_Info::Advisor_Window);
    QTreeWidget::enterEvent(event);
}









