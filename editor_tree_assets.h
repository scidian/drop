//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A sub classed QTreeWidget so we can override events for Assests List
//
//
#ifndef EDITOR_TREE_ASSETS_H
#define EDITOR_TREE_ASSETS_H

#include <QtWidgets>

class DrProject;
class DrProperty;
class InterfaceRelay;

class WidgetHoverHandler;

// Class constants
const int   ASSET_SIZE_LEFT =  3;                             // Size policy width of left column
const int   ASSET_SIZE_RIGHT = 5;                             // Size policy width of right column


//####################################################################################
//##    TreeAssetList
//##        A Tree List to show assests available to current project
//############################
class TreeAssetList: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject           *m_project;                  // Pointer to currently loaded project
    InterfaceRelay      *m_relay;                    // Pointer to InterfaceRelay class of parent form

    WidgetHoverHandler  *m_widget_hover;            // Pointer to a widget hover handler

public:
    // Constructor
    explicit        TreeAssetList(QWidget *parent, DrProject *project, InterfaceRelay *relay);

    // Function Calls
    void            buildAssetList();
    InterfaceRelay* getRelay() { return m_relay; }

    // Property Builders
    void            applyHeaderBodyProperties(QWidget *widget, DrProperty *property);
    void            applyHeaderBodyProperties(QWidget *widget, QString header, QString body);

private slots:
    void            setAdvisorInfo(QString header, QString body);

};



//####################################################################################
//##    AssetCategoryButton
//##        A sub classed QPushButton so we can override events for header buttons in Asset Inspector List
//############################
class AssetCategoryButton : public QPushButton
{
    Q_OBJECT

private:
    TreeAssetList       *m_parent_tree;
    QTreeWidgetItem     *m_parent_item;
    bool                 m_is_shrunk = false;
    QString              m_header, m_body;

public:
    AssetCategoryButton(const QString &text, TreeAssetList *parent_tree, QTreeWidgetItem *parent_item);

    // Events
    virtual void    enterEvent(QEvent *event) override;                                // Inherited from QWidget

    // Getters and Setters
    void            setAdvisorHeaderText(QString header) { m_header = header; }
    void            setAdvisorBodyText(QString body) { m_body = body; }

private slots:
    void buttonPressed();
};



#endif // EDITOR_TREE_ASSETS_H




