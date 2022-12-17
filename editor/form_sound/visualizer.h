//
//      Created by Stephens Nunnally on 5/28/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QFrame>

#include "3rd_party/soloud/soloud.h"
#include "project/enums_entity_types.h"


//####################################################################################
//##    VisualFrame
//##        Allows us to draw visualizer for sound played by SoLoud
//############################
class VisualFrame : public QFrame
{
    Q_OBJECT

private:
    bool        m_show_border       { false };              // Should draw a border around frame?
    bool        m_scale_size        { false };              // Set to true to have visualizer resize itself based on Dr::Scale()
    double      m_scale_width       { 100 };                // Target Dr::Scale width when m_scale_size is true
    double      m_scale_height      { 25 };                 // Target Dr::Scale height when m_scale_size is true


public:
    VisualFrame(QWidget *parent = nullptr);
    virtual ~VisualFrame() override;

    // Event Overrides
    virtual void    paintEvent(QPaintEvent *event) override;

    // Setters
    void            showBorder(bool border_on) { m_show_border = border_on; }
    void            turnOnAutoScale(double width, double height);

};

#endif // VISUALIZER_H











