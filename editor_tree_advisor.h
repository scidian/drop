//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Advisor Window
//
//
#ifndef EDITOR_TREE_ADVISOR_H
#define EDITOR_TREE_ADVISOR_H

#include <QtWidgets>

#include "enums.h"

class DrProject;
class InterfaceRelay;


class TreeAdvisor: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject      *m_project;                  // Pointer to currently loaded project
    InterfaceRelay *m_interface;                // Pointer to interface class of parent form

    std::string     advisor_header { "No Data" };                       // Keeps current Advisor Header
    QMutex          advisor_mutex { QMutex::NonRecursive };             // Used to keep building function thread safe

public:
    // Constructor
    explicit        TreeAdvisor(QWidget *parent, DrProject *project, InterfaceRelay *interface) :
                                QTreeWidget (parent), m_project(project), m_interface(interface) { }

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual void enterEvent(QEvent *event) override;

    // Getters and setters
    std::string     getAdvisorHeader() { return advisor_header; }

private slots:
    void        changeAdvisor(HeaderBodyList header_body_list);             // Set as slot so we can emit queued signals
};



#endif // EDITOR_TREE_ADVISOR_H