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


enum class Colors {
    Basic,
    Grays,
    Main,
    Accent,
};


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
    QWidget     *m_wants_return_variable;           // A widget that wants something from this popup when its done

    bool         m_below = true;                    // Tracks if this popup is shown below or above origin point
    QPoint       m_offset;                          // Adjustment to popup location from parent

public:
    FormPopup(DrProject *project, QWidget *widget_to_use_for_mapToGlobal, QWidget *parent, int x_offset = 0, int y_offset = 5);

    // Event Overrides
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

    // Getters and Setters
    QWidget*     getWidget() { return m_inner_widget; }

    QPoint       getOffset() { return m_offset; }
    void         setOffset(QPoint offset) { m_offset = offset; }

    // Functions
    void         updateMask();


    // ***** Snap to Grid Option Popup
    void         buildPopupGridSnap();


    // ***** Color Popup
    void         buildPopupColors(QWidget *wants_color, QColor start_color);

    QWidget*     createColorBlock(QLabel *info_label, int start_index, int columns, int rows, int mid_step,
                                  int block_width, int block_height, int border, int x_spacing, int y_spacing, Colors type);
    QString      createButtonStyleSheet(QColor color, int border);
    void         setInfoLabelColor(QLabel *label, QColor color);


signals:
    void         colorGrabbed(QWidget *parent, QColor m_color);

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
    virtual void leaveEvent(QEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
};


#endif // FORM_POPUP_H






