//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Object Inspector List
//
//
#ifndef EDITOR_TREE_INSPECTOR_H
#define EDITOR_TREE_INSPECTOR_H

#include <QtWidgets>

class DrProject;
class InterfaceRelay;


class InspectorCategoryButton;

class TreeInspector: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject      *m_project;                  // Pointer to currently loaded project
    InterfaceRelay *m_interface;                // Pointer to interface class of parent form

public:
    explicit        TreeInspector(QWidget *parent, DrProject *project, InterfaceRelay *interface) :
                                  QTreeWidget (parent), m_project(project), m_interface(interface) {
        connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemWasClicked(QTreeWidgetItem *, int)));
    }

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual void    enterEvent(QEvent *event) override;                                // Inherited from QWidget

    // Function Calls
    void            buildInspectorFromKeys(QList<long> key_list);

private slots:
    void            itemWasClicked(QTreeWidgetItem *item, int column);

};



//############################
//##    InspectorCategoryButton
//##        A sub classed QPushButton so we can override events for header buttons in Object Inspector List
//############################
class InspectorCategoryButton : public QPushButton
{
    Q_OBJECT

private:
    TreeInspector       *m_parent_tree;
    QTreeWidgetItem     *m_parent_item,     *m_child_item;
    QFrame              *m_child_frame;
    int                  m_height;
    bool                 m_is_shrunk = false;
    QRect                start_rect, end_rect;

public:
    InspectorCategoryButton(const QString &text, TreeInspector *parent_tree,
                            QTreeWidgetItem *parent_item, QTreeWidgetItem *child_item, QFrame *new_child);

private slots:
    void animationDone();
    void animationUpdate(const QVariant &value);
    void buttonPressed();
};



#endif // EDITOR_TREE_INSPECTOR_H











