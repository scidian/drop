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
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

#include "core/dr_math.h"


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

    int             getDrawLeft()           { return m_draw_left; }
    int             getDrawTop()            { return m_draw_top; }
    void            setDrawLeft(int left)   { m_draw_left = left; }
    void            setDrawTop(int top)     { m_draw_top = top; }

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


//class DoubleSlider(QSlider):

//    # create our our signal that we can connect to if necessary
//    doubleValueChanged = pyqtSignal(float)

//    def __init__(self, decimals=3, *args, **kargs):
//        super(DoubleSlider, self).__init__( *args, **kargs)
//        self._multi = 10 ** decimals
//        self.valueChanged.connect(self.emitDoubleValueChanged)

//    def emitDoubleValueChanged(self):
//        value = float(super(DoubleSlider, self).value())/self._multi
//        self.doubleValueChanged.emit(value)

//    def value(self):
//        return float(super(DoubleSlider, self).value()) / self._multi

//    def setMinimum(self, value):
//        return super(DoubleSlider, self).setMinimum(value * self._multi)

//    def setMaximum(self, value):
//        return super(DoubleSlider, self).setMaximum(value * self._multi)

//    def setSingleStep(self, value):
//        return super(DoubleSlider, self).setSingleStep(value * self._multi)

//    def singleStep(self):
//        return float(super(DoubleSlider, self).singleStep()) / self._multi

//    def setValue(self, value):
//        super(DoubleSlider, self).setValue(int(value * self._multi))



//####################################################################################
//##    DoubleSlider
//##        Allows us to use double precision with a slider
//############################
class InspectorDoubleSlider : public QSlider
{
    Q_OBJECT

private:
    int     m_decimals      { 0 };
    double  m_multiplier    { 1.0 };

public:
    InspectorDoubleSlider(QWidget *parent = nullptr, int decimals = 0) : QSlider(parent), m_decimals(decimals) {
        m_multiplier = std::pow(10.0, static_cast<double>(m_decimals));
        connect(this, SIGNAL(valueChanged(int)), this, SLOT(doubleValueChanged(int)));
    }

    double value() { return double(QSlider::value()) * m_multiplier; }



public slots:
    void    doubleValueChanged(int value) {
        double value_double = double(value) * m_multiplier;

    }
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

















