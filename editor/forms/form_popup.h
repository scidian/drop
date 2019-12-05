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
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

// Forward declarations
class       DrProject;
class       FormPopup;
class       TreeInspector;
enum class  Color_Palettes;         // Defined in "editor/colors.h"
enum class  DrAssetType;            // Defined in "model/enums_types.h"

enum class Colors {
    Basic,
    Grays,
    Main,
    Accent,
    History,
};

//####################################################################################
//##    FormPopup
//##        Class to hold a Popup Form
//############################
class FormPopup : public QWidget
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject       *m_project;                         // Pointer to the open project
    QWidget         *m_mapper;                          // Widget to use for mapping to global coordinates (usually same as parent)

    // Local Variables
    QWidget         *m_inner_widget;                    // Inner container widget, allows for a double form border

    QVBoxLayout     *m_layout;                          // Layout for *this form
    bool             m_below = true;                    // Tracks if this popup is shown below or above origin point
    QPoint           m_offset;                          // Adjustment to popup location from parent

public:
    // Constructor
    FormPopup(QWidget *parent, DrProject *project, QWidget *widget_to_use_for_mapToGlobal, int x_offset = 0, int y_offset = 5);

    // Event Overrides
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

    // Getters
    DrProject*      getProject()            { return m_project; }
    QWidget*        getMapper()             { return m_mapper; }
    QWidget*        getInnerWidget()        { return m_inner_widget; }


    // ***** Custom Pop-Ups
    void            buildPopupAddEntity();                                              // Add Entity to Project
    void            addAssetFromPopup(DrAssetType asset_type, long source_key);         // Child function for Popup Entity Form

    void            buildPopupGridSnap();                                               // Snap to Grid Option
};


//####################################################################################
//##    FormPopupColor
//##        A subclassed FormPopup to be used for Color Picker Popup window
//############################
class FormPopupColor : public FormPopup
{
    Q_OBJECT

private:
    QWidget         *m_wants_return_variable;           // A widget that wants something from this popup when its done
    QColor           m_start_color;                     // Selected color when the color popup starts

    Color_Palettes   m_current_palette;                 // Palette currently shown on palette page
    QStackedWidget  *m_palette_block;                   // Widget holding the currently selected palette color buttons

    QLabel          *m_color_label;                     // Label that shows the color to be picked

public:
    FormPopupColor(DrProject *project, QWidget *widget_to_use_for_mapToGlobal, QWidget *parent, int x_offset = 0, int y_offset = 5);

    // Event Handlers
    virtual void closeEvent(QCloseEvent *event) override;

    // Building Functions
    void         buildPopupColors(QWidget *wants_color, QColor start_color);
    QWidget*     createColorBlock(Colors type, int start_index, int columns, int rows, int mid_step,
                                  int block_width, int block_height, int border, int x_spacing, int y_spacing);
    QString      createButtonStyleSheet(QColor color, int border);
    void         setInfoLabelColor(QColor color);

    // Getters
    QLabel*         getColorLabel()    { return m_color_label; }
    QWidget*        getReturnWidget()  { return m_wants_return_variable; }
    QColor          getStartColor()    { return m_start_color; }

signals:
    void         colorGrabbed(QWidget *parent, QColor m_color);

};


//####################################################################################
//##    DrQPushButtonPageSelect
//##        Button to select between color box pages
//############################
class DrQPushButtonPageSelect : public QPushButton
{
    FormPopupColor *m_popup;                // FormPopupColor form this button is installed on
    QString         m_description;          // What to show I am in the info label
    QString         m_text_before;          // Text info color box had before we changed it

public:
    explicit DrQPushButtonPageSelect(QWidget *parent, FormPopupColor *popup, QString description);
    virtual ~DrQPushButtonPageSelect() override;

    // Event Overrides
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
};


//####################################################################################
//##    DrQPushButtonColorSelect
//##        A little color box label that shows a color to pick
//############################
class DrQPushButtonColorSelect : public QPushButton
{
    FormPopupColor *m_popup;                // FormPopupColor form this button is installed on
    QColor          m_color;                // This label's color

    int             m_width;                // This blocks width
    int             m_height;               // This blocks height

public:
    explicit DrQPushButtonColorSelect(QWidget *parent, FormPopupColor *popup, QColor my_color, int width, int height);
    virtual ~DrQPushButtonColorSelect() override;

    // Event Overrides
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
};


#endif // FORM_POPUP_H






