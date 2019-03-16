//
//      Created by Stephens Nunnally on 3/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WIDGETS_H
#define WIDGETS_H

#include <QPushButton>
#include <QTreeWidget>


//####################################################################################
//##    CategoryButton
//##        A sub classed QPushButton so we can override events for header buttons in Tree Lists
//############################
class CategoryButton : public QPushButton
{
    Q_OBJECT

private:
    QTreeWidget          m_parent_tree;
    QTreeWidgetItem     *m_parent_item;
    bool                 m_is_shrunk = false;
    QColor               m_color;

public:
    CategoryButton(const QString &text, QColor color, QWidget *parent, QTreeWidgetItem *parent_tree_item);

    void            paintEvent(QPaintEvent *event);

private slots:
    void            buttonPressed();
};



#endif // WIDGETS_H








