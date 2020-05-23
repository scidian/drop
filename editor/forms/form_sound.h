//
//      Created by Stephens Nunnally on 5/23/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_SOUND_H
#define FORM_SOUND_H

#include <QLabel>
#include <QWidget>

// Forward Declarations
class DrProject;


//####################################################################################
//##    FormSound
//##        Form to display Sound Files
//############################
class FormSound : public QWidget
{
private:
    // External Borrowed Pointers
    DrProject       *m_project;                         // Pointer to the open project

    // Local Variables
    QWidget         *m_inner_widget;                    // Container widget, allows for a double form border


public:
    // Constructor / Destructor
    explicit FormSound(DrProject *project, QWidget *parent = nullptr);
    virtual ~FormSound() override;

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;


    // SoLoud Testing
    void    playSound();


};

#endif // FORM_SOUND_H
