//
//
//      MainWindow - Class that holds our main form window
//
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>                    // Header includes all QtWidget items

#include <QDropEvent>
#include <QDragMoveEvent>

#include "01_project.h"

//#######################################################################
//##
//##    MainWindow - Main editor window
//##
//#######################################################################
class MyTreeView;                        // Necessary forward declaration

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QLabel *label_1;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_object;
    QLabel *label_object_2;
    QLabel *label_object_3;

private:
    MyTreeView     *treeScene;

    QWidget *widgetCentral;
        QVBoxLayout *verticalLayout;
        QSplitter *splitterVertical;
        QWidget *widgetInner;
        QHBoxLayout *horizontalLayout;
        QSplitter *splitterHorizontal;
        QScrollArea *areaBottom;
        QWidget *widgetBottom;
        QMenuBar *menuBar;
        QMenu *menuDrop;
        QDockWidget *assets;
        QWidget *widgetAssests;
        QDockWidget *inspector;
        QWidget *widgetInspector;
        QTableWidget *tableWidget;
        QDockWidget *toolbar;
        QWidget *widgetToolbar;
        QPushButton *buttonWorlds;
        QPushButton *buttonPlay;
        QPushButton *buttonSettings;
        QPushButton *buttonAtlas;
        QPushButton *buttonFonts;


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

    void            listSelectionChanged(QList<QTreeWidgetItem*> item_list);

    void            applyMainWindowPalette(Color_Scheme new_color);
    void            buildMainWindow();
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







