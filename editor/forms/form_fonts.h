//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      Font Builder
//
//
#ifndef FORM_FONTS_H
#define FORM_FONTS_H

#include <QWidget>

// Forward Declarations
class DrProject;


//####################################################################################
//##    FormFonts
//##        Font builder form
//############################
class FormFonts : public QWidget
{
private:
    // External Borrowed Pointers
    DrProject   *m_project;                         // Pointer to the open project

    // Form Variables
    QWidget     *m_inner_widget;                    // Container widget, allows for a double form border

public:
    // Constructor
    FormFonts(DrProject *project, QWidget *parent = nullptr);

    // Event Overrides
    virtual void resizeEvent(QResizeEvent *event) override;


};


#endif // FORM_FONTS_H



