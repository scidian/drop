//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Tree Project List
//
//
#ifndef EDITOR_TREE_PROJECT_H
#define EDITOR_TREE_PROJECT_H

#include <QGraphicsItem>
#include <QProxyStyle>
#include <QTreeWidget>

#include "enums.h"

// Forward declarations
class IEditorRelay;
class DrScene;
class DrProject;
class DrSettings;

//####################################################################################
//##    TreeProject
//##        A Tree List of Current Project
//############################
class TreeProject: public QTreeWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject      *m_project;                          // Pointer to currently loaded project
    IEditorRelay   *m_editor_relay;                     // Pointer to IEditorRelay class of parent form

    // Local Variables
    long            m_selected_key = c_no_key;          // Holds first selected item in QTreeWidget (treeProject)
    bool            m_allow_selection_event = true;     // When false, stops selectionChanged event from propagating changes to editor

    QStringList     m_drag_formats;
    bool            m_is_dragging;                      // Set to true when we are dragging
    bool            m_can_drop;
    int             m_mouse_x;
    int             m_mouse_y;

public:
    explicit        TreeProject(QWidget *parent, DrProject *project, IEditorRelay *editor_relay) :
                                QTreeWidget (parent), m_project(project), m_editor_relay(editor_relay) { }

    // Function calls
    void                        buildProjectTree();
    QList <QTreeWidgetItem*>    getListOfAllTreeWidgetItems();
    QList <QTreeWidgetItem*>    getListOfChildrenFromItem( QTreeWidgetItem *item );
    void                        updateItemNames(QList<DrSettings*> changed_items, QList<long> property_keys);
    void                        updateSelectionFromView(QList<QGraphicsItem*> item_list);

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
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
class TreeStageHighlightProxy : public QProxyStyle
{
private:
    // External Borrowed Pointers
    TreeProject     *m_parent_tree;              // Pointer to parent TreeProject

public:
    explicit TreeStageHighlightProxy(QStyle *baseStyle, TreeProject *parent_tree) : QProxyStyle(baseStyle), m_parent_tree(parent_tree) { }
    virtual ~TreeStageHighlightProxy() override;

    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

};



#endif // EDITOR_TREE_PROJECT_H








