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
//          Advisor (Dock)
//
//          Components That Can Hold Focus:
//              Asset List (Dock)
//              Scene View
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
class SceneGraphicsScene;
class SceneGraphicsView;
class TreeSceneView;
class TreeObjectInspector;
class TreeAssetList;
class TreeAdvisorList;

// Local enum for referencing debug labels
enum class Label_Names
{
    Label1,          Label2,          Label3,
    LabelObject1,    LabelObject2,    LabelObject3,
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
    Form_Main_Mode  current_mode;                                       // Holds current editing mode of FormMain

    // Locals that need to be SAVED / LOADED from each project
    DrProject      *project;                                            // Holds whatever the current open game project is
    long            current_world;                                      // Tracks which world to show in the scene viewer

private:
    std::string     advisor_header { "No Data" };                       // Keeps current Advisor Header
    QMutex          advisor_mutex { QMutex::NonRecursive };             // Used to keep building function thread safe

    SceneGraphicsScene  *scene;                 // Holds the currently selected scene, ready for rendering in SceneGraphicsView
    SceneGraphicsView   *viewMain;              // Renders scene for the viewer

    TreeSceneView       *treeScene;             // Custom classes for Scene List
    TreeObjectInspector *treeObject;            // Custom classes for Object Inspector
    TreeAssetList       *treeAsset;             // Custom classes for Asset List
    TreeAdvisorList     *treeAdvisor;           // Custom classes for Advisor List

    // Normal Qt Classes for simple objects
    QMenuBar      *menuBar;
    QWidget       *widgetAdvisor, *widgetAssests, *widgetCentral, *widgetScene, *widgetInspector, *widgetToolbar;
    QScrollArea   *areaBottom;

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

    // Member functions
    void        setAdvisorInfo(HeaderBodyList header_body_list);
    void        setLabelText(Label_Names label_name, QString new_text);

    // Scene Handling
    void        populateScene();

    // Tree Scene List Handling
    void        listSelectionChanged(QList<QTreeWidgetItem*> item_list);
    void        populateTreeSceneList();

    // Object Inspector Handling
    void        buildObjectInspector();

private:
    // Form setup
    void        applyColoring();
    void        applyDropShadow(QWidget *target_widget, qreal blur_radius, qreal offset_x, qreal offset_y, QColor shadow_color);
    void        applyDropShadowByType(QWidget *target_widget, Shadow_Types shadow_type);
    void        buildMenu();
    void        buildWindow(Form_Main_Mode new_layout);
    void        buildWindowModeEditScene();
    void        changePalette(Color_Scheme new_color_scheme);
    void        listChildren();
    void        refreshMainView();

private slots:
    void        changeAdvisor(HeaderBodyList header_body_list);             // Set as slot so we can emit queued signals

signals:
    void        sendAdvisorInfo(HeaderBodyList header_body_list);           // Forwards info to MainWindow::changeAdvisor
};


//####################################################################################
//##    SceneGraphicsScene
//##        A sub classed QGraphicsScene so we can override events for our Scene
//############################
class SceneGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

private:
    // Local member variables
    FormMain   *m_parent_window;

public:
    // Constructor
    explicit SceneGraphicsScene(QWidget *parent, FormMain *main_window) : QGraphicsScene(parent = nullptr), m_parent_window (main_window) { }

    // Event Overrides, start at Qt Docs for QGraphicsScene Class to find more
    virtual void keyPressEvent(QKeyEvent *event) override;                              // Inherited from QGraphicsScene
    virtual void keyReleaseEvent(QKeyEvent *event) override;                            // Inherited from QGraphicsScene
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;              // Inherited from QGraphicsScene
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;             // Inherited from QGraphicsScene

    // Getters and setters
    FormMain*    getMainWindow() { return m_parent_window; }

};


