//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_SETTINGS_H
#define FORM_SETTINGS_H

#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QScreen>
#include <QWidget>

class DrProject;

class FormSettings : public QWidget
{
private:
    DrProject   *m_project;                         // Pointer to the open project
    QWidget     *m_inner_widget;                    // Container widget, allows for a double form border


    // TEMP for finding color
    QLabel     *m_label;
    QColor      m_color = Qt::black;
    QPixmap     m_capture;
    QImage      m_image;
    QScreen    *m_screen;
    QSize       m_start_size;
    QString     m_style;

public:
    void        setColor(QColor color);




public:
    FormSettings(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    virtual void resizeEvent(QResizeEvent *event) override;

};


#endif // FORM_SETTINGS_H
