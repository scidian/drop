//
//      Created by Stephens Nunnally on 10/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_ABOUT_H
#define FORM_ABOUT_H

#include <QWidget>

// Forward Declarations
class DrProject;


//####################################################################################
//##    FormAbout
//##        About Form
//############################
class FormAbout : public QWidget
{
private:
    // External Borrowed Pointers
    DrProject   *m_project;                                 // Pointer to the open project

    // Local Variables
    QWidget     *m_inner_widget;                            // Container widget, allows for a double form border
    bool         m_finished_loading = false;                // True once finished laoding

public:
    FormAbout(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;

    // Form Functions
    void    buildForm();
    QString licenseText();

};


#endif // FORM_ABOUT_H
