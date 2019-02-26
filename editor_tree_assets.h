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
#include <QTreeWidget>

class DrProject;
class DrProperty;
class DrSettings;

class InterfaceRelay;
class WidgetHoverHandler;

// Class constants
const int   c_asset_size_left =  3;                 // Size policy width of left column
const int   c_asset_size_right = 5;                 // Size policy width of right column


//####################################################################################
//##    TreeAssetList
//##        A Tree List to show assests available to current project
//############################
class TreeAssets: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject           *m_project;                 // Pointer to currently loaded project
    InterfaceRelay      *m_relay;                   // Pointer to InterfaceRelay class of parent form

    QList<QFrame*>       m_asset_frames;            // List of the single row frames that contain name and pixmap labels

    WidgetHoverHandler  *m_widget_hover;            // Pointer to a widget hover handler

public:
    // Constructor
    explicit        TreeAssets(QWidget *parent, DrProject *project, InterfaceRelay *relay);

    // Function Calls
    void            buildAssetTree();
    InterfaceRelay* getRelay() { return m_relay; }
    void            updateAssetList(QList<DrSettings*> changed_items, QList<long> property_keys);

    // Property Builders
    void            attachToHoverHandler(QWidget *widget, DrProperty *property);
    void            attachToHoverHandler(QWidget *widget, QString header, QString body);

private slots:
    void            setAdvisorInfo(QString header, QString body);

};


//####################################################################################
//##    AssetMouseHandler
//##        Event filter handler for asset items
//####################################################################################
class AssetMouseHandler : public QObject
{
private:
    InterfaceRelay      *m_relay;                    // Pointer to InterfaceRelay class of parent form

public:
    explicit        AssetMouseHandler(QObject *parent, InterfaceRelay *relay);

protected:
    bool            eventFilter(QObject* obj, QEvent* event) override;
};



#endif // EDITOR_TREE_ASSETS_H


















