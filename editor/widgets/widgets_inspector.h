//
//      Created by Stephens Nunnally on 01/08/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//      Customized QWidgets
//
//
#ifndef WIDGETS_H
#define WIDGETS_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QSpinBox>


//####################################################################################
//##    CheckBox
//##        Allows us to draw custom check mark
//############################
class InspectorCheckBox : public QCheckBox
{
    Q_OBJECT
private:
    int             m_draw_left = 5;
    int             m_draw_top =  1;

public:
    InspectorCheckBox(QWidget *parent = nullptr) : QCheckBox (parent) { }
    virtual ~InspectorCheckBox() override { }

    int             getDrawLeft() { return m_draw_left; }
    int             getDrawTop() { return m_draw_top; }
    void            setDrawLeft(int left) { m_draw_left = left; }
    void            setDrawTop(int top) { m_draw_top = top; }

protected:
    virtual void    paintEvent(QPaintEvent *) override;
};


//####################################################################################
//##    DropDownComboBox
//##        Allows us to move combo listview underneath control to appear as a drop down list
//############################
class InspectorComboBoxDropDown : public QComboBox
{
    Q_OBJECT

public:
    InspectorComboBoxDropDown(QWidget *parent = nullptr) : QComboBox(parent) { }
    virtual ~InspectorComboBoxDropDown() override { }

protected:
    virtual void    showPopup() override;
};


//####################################################################################
//##    Slider
//##        Allows us to use Style Sheets with QSlider and still have Tick Marks painted
//############################
class InspectorSlider : public QSlider
{
    Q_OBJECT

private:
    QColor          m_tick_color = Qt::white;
public:
    InspectorSlider(QWidget *parent = nullptr) : QSlider(parent) { }
    virtual ~InspectorSlider() override { }

    // Event Overrides
    virtual void    paintEvent(QPaintEvent *event) override;

    // Getters / Setters
    void            setTickColor(QColor color) {    m_tick_color =  color; }

public slots:
    void            updateValue(int value);
};


//####################################################################################
//##    TripleSpinBox
//##        Allows us to control number of decimals being shown in spin box
//##        #NOTE: As name implies usually aiming for 3 decimals, but can be set by calling InspectorTripleSpinBox->setDecimals(xxx);
//############################
class InspectorTripleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    InspectorTripleSpinBox(QWidget *parent = nullptr) : QDoubleSpinBox(parent) { }
    virtual ~InspectorTripleSpinBox() override { }

protected:
    virtual QString textFromValue(double value) const override;
};


//####################################################################################
//##    Spin Slot Integer
//##        Allows us to attach to setValue slot without emitting signals
//############################
class InspectorSpinSlot : public QSpinBox
{
    Q_OBJECT
private:
    bool            m_show_menu_step        { true };

public:
    InspectorSpinSlot(QWidget *parent = nullptr) : QSpinBox(parent) { }
    virtual ~InspectorSpinSlot() override { }

    // Event Overrides
    virtual void    contextMenuEvent(QContextMenuEvent *event) override;

    // Getters / Setters
    QLineEdit      *getLineEdit() { return this->lineEdit(); }
    void            setShowMenuStep(bool show) { m_show_menu_step = show; }

public slots:
    void            updateValue(int value);
};


//####################################################################################
//##    Spin Slot Double
//##        Allows us to attach to setValue slot without emitting signals
//############################
class InspectorDoubleSpinSlot : public QDoubleSpinBox
{
    Q_OBJECT
private:
    double          m_update_tolerance = 0.0;

public:
    InspectorDoubleSpinSlot(QWidget *parent = nullptr) : QDoubleSpinBox(parent) { }
    virtual ~InspectorDoubleSpinSlot() override { }

    // Getters / Setters
    void            setUpdateTolerance(double tolerance) { m_update_tolerance = tolerance; }

public slots:
    void            updateValue(double value);
};



#endif // WIDGETS_H

