//####################################################################################
//##    SceneGraphicsView
//##        A sub classed QGraphicsView so we can override events for our View Area
//############################
class SceneGraphicsView : public QGraphicsView
{
    Q_OBJECT

private:
    // Local member variables
    FormMain   *m_parent_window;
    int         m_zoom = 250;
    int         m_rotate = 0;
    bool        m_flag_key_down_spacebar = false;

public:
    // Constructor
    explicit SceneGraphicsView(QWidget *parent, FormMain *main_window) : QGraphicsView(parent = nullptr), m_parent_window (main_window) { }

    // Event Overrides, start at Qt Docs for QGraphicsView Class to find more
    virtual void enterEvent(QEvent *event) override;                                // Inherited from QWidget
    virtual void keyPressEvent(QKeyEvent *event) override;                          // Inherited from QWidget
    virtual void keyReleaseEvent(QKeyEvent *event) override;                        // Inherited from QWidget
    virtual void mouseMoveEvent(QMouseEvent *event) override;                       // Inherited from QWidget
#if QT_CONFIG(wheelevent)
    virtual void wheelEvent(QWheelEvent *event) override;                           // Inherited from QWidget
#endif

    // Getters and setters
    FormMain*    getMainWindow() { return m_parent_window; }

    // Custom handling functions
    void applyUpdatedMatrix();
    void zoomInOut(int level);

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

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual void enterEvent(QEvent *event) override;                                                            // Inherited from QWidget
    virtual void dragMoveEvent(QDragMoveEvent *event) override;                                                 // Inherited from QAbstractItemView
    virtual void dropEvent(QDropEvent *event) override;                                                         // Inherited from QTreeWidget
    virtual void selectionChanged (const QItemSelection &selected, const QItemSelection &deselected) override;  // Inherited from QTreeView
    virtual void startDrag(Qt::DropActions supportedActions) override;                                          // Inherited from QAbstractItemView

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
//##    TreeObjectInspector
//##        A sub classed QTreeWidget so we can override events for Object Inspector List
//############################
class TreeObjectInspector: public QTreeWidget
{
    Q_OBJECT

private:
    FormMain       *m_parent_window;

public:
    explicit        TreeObjectInspector(QWidget *parent, FormMain *main_window) : QTreeWidget (parent), m_parent_window (main_window) { }

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual void    enterEvent(QEvent *event) override;                                // Inherited from QWidget

    // Getters and setters
    FormMain*       getMainWindow() { return m_parent_window; }

};

//############################
//##    InspectorCategoryButton
//##        A sub classed QPushButton so we can override events for header buttons in Object Inspector List
//############################
class InspectorCategoryButton : public QPushButton
{
    Q_OBJECT

private:
    QTreeWidgetItem *m_pItem;
    QFrame          *m_child_frame;
    QRect            m_rect;

public:
    InspectorCategoryButton(const QString& a_Text, QTreeWidget* a_pParent, QTreeWidgetItem* a_pItem, QFrame* new_child);

private slots:
    void ButtonPressed();

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
    // Constructor
    explicit TreeAssetList(QWidget *parent, FormMain *main_window) : QTreeWidget (parent), m_parent_window (main_window) { }

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual void enterEvent(QEvent *event) override;                                // Inherited from QWidget

    // Getters and setters
    FormMain*    getMainWindow() { return m_parent_window; }
};


//####################################################################################
//##    TreeAdvisorList
//##        A sub classed QTreeWidget so we can override events for Advisor Window
//############################
class TreeAdvisorList: public QTreeWidget
{
    Q_OBJECT

private:
    FormMain    *m_parent_window;

public:
    // Constructor
    explicit TreeAdvisorList(QWidget *parent, FormMain *main_window) : QTreeWidget (parent), m_parent_window (main_window) { }

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual void enterEvent(QEvent *event) override;                                // Inherited from QWidget

    // Getters and setters
    FormMain*    getMainWindow() { return m_parent_window; }
};




#endif // MAINWINDOW_H









