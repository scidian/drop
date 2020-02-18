//
//      Created by Stephens Nunnally on 2/5/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_PROGRESS_BOX_H
#define FORM_PROGRESS_BOX_H

#include <QElapsedTimer>
#include <QLabel>
#include <QProgressBar>
#include <QThread>
#include <QTimer>
#include <QWidget>

#include <deque>

#include "core/interface/dr_progress.h"

// Forward declarations
class DrProject;


//####################################################################################
//##    FormProgressBox
//##        A popup progress box to be used during intense calculations
//##
//##        After initial creation, updateValue is called for each item from 0 to 100, and then
//##        moveToNextItem() is called and them updateValue from 0 to 100 agfain and so on
//##
//############################
class FormProgressBox : public QWidget, public IProgressBar
{
    Q_OBJECT

public:
    // Constructor / Destructor
    FormProgressBox(QString info_text = "Working...", QString cancel_button_text = "Cancel", int items = 1, QWidget *parent = nullptr);
    virtual ~FormProgressBox() override;


private:
    // Form Widgets
    QWidget        *m_inner_widget          { nullptr };                // Container widget, allows for a double form border
    QLabel         *m_info_text             { nullptr };                // Working text
    QProgressBar   *m_progress_bar          { nullptr };                // Progress bar...
    QTimer         *m_color_timer           { nullptr };                // Timer that updates progress bar colors

    // Progress Variables
    int             m_start_value           {   0 };                    // Progress bar start value
    int             m_end_value             { 100 };                    // Progress bar end value
    int             m_current_value         {   0 };                    // Current value of progress bar

    bool            m_canceled              { false };                  // Becomes true if user cancels event

    // Timer Variables
    QElapsedTimer       m_start_time;
    std::deque<double>  m_estimated_times   { };                        // Stores last 5 estimated times for averaging




public:
    // Event Overrides
    virtual void    resizeEvent(QResizeEvent *event) override;

    // Virtual Progress Bar Features
    virtual void    setDisplayText(std::string text) override;
    virtual void    stopProgress() override;
    virtual bool    updateValue(int i) override;

    // Progress Functions
    bool            applyValue(int new_value);

    // Getters / Setters
    int             getStartValue() { return m_start_value; }
    int             getEndValue()   { return m_end_value; }


public slots:
    void            updateColors();

};



class TThread : public QThread
{
    Q_OBJECT

    void run();

public:
    explicit TThread(QObject *parent = nullptr);

signals:

private slots:
    void timerOut();
};




#endif // FORM_PROGRESS_BOX_H





















