//
//      Created by Stephens Nunnally on 5/26/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WIDGETS_TREES_H
#define WIDGETS_TREES_H

#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>


//####################################################################################
//##    TreeCategoryButton
//##        A sub classed QPushButton so we can override events for header buttons in Tree Lists
//############################
class TreeCategoryButton : public QPushButton
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
    TreeCategoryButton(const QString &text, QColor m_text_color, QColor m_disabled_color, QWidget *parent, QTreeWidgetItem *parent_tree_item);
    virtual ~TreeCategoryButton();

    // Event Calls
    virtual void    paintEvent(QPaintEvent *event) override;

public slots:
    void            buttonPressed();
};


#endif // WIDGETS_TREES_H













