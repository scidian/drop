//
//      Created by Stephens Nunnally on 01/08/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//      Customized QWidgets
//
//
#ifndef WIDGETS_H
#define WIDGETS_H

#include <QSlider>
#include <QSpinBox>


//####################################################################################
//##    Slider
//##        Allows us to use Style Sheets with QSlider and still have Tick Marks painted
//############################
class DrQSlider : public QSlider
{
    Q_OBJECT

private:
    QColor          m_tick_color = Qt::white;
public:
    DrQSlider(QWidget *parent = nullptr) : QSlider(parent) { }
    virtual ~DrQSlider() override { }

    // Event Overrides
    virtual void    paintEvent(QPaintEvent *event) override;

    // Getters / Setters
    void            setTickColor(QColor color) {    m_tick_color =  color; }

public slots:
    void    updateValue(int value);
};


//####################################################################################
//##    Spin Slot
//##        Allows us to attach to setValue slot without emitting signals
//############################
class DrQSpinSlot : public QSpinBox
{
    Q_OBJECT

public:
    DrQSpinSlot(QWidget *parent = nullptr) : QSpinBox(parent) { }
    virtual ~DrQSpinSlot() override { }

public slots:
    void    updateValue(int value);
};





#endif // WIDGETS_H

















