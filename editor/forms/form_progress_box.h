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
#include <QTimer>
#include <QWidget>

// Forward declarations
class DrProject;

//####################################################################################
//##    FormProgressBox
//##        A popup progress box to be used during intense calculations
//############################
class FormProgressBox : public QWidget
{
    Q_OBJECT

public:
    // Constructor / Destructor
    FormProgressBox(QString info_text = "Working...", QString cancel_button_text = "Cancel", int min = 0, int max = 100, QWidget *parent = nullptr);
    virtual ~FormProgressBox() override;


private:
    // Form Widgets
    QWidget        *m_inner_widget          { nullptr };                // Container widget, allows for a double form border
    QLabel         *m_info_text             { nullptr };                // Working text
    QProgressBar   *m_progress_bar          { nullptr };                // Progress bar...
    QTimer         *m_color_timer           { nullptr };                // Timer that updates progress bar colors

    // Progress Variables
    double          m_show_if_longer_than   { 2.0 };                    // Don't show form if calculated time is less than this many seconds

    int             m_start_value           {   0 };                    // Progress bar start value
    int             m_end_value             { 100 };                    // Progress bar end value
    int             m_current_value         {   0 };                    // Current value of progress bar

    QElapsedTimer   m_start_time;
    bool            m_canceled              { false };                  // Becomes true if user cancels event


public:
    // Event Overrides
    virtual void    resizeEvent(QResizeEvent *event) override;

    // Progress Functions
    void            setInfoText(QString info_text);
    void            setShowIfWaitIsLongerThan(double seconds)       { m_show_if_longer_than = seconds; }
    void            setValue(int new_value);
    bool            wasCanceled()                                   { return m_canceled; }


public slots:
    void            updateColors();

};


#endif // FORM_PROGRESS_BOX_H





















