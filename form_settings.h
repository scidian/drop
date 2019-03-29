//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_SETTINGS_H
#define FORM_SETTINGS_H

#include <QLabel>
#include <QWidget>

class DrProject;
class MouseLabel;

class FormSettings : public QWidget
{
private:
    DrProject   *m_project;                         // Pointer to the open project
    QWidget     *m_inner_widget;                    // Container widget, allows for a double form border

    MouseLabel  *m_label;


public:
    explicit FormSettings(DrProject *project, QWidget *parent = nullptr);
    virtual ~FormSettings() override;

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;

};




class MouseLabel : public QLabel
{
    Q_OBJECT

private:
    QColor      m_color = Qt::black;
    QSize       m_size;

public:
    explicit MouseLabel(QWidget *parent = nullptr);
    virtual ~MouseLabel() override;

    // Event Overrides
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    void         setColor(QColor color);
};





#endif // FORM_SETTINGS_H






















