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
class InterfaceRelay;


//####################################################################################
//##    TreeAssetList
//##        A Tree List to show assests available to current project
//############################
class TreeAssetList: public QTreeWidget
{
    Q_OBJECT

private:
    DrProject      *m_project;                  // Pointer to currently loaded project
    InterfaceRelay *m_relay;                    // Pointer to InterfaceRelay class of parent form

public:
    // Constructor
    explicit        TreeAssetList(QWidget *parent, DrProject *project, InterfaceRelay *relay) :
                                  QTreeWidget (parent), m_project(project), m_relay(relay) { }

    // Event Overrides, start at Qt Docs for QTreeWidget Class to find more
    virtual void    enterEvent(QEvent *event) override;                                // Inherited from QWidget

};

#endif // EDITOR_TREE_ASSETS_H




