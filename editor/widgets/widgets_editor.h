//
//      Created by Stephens Nunnally on 5/22/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WIDGETS_EDITOR_H
#define WIDGETS_EDITOR_H

#include <QButtonGroup>
#include <QElapsedTimer>
#include <QLabel>
#include <QGraphicsView>
#include <QMap>
#include <QPushButton>
#include <QRubberBand>
#include <QToolButton>

#include "core/types/dr_pointf.h"
#include "editor/constants_advisor_info.h"
#include "editor/enums_editor.h"
#include "project/enums_entity_types.h"


//####################################################################################
//##    EditorViewRubberBand
//##        A sub classed QRubberBand so we can override paint event for rubber band
//############################
class EditorViewRubberBand : public QRubberBand
{
public:
    // Constructor
    EditorViewRubberBand(Shape shape, QWidget *parent) : QRubberBand (shape, parent) { }

    // Event overrides
    virtual void    paintEvent(QPaintEvent *) override;
};


//####################################################################################
//##    EditorViewToolTip
//##        A parentless widget to be used as a custom tooltip
//############################
class EditorViewToolTip : public QWidget
{
private:
    View_Mode   m_tip_type = View_Mode::None;           // Which type of tool tip to show
    QPoint      m_offset;                               // Stores how much to offset the current tooltip from mouse position
    double      m_angle = 0;                            // Stores angle to show in tooltip
    double      m_x = 0;                                // Stores x value of resizing / moving
    double      m_y = 0;                                // Stores y value of resizing / moving
    int         m_int = 0;                              // Stores zoom scale
    int         m_x_radius = 100;
    int         m_y_radius = 100;

public:
    // Constructor
    EditorViewToolTip(QWidget *parent = nullptr);

    // Event overrides
    virtual void    paintEvent(QPaintEvent *) override;

    // Functions
    void            drawText(QPainter &painter, int left_offset = 0, int top_offset = 0);
    void            startToolTip(View_Mode type, QPoint mouse_position, QVariant data);
    void            stopToolTip();
    void            updateToolTipPosition(QPoint mouse_position);
    void            updateToolTipData(QVariant data);

    // Getters and Setters
    QPoint          getOffset()  { return m_offset;   }
    View_Mode       getTipType() { return m_tip_type; }
    int             getXRadius() { return m_x_radius; }
    int             getYRadius() { return m_y_radius; }

};


#endif // WIDGETS_EDITOR_H




















