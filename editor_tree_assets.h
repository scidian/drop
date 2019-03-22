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
#include <QVBoxLayout>
#include <QTime>
#include <QTreeWidget>

class DrProject;
class DrProperty;
class DrSettings;

class CategoryButton;
class IEditorRelay;
class WidgetHoverHandler;

// Class constants
constexpr int   c_asset_size_left =  3;                 // Size policy width of left column
constexpr int   c_asset_size_right = 5;                 // Size policy width of right column


//####################################################################################
//##    TreeAssetList
//##        A Tree List to show assests available to current project
//############################
class TreeAssets: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject            *m_project;                // Pointer to currently loaded project
    IEditorRelay         *m_editor_relay;           // Pointer to IEditorRelay class of parent form
    WidgetHoverHandler   *m_widget_hover;           // Pointer to a widget hover handler

    QList<QFrame*>        m_asset_frames;           // List of the single row frames that contain name and pixmap labels

    QWidget              *m_search_widget;
    QVBoxLayout          *m_search_layout;
    QLineEdit            *m_search_bar;             // Search bar at bottom of asset dock

public:
    // Constructor
    explicit        TreeAssets(QWidget *parent, DrProject *project, IEditorRelay *editor_relay);

    // Tree Building Functions
    void            addAssetsToCategory(QTreeWidgetItem *tree_item, QFrame *asset_frame);
    void            buildAssetTree(QString search_text = "");
    CategoryButton* initializeCatergoryButton(QTreeWidgetItem *tree_item, QString name);

    // Function Calls
    IEditorRelay*   getRelay() { return m_editor_relay; }
    void            forceUpdateOfItemSizes() { this->scheduleDelayedItemsLayout(); }
    void            updateAssetList(QList<DrSettings*> changed_items, QList<long> property_keys);

private slots:
    void            searchTextChanged(QString new_text);
    void            setAdvisorInfo(QString header, QString body);

};


//####################################################################################
//##    AssetMouseHandler
//##        Event filter handler for asset items
//####################################################################################
class AssetMouseHandler : public QObject
{
private:
    IEditorRelay      *m_editor_relay;                    // Pointer to IEditorRelay class of parent form
    QTime              m_scroll_timer;
    bool               m_flag_scrolling;
    int                m_position;
    int                m_pause_time;
    int                m_width;

public:
    explicit        AssetMouseHandler(QObject *parent, IEditorRelay *editor_relay);

    // Event Overrides
    bool            eventFilter(QObject* obj, QEvent* event) override;

    // Functions
    void            handleScroll(QLabel *label, QString asset_name);
};





#endif // EDITOR_TREE_ASSETS_H












