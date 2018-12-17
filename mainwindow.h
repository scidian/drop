//
//
//      MainWindow - Class that holds our main form window
//
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>

#include <QDropEvent>
#include <QDragMoveEvent>

#include "01_project.h"

// Declare UI, handled from external Designer auto generated header file
namespace Ui {
class MainWindow;
}

//#######################################################################
//##
//##    MainWindow - Main editor window
//##
//#######################################################################
class MyTreeView;                        // Necessary forward declaration

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;                                                 // Holds all the widgets from form design for use in main window
    MyTreeView     *treeScene;

    // Locals that need to be SAVED / LOADED from each project
    DrProject      *project;                                            // Holds whatever the current open game project is
    long            current_world;                                      // Tracks which world to show in the scene viewer

private slots:
    void on_tableWidget_itemClicked(QTableWidgetItem *item);            // Handles click on Object Inspector table

public:
    // Constructor and Destructor
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Helper calls
    DrProject*      getProject() { return project; }
    Ui::MainWindow* getUi() { return ui; }
    MyTreeView*     getSceneListWidget();
    void            setSceneListWidget(MyTreeView *new_tree_scene);
    void            listSelectionChanged(QList<QTreeWidgetItem*> item_list);
    void            buildObjectInspector();


};


//#######################################################################
//##
//##    MyTreeView - A sub classed QTreeWidget so we can override events
//##
//#######################################################################
class MyTreeView: public QTreeWidget
{
    Q_OBJECT
private:
    MainWindow  *m_parent_window;
    long         m_selected_key = 0;                                    // Holds first selected item in QTreeWidget (treeScene)
    bool         m_is_dragging;                                         // Set to true when we are dragging
    bool         m_can_drop;
    int          m_mouse_x;
    int          m_mouse_y;

public:
    explicit MyTreeView(QWidget *parent, MainWindow *main_window) : QTreeWidget (parent), m_parent_window (main_window) { }

    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
    virtual void selectionChanged (const QItemSelection &selected, const QItemSelection &deselected) override;
    virtual void startDrag(Qt::DropActions supportedActions) override;

    MainWindow*  getMainWindow() { return m_parent_window; }
    long         getSelectedKey() { return m_selected_key; }
    void         setSelectedKey(long new_key) { m_selected_key = new_key; }

    bool         getIsDragging() { return m_is_dragging; }
    void         setIsDragging(bool new_state) { m_is_dragging = new_state; }

    bool         canWeDrop() { return m_can_drop; }
    int          getMouseX() { return m_mouse_x; }
    int          getMouseY() { return m_mouse_y; }
};




#endif // MAINWINDOW_H







