//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Tree Scene List
//
//
#ifndef EDITOR_TREE_SCENE_H
#define EDITOR_TREE_SCENE_H

#include <QtWidgets>

class DrProject;
class InterfaceRelay;


class TreeScene: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject      *m_project;                  // Pointer to currently loaded project
    InterfaceRelay *m_interface;                // Pointer to interface class of parent form

    long            m_selected_key = 0;         // Holds first selected item in QTreeWidget (treeScene)
    bool            m_is_dragging;              // Set to true when we are dragging
    bool            m_can_drop;
    int             m_mouse_x;
    int             m_mouse_y;

public:
    explicit        TreeScene(QWidget *parent, DrProject *project, InterfaceRelay *interface) :
                              QTreeWidget (parent), m_project(project), m_interface(interface) { }

    // Function calls
    void            populateTreeSceneList();

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual void enterEvent(QEvent *event) override;                                                            // Inherited from QWidget
    virtual void dragMoveEvent(QDragMoveEvent *event) override;                                                 // Inherited from QAbstractItemView
    virtual void dropEvent(QDropEvent *event) override;                                                         // Inherited from QTreeWidget
    virtual void selectionChanged (const QItemSelection &selected, const QItemSelection &deselected) override;  // Inherited from QTreeView
    virtual void startDrag(Qt::DropActions supportedActions) override;                                          // Inherited from QAbstractItemView

    // Getters and setters
    long         getSelectedKey() { return m_selected_key; }
    void         setSelectedKey(long new_key) { m_selected_key = new_key; }

    bool         getIsDragging() { return m_is_dragging; }
    void         setIsDragging(bool new_state) { m_is_dragging = new_state; }

    bool         canWeDrop() { return m_can_drop; }
    int          getMouseX() { return m_mouse_x; }
    int          getMouseY() { return m_mouse_y; }
};




//############################
//##    SceneTreeHighlightStyle
//##        A sub classed QProxyStyle so we can overwrite events and do some custom
//##        drawing of TreeWidget list divider in Tree Scene List
//############################
class SceneTreeHighlightProxy : public QProxyStyle
{
private:
    TreeScene       *m_parent_tree;              // Pointer to parent TreeScene
    InterfaceRelay  *m_interface;                // Pointer to interface class of parent form

public:
    explicit SceneTreeHighlightProxy(QStyle *baseStyle, TreeScene *parent_tree, InterfaceRelay *interface) :
                                     QProxyStyle(baseStyle), m_parent_tree(parent_tree), m_interface(interface) { }
    virtual ~SceneTreeHighlightProxy() override;

    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

};



#endif // EDITOR_TREE_SCENE_H







