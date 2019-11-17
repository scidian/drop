//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Assests List
//
//
#ifndef EDITOR_TREE_ASSETS_H
#define EDITOR_TREE_ASSETS_H

#include <QLabel>
#include <QMutex>
#include <QVBoxLayout>
#include <QTimer>
#include <QTreeWidget>

#include "enums.h"

// Forward declarations
class DrProject;
class DrProperty;
class DrSettings;
class DrQLayoutFlow;

class DrQPushButtonCategory;
class IEditorRelay;
class DrFilterHoverHandler;

// Class constants
constexpr int   c_asset_size_left =  3;                         // Size policy width of left column
constexpr int   c_asset_size_right = 5;                         // Size policy width of right column


//####################################################################################
//##    TreeAssetList
//##        A Tree List to show assests available to current project
//############################
class TreeAssets: public QTreeWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject            *m_project;                            // Pointer to currently loaded project
    IEditorRelay         *m_editor_relay;                       // Pointer to IEditorRelay class of parent form

    // Local Variables
    DrFilterHoverHandler *m_filter_hover;                       // Pointer to an event filter hover handler

    long                  m_selected_key = c_no_key;            // Unique Key of last clicked on Asset

    std::map <DrAssetType, DrQLayoutFlow*>  m_grid_layouts;     // List of the layouts so that we can sort without rebuilding
    QList<QFrame*>                          m_asset_frames;     // List of the single row frames that contain name and pixmap labels

    QWidget              *m_search_widget;
    QVBoxLayout          *m_search_layout;
    QLineEdit            *m_search_bar;                         // Search bar at bottom of asset dock

public:
    // Constructor
    explicit        TreeAssets(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);

    // Event Overrides
    virtual void    focusInEvent(QFocusEvent *event) override;
    virtual void    focusOutEvent(QFocusEvent *event) override;
    virtual void    keyPressEvent(QKeyEvent *event) override;

    // Tree Building Functions
    void                    buildAssetTree(QString search_text = "");
    DrQPushButtonCategory*  createCategoryButton(QTreeWidgetItem *item, DrAssetType asset_type);
    void                    ensureSelectedKeyVisible();
    void                    expandCollapseComponents();
    QList<QTreeWidgetItem*> getListOfTopLevelItems();
    void                    removeAsset(long asset_key);

    // Function Calls
    IEditorRelay*   getRelay() { return m_editor_relay; }
    void            forceUpdateOfItemSizes() { this->scheduleDelayedItemsLayout(); }                    // !!!!! #NOTE: Forces updating of QTreeWidget, yay!
    void            updateAssetList(QList<DrSettings*> changed_items, QList<long> property_keys);

    // Getters / Setters
    DrFilterHoverHandler*   getHoverHandler();
    void                    setHoverHandler(DrFilterHoverHandler* hover_handler) { m_filter_hover = hover_handler; }
    long                    getSelectedKey() { return m_selected_key; }
    void                    setSelectedKey(long key);

private slots:
    void            searchTextChanged(QString new_text);
    void            setAdvisorInfo(QString header, QString body);

public slots:
    void            handleCollapsed(QTreeWidgetItem *item);
    void            handleExpanded(QTreeWidgetItem *item);

};


//####################################################################################
//##    DrFilterAssetMouseHandler
//##        Event filter handler for asset items
//####################################################################################
class DrFilterAssetMouseHandler : public QObject
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    IEditorRelay      *m_editor_relay;                  // Pointer to IEditorRelay class of parent form

    // Local Variables - Following variables are used for Asset Name scrolling on mouse over
    QMutex             scrolling_mutex { QMutex::NonRecursive };
    QTimer            *m_timer;                         // Timer used to start scrolling
    bool               m_flag_scrolling = false;        // Flag to track when scrolling is active

    QLabel            *m_label_to_scroll;
    int                m_position;                      // Position of label
    QString            m_scroll_text;                   // Text we use for scrolling
    int                m_scroll_width;                  // Width of total string as drawn
    QRect              m_starting_rect;                 // Label starting geometry
    QString            m_starting_text;                 // Label starting text

public:
    explicit        DrFilterAssetMouseHandler(QObject *parent, IEditorRelay *editor_relay);

    // Event Overrides
    bool            eventFilter(QObject* object, QEvent* event) override;

    // Functions
    void            handleScroll(QLabel *label, bool first_time);
    void            startDragAndDrop(QLabel *label_pixmap, long asset_key);

public slots:
    void            startScroll();
};





#endif // EDITOR_TREE_ASSETS_H












