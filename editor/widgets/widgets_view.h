//
//      Created by Stephens Nunnally on 5/22/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef WIDGETS_EDITOR_H
#define WIDGETS_EDITOR_H

#include <QButtonGroup>
#include <QDial>
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

// Local Enums
enum class Dial_Style {
    Knotch_Circle,
    Knotch_Line,
};


//####################################################################################
//##    ViewDial
//##        A sub classed QDial so we can override paint event for Dial
//############################
class ViewDial : public QDial
{
    Q_OBJECT

    Q_PROPERTY(int dialSize READ getDialSize WRITE setDialSize)

private:
    Dial_Style  m_dial_style        { Dial_Style::Knotch_Line };
    int         m_dial_size         { 20 };
    bool        m_show_ticks        { true };

public:
    // Constructor
    ViewDial(QWidget *parent = nullptr, Dial_Style style = Dial_Style::Knotch_Line, bool show_ticks = false, int dial_size = 20);
    virtual ~ViewDial() override;

    // Event Overrides
    virtual void    paintEvent(QPaintEvent *event) override;

    // Getters / Setters
    int             getDialSize() const;
    Dial_Style      getDialStyle()                  { return m_dial_style; }
    bool            getShowTicks()                  { return m_show_ticks; }
    void            setDialSize(int size);
    void            setDialStyle(Dial_Style style)  { m_dial_style = style; }
    void            setShowTicks(bool show)         { m_show_ticks = show; }
};


//####################################################################################
//##    ViewRubberBand
//##        A sub classed QRubberBand so we can override paint event for rubber band
//############################
class ViewRubberBand : public QRubberBand
{
public:
    // Constructor
    ViewRubberBand(Shape shape, QWidget *parent) : QRubberBand(shape, parent) { }

    // Event overrides
    virtual void    paintEvent(QPaintEvent *event) override;
};


//####################################################################################
//##    ViewToolTip
//##        A parentless widget to be used as a custom tooltip
//############################
class ViewToolTip : public QWidget
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
    ViewToolTip(QWidget *parent = nullptr);

    // Event overrides
    virtual void    paintEvent(QPaintEvent *event) override;

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




















