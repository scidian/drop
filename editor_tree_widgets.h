//
//      Created by Stephens Nunnally on 1/31/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef EDITOR_TREE_WIDGETS_H
#define EDITOR_TREE_WIDGETS_H

#include <QPushButton>
#include <QTreeWidgetItem>

#include "enums.h"

class DrProperty;


//####################################################################################
//##    WidgetHoverHandler
//##        Catches hover events for widgets on Object Inspector without needing subclassing
//############################
class WidgetHoverHandler : public QObject
{
    Q_OBJECT

public:
    WidgetHoverHandler(QObject *parent) : QObject(parent) {}
    virtual ~WidgetHoverHandler() {}

    void            attach(QWidget *widget);

    void            applyHeaderBodyProperties(QWidget *widget, DrProperty *property);
    void            applyHeaderBodyProperties(QWidget *widget, QString header, QString body);
    void            applyHeaderBodyProperties(QWidget *widget, HeaderBodyList header_body_list);

protected:
    bool            eventFilter(QObject *obj, QEvent *event);

signals:
    void            signalMouseHover(QString header, QString body);
};



//####################################################################################
//##    MouseWheelWidgetAdjustmentGuard
//##        Stops widget from stealing focus on mouse wheel
//####################################################################################
class MouseWheelWidgetAdjustmentGuard : public QObject
{
public:
    explicit        MouseWheelWidgetAdjustmentGuard(QObject *parent);

protected:
    bool            eventFilter(QObject* obj, QEvent* event) override;
};



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





#endif // EDITOR_TREE_WIDGETS_H









