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


//#######################################################################
//##
//##    FormMain - Main editor window
//##
//#######################################################################
class TreeSceneView;                        // Necessary forward declaration

class FormMain : public QMainWindow
{
    Q_OBJECT

public:
    // Locals that need to be SAVED / LOADED from each project
    DrProject      *project;                                            // Holds whatever the current open game project is
    long            current_world;                                      // Tracks which world to show in the scene viewer

    Globals        *globals;                                            // Holds project globals

    QLabel      *label_1;
    QLabel      *label_2;
    QLabel      *label_3;
    QLabel      *label_object;
    QLabel      *label_object_2;
    QLabel      *label_object_3;

private:
    TreeSceneView *treeScene;

    QMenu         *menuDrop;
    QMenuBar      *menuBar;

    QWidget       *widgetAssests;
    QWidget       *widgetBottom;
    QWidget       *widgetCentral;
    QWidget       *widgetInner;
    QWidget       *widgetInspector;
    QWidget       *widgetToolbar;

    QHBoxLayout   *horizontalLayout;
    QVBoxLayout   *verticalLayout;

    QSplitter     *splitterHorizontal;
    QSplitter     *splitterVertical;

    QDockWidget   *assets;
    QDockWidget   *inspector;
    QDockWidget   *toolbar;

    QScrollArea   *areaBottom;
    QTableWidget  *tableWidget;

    QPushButton   *buttonAtlas;
    QPushButton   *buttonFonts;
    QPushButton   *buttonPlay;
    QPushButton   *buttonSettings;
    QPushButton   *buttonWorlds;



private slots:
    void on_tableWidget_itemClicked(QTableWidgetItem *item);            // Handles click on Object Inspector table

public:
    // Constructor and Destructor
    explicit FormMain(QWidget *parent = nullptr, Globals *the_globals = nullptr);
    ~FormMain();

    // Helper calls
    void            listSelectionChanged(QList<QTreeWidgetItem*> item_list);

    void            applyColoring(Color_Scheme new_color);
    void            applyPalette(Color_Scheme new_color);
    void            buildWindow();
    void            buildObjectInspector();

};


//#######################################################################
//##
//##    MyTreeView - A sub classed QTreeWidget so we can override events
//##
//#######################################################################
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




#endif // MAINWINDOW_H







