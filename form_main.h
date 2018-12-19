//
//
//      FormMain - Class that holds our main form window
//
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>                    // Header includes all QtWidget items

#include "01_project.h"
#include "globals.h"


//####################################################################################
//##    FormMain - Main editor window
//####################################################################################
class TreeSceneView;                        // Necessary forward declaration

class FormMain : public QMainWindow
{
    Q_OBJECT

public:
    // Locals
    Globals        *globals;                                            // Holds project globals

    // Locals that need to be SAVED / LOADED from each project
    DrProject      *project;                                            // Holds whatever the current open game project is
    long            current_world;                                      // Tracks which world to show in the scene viewer

    // Public labels to make changing text easier
    QScrollArea   *areaBottom;
    QLabel        *label_1,         *label_2,           *label_3;
    QLabel        *label_object,    *label_object_2,    *label_object_3;

private:
    TreeSceneView *treeScene;
    QTableWidget  *tableWidget;

    QMenu         *menuDrop;
    QMenuBar      *menuBar;
    QWidget       *widgetAssests, *widgetBottom, *widgetCentral, *widgetInner, *widgetInspector, *widgetToolbar;

    QHBoxLayout   *horizontalLayout;
    QVBoxLayout   *verticalLayout;
    QSplitter     *splitterHorizontal, *splitterVertical;

    QDockWidget   *assets, *inspector, *toolbar;
    QPushButton   *buttonAtlas, *buttonFonts, *buttonPlay, *buttonSettings, *buttonWorlds;


private slots:
    void on_tableWidget_itemClicked(QTableWidgetItem *item);            // Handles click on Object Inspector table

public:
    // Constructor and Destructor
    explicit FormMain(QWidget *parent = nullptr, Globals *the_globals = nullptr);
    ~FormMain();

    // Form setup
    void            applyColoring(Color_Scheme new_color);
    void            applyPalette(Color_Scheme new_scheme);
    void            buildWindow();

    // Tree Scene List Handling
    void            listSelectionChanged(QList<QTreeWidgetItem*> item_list);
    void            populateTreeSceneList();

    // Object Inspector Handling
    void            buildObjectInspector();

};


//####################################################################################
//##    MyTreeView - A sub classed QTreeWidget so we can override events
//####################################################################################
class TreeSceneView: public QTreeWidget
{
    Q_OBJECT
private:
    FormMain    *m_parent_window;
    long         m_selected_key = 0;                                    // Holds first selected item in QTreeWidget (treeScene)
    bool         m_is_dragging;                                         // Set to true when we are dragging
    bool         m_can_drop;
    int          m_mouse_x;
    int          m_mouse_y;

public:
    explicit TreeSceneView(QWidget *parent, FormMain *main_window) : QTreeWidget (parent), m_parent_window (main_window) { }

    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
    virtual void selectionChanged (const QItemSelection &selected, const QItemSelection &deselected) override;
    virtual void startDrag(Qt::DropActions supportedActions) override;

    FormMain*    getMainWindow() { return m_parent_window; }
    long         getSelectedKey() { return m_selected_key; }
    void         setSelectedKey(long new_key) { m_selected_key = new_key; }

    bool         getIsDragging() { return m_is_dragging; }
    void         setIsDragging(bool new_state) { m_is_dragging = new_state; }

    bool         canWeDrop() { return m_can_drop; }
    int          getMouseX() { return m_mouse_x; }
    int          getMouseY() { return m_mouse_y; }
};



//####################################################################################
//##    SceneTreeHighlightStyle
//##        A sub classed QProxyStyle so we can overwrite event and do
//##        some cuistom drawing of TreeWidget list divider
//####################################################################################
class SceneTreeHighlightProxy : public QProxyStyle
{
private:
    TreeSceneView  *m_parent_tree;

public:
    explicit SceneTreeHighlightProxy(QStyle *baseStyle, TreeSceneView *parent_tree) : QProxyStyle(baseStyle), m_parent_tree(parent_tree) { }
    virtual ~SceneTreeHighlightProxy() override;

    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

};





#endif // MAINWINDOW_H









