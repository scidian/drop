//
//      Created by Stephens Nunnally on 3/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef FORM_POPUP_H
#define FORM_POPUP_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class DrProject;
class FormPopup;
class TreeInspector;


// Popups
namespace Dr {
    void        BuildPopupColors(QWidget *parent_form, FormPopup *popup, QWidget *wants_color, QColor start_color);
    void        BuildPopupGridSnap(QWidget *parent_form, FormPopup *popup);
}


//####################################################################################
//##    FormPopup
//##        Class to hold a Popup Form
//############################
class FormPopup : public QWidget
{
    Q_OBJECT

private:
    DrProject   *m_project;                         // Pointer to the open project

    QWidget     *m_mapper;                          // Widget to use for mapping to global coordinates (usually same as parent)
    QWidget     *m_inner_widget;                    // Container widget, allows for a double form border

    QPoint       m_offset { 0, 5};                  // Adjustment to popup location from parent

public:
    FormPopup(DrProject *project, QWidget *widget_to_use_for_mapToGlobal, QWidget *parent);

    // Event Overrides
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

    // Getters and Setters
    QWidget*     getWidget() { return m_inner_widget; }

    QPoint       getOffset() { return m_offset; }
    void         setOffset(QPoint offset) { m_offset = offset; }

signals:
    void        colorGrabbed(QWidget *parent, QColor m_color);

};



//####################################################################################
//##    ColorSelecterButton
//##        A little color box label that shows a color to pick
//############################
class ColorSelecterButton : public QPushButton
{
    FormPopup   *m_popup;               // Popup form this button is installed on
    QWidget     *m_widget;              // Widget that wants the color
    QLabel      *m_info_label;          // Label that will temporarily show this color
    QColor       m_color;               // This label's color

public:
    explicit ColorSelecterButton(QWidget *parent, FormPopup *popup, QWidget *wants_color, QLabel *info_label, QColor my_color);
    virtual ~ColorSelecterButton() override;

    // Event Overrides
    virtual void enterEvent(QEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
};


#endif // FORM_POPUP_H






