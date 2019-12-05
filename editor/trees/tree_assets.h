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
#include <QPushButton>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "enums.h"

// Forward declarations
class DrProject;
class DrProperty;
class DrSettings;

class AssetFlowLayout;
class AssetCategoryButton;
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
    DrProject            *m_project;                                // Pointer to currently loaded project
    IEditorRelay         *m_editor_relay;                           // Pointer to IEditorRelay class of parent form

    // Local Variables
    DrFilterHoverHandler *m_filter_hover;                           // Pointer to an event filter hover handler

    long                  m_selected_key = c_no_key;                // Unique Key of last clicked on Asset

    std::map <Asset_Category, AssetFlowLayout*> m_grid_layouts;     // List of the layouts so that we can sort without rebuilding
    QList<QFrame*>                              m_asset_frames;     // List of the single row frames that contain name and pixmap labels

    QList<DrType>         m_show_types { };                         // Types to show for this TreeAssets

    QWidget              *m_search_widget;
    QVBoxLayout          *m_search_layout;
    QLineEdit            *m_search_bar;                             // Search bar at bottom of asset dock

public:
    // Constructor
    explicit        TreeAssets(QWidget *parent, DrProject *project, IEditorRelay *editor_relay, QList<DrType> show_types);

    // Event Overrides
    virtual void    focusInEvent(QFocusEvent *event) override;
    virtual void    focusOutEvent(QFocusEvent *event) override;
    virtual void    keyPressEvent(QKeyEvent *event) override;

    // Tree Building Functions
    void                    buildAssetTree(QString search_text = "");
    AssetCategoryButton*    createCategoryButton(QTreeWidgetItem *item, Asset_Category asset_type);
    void                    ensureSelectedKeyVisible();
    void                    expandCollapseComponents();
    QList<QTreeWidgetItem*> getListOfTopLevelItems();
    void                    removeAsset(long entity_key);

    // Function Calls
    void            forceUpdateOfItemSizes() { this->scheduleDelayedItemsLayout(); }                    // !!!!! #NOTE: Forces updating of QTreeWidget, yay!
    void            updateAssetList(QList<DrSettings*> changed_entities, QList<long> property_keys);

    // Getters / Setters
    IEditorRelay*           getEditorRelay() { return m_editor_relay; }
    DrProject*              getParentProject() { return m_project; }

    DrFilterHoverHandler*   getHoverHandler();
    void                    setHoverHandler(DrFilterHoverHandler* hover_handler) { m_filter_hover = hover_handler; }
    long                    getSelectedKey() { return m_selected_key; }
    void                    setSelectedKey(long key);
    QList<DrType>&          getShowTypes() { return m_show_types; }
    void                    setShowTypes(QList<DrType> types) { m_show_types = types; }

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



//####################################################################################
//##    AssetCategoryButton
//##        A sub classed QPushButton so we can override events for header buttons in Tree Lists
//############################
class AssetCategoryButton : public QPushButton
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    QTreeWidget          m_parent_tree;
    QTreeWidgetItem     *m_parent_item;

    // Local Variables
    QColor               m_text_color;
    QColor               m_disabled_color;

public:
    // Constructor
    AssetCategoryButton(const QString &text, QColor m_text_color, QColor m_disabled_color, QWidget *parent, QTreeWidgetItem *parent_tree_item);
    virtual ~AssetCategoryButton();

    // Event Calls
    void            paintEvent(QPaintEvent *event);

private slots:
    void            buttonPressed();
};



//####################################################################################
//##    AssetFlowLayout
//##        A changing GridLayout that adjusts to the size of the parent widget
//############################
class AssetFlowLayout : public QLayout
{
private:
    // External Borrowed Pointers
    DrProject           *m_project;                         // Pointer to currently loaded project

    // Local Variables
    QList<QLayoutItem*>  item_list;                         // Holds a list of items inserted into the layout, so we can delete them in the destructor
    int                  m_hSpace;
    int                  m_vSpace;
    QSize                m_last_size;

    int                  m_row_width;                       // Holds the largest width (in item count) of the layout

public:
    // Constructors / Destructor
    explicit AssetFlowLayout(DrProject *project, QWidget *parent, int margin_left = -1, int margin_right = -1,
                                                                  int margin_top = -1, int margin_bottom = -1,
                                                                  int hSpacing = -1, int vSpacing = -1);
    explicit AssetFlowLayout(DrProject *project,  int margin_left = -1, int margin_right = -1,
                                                  int margin_top = -1, int margin_bottom = -1,
                                                  int hSpacing = -1, int vSpacing = -1);
    ~AssetFlowLayout() override;

    // Main Functions
    int     doLayout(const QRect &rect, int &row_width, bool testOnly) const;
    int     smartSpacing(QStyle::PixelMetric pm) const;
    void    sortItems();

    // QLayout Overrides
    void                addItem(QLayoutItem *item) override;
    Qt::Orientations    expandingDirections() const override;
    QLayoutItem*        itemAt(int index) const override;
    void                setGeometry(const QRect &rect) override;
    QLayoutItem*        takeAt(int index) override;
    int                 count() const override;
    bool                hasHeightForWidth() const override;
    int                 heightForWidth(int) const override;
    QSize               minimumSize() const override;
    QSize               sizeHint() const override;

    // Size Functions
    int     horizontalSpacing() const;
    int     verticalSpacing() const;
    QSize   lastSize() { return m_last_size; }
    int     rowWidth() { return m_row_width; }

};




#endif // EDITOR_TREE_ASSETS_H












