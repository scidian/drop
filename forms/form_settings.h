//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_SETTINGS_H
#define FORM_SETTINGS_H

#include <QLabel>
#include <QWidget>

// Forward declarations
class DrProject;

//####################################################################################
//##    FormSettings
//##        Form to display Project settings
//############################
class FormSettings : public QWidget
{
private:
    // External Borrowed Pointers
    DrProject       *m_project;                         // Pointer to the open project

    // Local Variables
    QWidget         *m_inner_widget;                    // Container widget, allows for a double form border


public:
    explicit FormSettings(DrProject *project, QWidget *parent = nullptr);
    virtual ~FormSettings() override;

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;

};






#endif // FORM_SETTINGS_H






















