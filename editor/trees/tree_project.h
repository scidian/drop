//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Tree Project List
//
//
#ifndef EDITOR_TREE_PROJECT_H
#define EDITOR_TREE_PROJECT_H

#include <QCheckBox>
#include <QGraphicsItem>
#include <QProxyStyle>
#include <QTreeWidget>

#include "project/enums_entity_types.h"

// Forward declarations
class IEditorRelay;
class DrScene;
class DrProject;
class DrSettings;
class DrThing;

// Local Defines
#define COLUMN_TITLE    0
#define COLUMN_LOCK     1
#define COLUMN_Z_ORDER  2

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
    long            m_selected_key = c_no_key;          // Holds first selected item in QTreeWidget (treeProjectEditor)
    bool            m_allow_selection_event = true;     // When false, stops selectionChanged event from propagating changes to editor

    QStringList     m_drag_formats;
    bool            m_is_dragging;                      // Set to true when we are dragging
    bool            m_can_drop;
    int             m_mouse_x;
    int             m_mouse_y;

public:
    explicit        TreeProject(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);

    // Function calls
    void                        buildProjectTree(bool total_rebuild = false);
    void                        ensureSelectionVisible();
    QTreeWidgetItem*            findItemWithProjectKey(long key);
    QList<QTreeWidgetItem*>     getListOfAllTreeWidgetItems();
    QList<QTreeWidgetItem*>     getListOfChildrenFromItem( QTreeWidgetItem *item);
    void                        installLockBox(DrSettings *entity, QTreeWidgetItem *thing_item);
    void                        processLockClick(QCheckBox *from_lock);
    void                        updateItems(std::list<DrSettings*> changed_items, std::list<long> property_keys);
    void                        updateSelectionFromKeyList(QList<long> key_list);
    void                        updateSelectionFromView(QList<QGraphicsItem*> item_list);

    // Event Overrides
    virtual void    enterEvent(QEvent *event) override;                                                             // Inherited from QWidget
    virtual void    dragMoveEvent(QDragMoveEvent *event) override;                                                  // Inherited from QAbstractItemView
    virtual void    dropEvent(QDropEvent *event) override;                                                          // Inherited from QTreeWidget
    virtual void    focusInEvent(QFocusEvent *event) override;                                                      // Inherited from QAbstractItemView
    virtual void    keyPressEvent(QKeyEvent *event) override;                                                       // Inherited from QAbstractItemView
    virtual void    selectionChanged (const QItemSelection &selected, const QItemSelection &deselected) override;   // Inherited from QTreeView
    virtual void    startDrag(Qt::DropActions supportedActions) override;                                           // Inherited from QAbstractItemView

    // Getters and setters
    IEditorRelay*           getEditorRelay() { return m_editor_relay; }
    DrProject*              getParentProject() { return m_project; }

    long            getSelectedKey() { return m_selected_key; }
    void            setSelectedKey(long new_key) { m_selected_key = new_key; }

    bool            getIsDragging() { return m_is_dragging; }
    void            setIsDragging(bool new_state) { m_is_dragging = new_state; }

    void            setAllowSelectionEvent(bool allow) { m_allow_selection_event = allow; }

    bool            canWeDrop() { return m_can_drop; }
    int             getMouseX() { return m_mouse_x; }
    int             getMouseY() { return m_mouse_y; }

public slots:
    void handleCollapsed(QTreeWidgetItem *item);
    void handleExpanded(QTreeWidgetItem *item);

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








