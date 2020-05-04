//
//      Created by Stephens Nunnally on 10/22/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_EXPIRE_H
#define FORM_EXPIRE_H

#include <QMainWindow>
#include <QWidget>

// Forward Declarations
class DrProject;


//####################################################################################
//##    Pops up on Alpha versions to warn version expired
//############################
class FormExpire : public QMainWindow
{
private:
    // Local Variables
    QWidget     *m_inner_widget;                    // Container widget, allows for a double form border
    bool         m_just_loaded = true;

public:
    FormExpire(QWidget *parent = nullptr);

    // Event Overrides
    virtual void    closeEvent(QCloseEvent *event) override;
    virtual void    resizeEvent(QResizeEvent *event) override;

};

#endif // FORM_EXPIRE_H
