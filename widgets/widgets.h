//
//      Created by Stephens Nunnally on 3/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WIDGETS_H
#define WIDGETS_H

#include <QMessageBox>
#include <QPushButton>
#include <QTreeWidget>


//####################################################################################
//##    DrQPushButtonCategory
//##        A sub classed QPushButton so we can override events for header buttons in Tree Lists
//############################
class DrQPushButtonCategory : public QPushButton
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    QTreeWidget          m_parent_tree;
    QTreeWidgetItem     *m_parent_item;

    // Local Variables
    bool                 m_is_shrunk = false;
    QColor               m_text_color;
    QColor               m_disabled_color;

public:
    // Constructor
    DrQPushButtonCategory(const QString &text, QColor m_text_color, QColor m_disabled_color, QWidget *parent, QTreeWidgetItem *parent_tree_item);

    // Event Calls
    void            paintEvent(QPaintEvent *event);

private slots:
    void            buttonPressed();
};



#endif // WIDGETS_H








