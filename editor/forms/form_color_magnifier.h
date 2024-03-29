//
//      Created by Stephens Nunnally on 3/29/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_COLOR_MAGNIFYING_GLASS_H
#define FORM_COLOR_MAGNIFYING_GLASS_H

#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QScreen>
#include <QWidget>

#include "core/types/dr_color.h"


//####################################################################################
//##    FormColorMagnifier
//##        Magnifying glass that pops up to pick a color off the screen
//############################
class FormColorMagnifier : public QWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    QWidget    *m_parent;                       // Parent widget that wants the color

    // Local Variables
    QLabel     *m_label;                        // Draws the magnifier on the form
    QColor      m_color = Qt::black;            // Holds current color
    QPixmap     m_capture;                      // Holds a QPixmap screen shot of the current screen
    QImage      m_image;                        // Holds a QImage of the screen shot
    QScreen    *m_screen;                       // Pointer to the last screen we were over

    int         m_width;                        // Width of magnifier
    int         m_height;                       // Height of magnifier
    double      m_zoom;                         // Level of zoom of magnifier

public:
    // Constructor / Destructor
    explicit FormColorMagnifier(QWidget *parent, QPoint mouse_pos, int width, int height, double zoom);
    virtual ~FormColorMagnifier() override;

    // Event Overrides
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    // Functions
    QPixmap     drawCursor(QPoint screen_pos, int width, int height, double zoom);
    void        updateColor(QPoint mouse_pos);

public slots:
    void        grabScreen(QScreen *screen);

signals:
    void        colorGrabbed(QWidget *parent, DrColor m_color);

};



#endif // FORM_COLOR_MAGNIFYING_GLASS_H



