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
class LabelHoverHandler;


//####################################################################################
//##    TreeInspector
//##        A Tree List to show properties of items / classes / objects
//############################
class TreeInspector: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject           *m_project;                  // Pointer to currently loaded project
    InterfaceRelay      *m_relay;                    // Pointer to InterfaceRelay class of parent form

    LabelHoverHandler   *m_label_hover;           // Pointer to a label hover handler

public:
    explicit        TreeInspector(QWidget *parent, DrProject *project, InterfaceRelay *relay);

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual void    enterEvent(QEvent *event) override;                                // Inherited from QWidget

    // Function Calls
    void            buildInspectorFromKeys(QList<long> key_list);
    InterfaceRelay* getRelay() { return m_relay; }

private slots:
    void            itemWasClicked(QTreeWidgetItem *item, int column);
    void            setAdvisorInfo(QString header, QString body);

};



//####################################################################################
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
    QString              m_header, m_body;

public:
    InspectorCategoryButton(const QString &text, TreeInspector *parent_tree,
                            QTreeWidgetItem *parent_item, QTreeWidgetItem *child_item, QFrame *new_child);

    // Events
    virtual void    enterEvent(QEvent *event) override;                                // Inherited from QWidget

    // Getters and Setters
    void            setAdvisorHeaderText(QString header) { m_header = header; }
    void            setAdvisorBodyText(QString body) { m_body = body; }

private slots:
    void animationDone();
    void animationUpdate(const QVariant &value);
    void buttonPressed();
};


//####################################################################################
//##    LabelHoverHandler
//##        Catches hover events for labels on Object Inspector without needing subclassing
//############################
class LabelHoverHandler : public QObject
{
    Q_OBJECT

public:
    LabelHoverHandler(QObject *parent) : QObject(parent) {}
    virtual ~LabelHoverHandler() {}

    void attach(QLabel *label);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void signalMouseHover(QString header, QString body);
};


#endif // EDITOR_TREE_INSPECTOR_H











