//
//      Created by Stephens Nunnally on 1/31/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef EDITOR_HOVER_HANDLER_H
#define EDITOR_HOVER_HANDLER_H

#include <QWidget>


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

    void attach(QWidget *widget);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void signalMouseHover(QString header, QString body);
};



#endif // EDITOR_HOVER_HANDLER_H
