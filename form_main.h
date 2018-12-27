//
//
//      FormMain - Class that holds our main form window
//
//      FormMain Modes:
//          Edit Scene
//          Edit UI
//          Node Map: World / UI Layout
//          Scene Map: Scene Layout
//
//      Main Components of FormMain while in normal "Edit Scene" mode:
//          Top Area (Toolbar)
//          Advisor (Dock)
//          Object Inspector (Dock)
//
//          Selectable Components:
//              Asset List (Dock)
//              Scene Area
//              Scene List
//              Variable List
//              Bottom Area (Labels, Scenes?)
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>                    // Header includes all QtWidget items

#include "01_project.h"
#include "globals.h"

// Necessary forward declarations
class SceneGraphicsView;
class TreeSceneView;
class TreeObjectInspector;
class TreeAssetList;

// Local enum for referencing debug labels
enum class Label_Names
{
    Label1,
    Label2,
    Label3,
    LabelObject1,
    LabelObject2,
    LabelObject3,
};


//####################################################################################
//##    FormMain - Main editor window
//############################
class FormMain : public QMainWindow
{
    Q_OBJECT

public:
    // Locals
    Globals        *globals;                                            // Holds project globals

    // Locals that need to be SAVED / LOADED from each project
    DrProject      *project;                                            // Holds whatever the current open game project is
    long            current_world;                                      // Tracks which world to show in the scene viewer

private:
    // Holds the currently selected scene, ready for rendering in a QGraphicsView
    QGraphicsScene      *scene;
    SceneGraphicsView   *viewMain;

    // Custom classes for Scene List
    TreeSceneView       *treeScene;

    // Custom classes for Object Inspector
    TreeObjectInspector *treeObject;

    // Custom classes for Asset List
    TreeAssetList       *treeAsset;

    // Normal Qt Classes for simple objects
    QMenu         *menuDrop;
    QMenuBar      *menuBar;
    QWidget       *widgetAdvisor, *widgetAssests, *widgetBottom, *widgetCentral, *widgetInner, *widgetInspector, *widgetToolbar;
    QScrollArea   *areaBottom;
    QTreeWidget   *treeAdvisor;

    QHBoxLayout   *horizontalLayout;
    QVBoxLayout   *verticalLayout, *verticalLayoutObject, *verticalLayoutAdvisor, *verticalLayoutAsset;
    QSplitter     *splitterHorizontal, *splitterVertical;

    QDockWidget   *advisor, *assets, *inspector, *toolbar;
    QPushButton   *buttonAtlas, *buttonFonts, *buttonPlay, *buttonSettings, *buttonWorlds;

    // Labels to display info
    QLabel        *label_1,         *label_2,           *label_3;
    QLabel        *label_object_1,  *label_object_2,    *label_object_3;

public:
    // Constructor and Destructor
    explicit FormMain(QWidget *parent = nullptr, Globals *the_globals = nullptr);
    ~FormMain();

    // Form setup
    void            applyColoring();
    void            buildWindow();

    // Member functions
    void            setAdvisorInfo(HeaderBodyList header_body_list);
    void            setLabelText(Label_Names label_name, QString new_text);

    // Scene Handling
    void            populateScene();

    // Tree Scene List Handling
    void            listSelectionChanged(QList<QTreeWidgetItem*> item_list);
    void            populateTreeSceneList();

    // Object Inspector Handling
    void            buildObjectInspector();

private:
    void            setAdvisorInfo(QString header_text, QString body_text);         // Make function private to force calls with Advisor type from enums.h
};




//####################################################################################
//##    SceneGraphicsView
//##        A sub classed QGraphics so we can override events for our View Area
//############################
class SceneGraphicsView : public QGraphicsView
{
    Q_OBJECT

private:
    // Local member variables
    FormMain   *m_parent_window;
    int         m_zoom = 250;
    int         m_rotate = 0;

public:
    // Constructor
    explicit SceneGraphicsView(QWidget *parent, FormMain *main_window) : QGraphicsView(parent = nullptr), m_parent_window (main_window) { }

    // Event overrides
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *) override;
#endif

    // Getters and setters
    FormMain*    getMainWindow() { return m_parent_window; }

    // Custom handling functions
    void applyUpdatedMatrix();
    void zoomInOut(int level);

};



//####################################################################################
//##    TreeObjectInspector
//##        A sub classed QTreeWidget so we can override events for Object Inspector List
//############################
class TreeObjectInspector: public QTreeWidget
{
    Q_OBJECT

private:
    FormMain    *m_parent_window;

public:
    explicit TreeObjectInspector(QWidget *parent, FormMain *main_window) : QTreeWidget (parent), m_parent_window (main_window) { }

    // Event Overrides, start at Qt Documentation for QTreeWidget Class to find more events
    virtual void enterEvent(QEvent *event) override;                                                            // Inherited from QWidget

    // Getters and setters
    FormMain*    getMainWindow() { return m_parent_window; }
};

//############################
//##    InspectorCategoryButton
//##        A sub classed QPushButton so we can override events for header buttons in Object Inspector List
//############################
class InspectorCategoryButton : public QPushButton
{
    Q_OBJECT

public:
    InspectorCategoryButton(const QString& a_Text, QTreeWidget* a_pParent, QTreeWidgetItem* a_pItem, QFrame* new_child);

private slots:
    void ButtonPressed();

private:
    QTreeWidgetItem *m_pItem;
    QFrame          *m_child_frame;
    QRect            m_rect;
};



//####################################################################################
//##    TreeSceneView
//##        A sub classed QTreeWidget so we can override events for Tree Scene List
//############################
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

    // Event Overrides, start at Qt Documentation for QTreeWidget Class to find more events
    virtual void dragMoveEvent(QDragMoveEvent *event) override;                                                 // Inherited from QAbstractItemView
    virtual void dropEvent(QDropEvent *event) override;                                                         // Inherited from QTreeWidget
    virtual void selectionChanged (const QItemSelection &selected, const QItemSelection &deselected) override;  // Inherited from QTreeView
    virtual void startDrag(Qt::DropActions supportedActions) override;                                          // Inherited from QAbstractItemView
    virtual void enterEvent(QEvent *event) override;                                                            // Inherited from QWidget

    // Getters and setters
    FormMain*    getMainWindow() { return m_parent_window; }
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
    TreeSceneView  *m_parent_tree;

public:
    explicit SceneTreeHighlightProxy(QStyle *baseStyle, TreeSceneView *parent_tree) : QProxyStyle(baseStyle), m_parent_tree(parent_tree) { }
    virtual ~SceneTreeHighlightProxy() override;

    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;

};


//####################################################################################
//##    TreeAssetList
//##        A sub classed QTreeWidget so we can override events for Assests List
//############################
class TreeAssetList: public QTreeWidget
{
    Q_OBJECT

private:
    FormMain    *m_parent_window;

public:
    explicit TreeAssetList(QWidget *parent, FormMain *main_window) : QTreeWidget (parent), m_parent_window (main_window) { }

    // Event Overrides, start at Qt Documentation for QTreeWidget Class to find more events
    virtual void enterEvent(QEvent *event) override;                                                            // Inherited from QWidget

    FormMain*    getMainWindow() { return m_parent_window; }
};



#endif // MAINWINDOW_H









