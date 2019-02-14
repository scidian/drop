//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Tree Project List
//
//
#ifndef EDITOR_TREE_PROJECT_H
#define EDITOR_TREE_PROJECT_H

#include <QProxyStyle>
#include <QTreeWidget>

class DrProject;
class InterfaceRelay;
class DrScene;

//####################################################################################
//##    TreeProject
//##        A Tree List of Current Project
//############################
class TreeProject: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject      *m_project;                          // Pointer to currently loaded project
    InterfaceRelay *m_relay;                            // Pointer to InterfaceRelay class of parent form

    long            m_selected_key = 0;                 // Holds first selected item in QTreeWidget (treeProject)
    bool            m_allow_selection_event = true;     // When false, stops selectionChanged event from propagating changes to editor

    bool            m_is_dragging;                      // Set to true when we are dragging
    bool            m_can_drop;
    int             m_mouse_x;
    int             m_mouse_y;

public:
    explicit        TreeProject(QWidget *parent, DrProject *project, InterfaceRelay *relay) :
                                QTreeWidget (parent), m_project(project), m_relay(relay) { }

    // Function calls
    void                        buildProjectTree();
    QList <QTreeWidgetItem*>    getListOfAllTreeWidgetItems();
    QList <QTreeWidgetItem*>    getListOfChildrenFromItem( QTreeWidgetItem *item );

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual bool    eventFilter(QObject *obj, QEvent *event) override;                                             // Inherited from QObject
    virtual void    enterEvent(QEvent *event) override;                                                            // Inherited from QWidget
    virtual void    dragMoveEvent(QDragMoveEvent *event) override;                                                 // Inherited from QAbstractItemView
    virtual void    dropEvent(QDropEvent *event) override;                                                         // Inherited from QTreeWidget
    virtual void    selectionChanged (const QItemSelection &selected, const QItemSelection &deselected) override;  // Inherited from QTreeView
    virtual void    startDrag(Qt::DropActions supportedActions) override;                                          // Inherited from QAbstractItemView

    // Getters and setters
    long            getSelectedKey() { return m_selected_key; }
    void            setSelectedKey(long new_key) { m_selected_key = new_key; }

    bool            getIsDragging() { return m_is_dragging; }
    void            setIsDragging(bool new_state) { m_is_dragging = new_state; }

    void            setAllowSelectionEvent(bool allow) { m_allow_selection_event = allow; }

    bool            canWeDrop() { return m_can_drop; }
    int             getMouseX() { return m_mouse_x; }
    int             getMouseY() { return m_mouse_y; }

};




//####################################################################################
//##    StageTreeHighlightStyle
//##        A sub classed QProxyStyle so we can overwrite events and do some custom
//##        drawing of TreeWidget list divider in Tree Stage List
//############################
class StageTreeHighlightProxy : public QProxyStyle
{
private:
    TreeProject     *m_parent_tree;              // Pointer to parent TreeProject
    InterfaceRelay  *m_relay;                    // Pointer to InterfaceRelay class of parent form

public:
    explicit StageTreeHighlightProxy(QStyle *baseStyle, TreeProject *parent_tree, InterfaceRelay *relay) :
                                     QProxyStyle(baseStyle), m_parent_tree(parent_tree), m_relay(relay) { }
    virtual ~StageTreeHighlightProxy() override;

    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

};



#endif // EDITOR_TREE_PROJECT_H








