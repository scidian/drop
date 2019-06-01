//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Advisor Window
//
//
#ifndef EDITOR_TREE_ADVISOR_H
#define EDITOR_TREE_ADVISOR_H
#include <QTreeWidget>
#include <QMutex>

// Forward declarations
class DrProject;
class IEditorRelay;

//####################################################################################
//##    TreeAdvisor
//##        A Tree List to show helpful tips for every item in the editor
//############################
class TreeAdvisor: public QTreeWidget
{
    Q_OBJECT

private:
    // External Borrowed Objects
    DrProject      *m_project;                                      // Pointer to currently loaded project
    IEditorRelay   *m_editor_relay;                                 // Pointer to IEditorRelay class of parent form

    // Local Variables
    QString         advisor_header { "No Data" };                   // Keeps current Advisor Header
    QMutex          advisor_mutex { QMutex::NonRecursive };         // Used to keep building function thread safe

public:
    // Constructor
    explicit        TreeAdvisor(QWidget *parent, DrProject *project, IEditorRelay *editor_relay) :
                                QTreeWidget (parent), m_project(project), m_editor_relay(editor_relay) { }

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual void    enterEvent(QEvent *event) override;

    // Getters and setters
    void            changeAdvisor(QString header, QString body);
    QString         getAdvisorHeader() { return advisor_header; }

};



#endif // EDITOR_TREE_ADVISOR_H










