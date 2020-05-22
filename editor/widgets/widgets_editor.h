//
//      Created by Stephens Nunnally on 5/22/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WIDGETS_EDITOR_H
#define WIDGETS_EDITOR_H

#include <QButtonGroup>
#include <QElapsedTimer>
#include <QLabel>
#include <QGraphicsView>
#include <QMap>
#include <QPushButton>
#include <QRubberBand>
#include <QToolButton>

#include "core/types/dr_pointf.h"
#include "editor/constants_advisor_info.h"
#include "editor/enums_editor.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrFilterHoverHandler;
class EditorView;
class NodeMapView;


//####################################################################################
//##    EditorViewToolbar
//##        A mini toolbar for use with a View
//############################
class EditorViewToolbar : public QFrame
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                  // Pointer to currently loaded project
    IEditorRelay           *m_editor_relay;                             // Pointer to IEditorRelay class of parent form
    EditorView             *m_view_editor       { nullptr };            // Holds parent EditorView*  if m_my_editor_mode is a mode that uses EditorView
    NodeMapView            *m_view_node         { nullptr };            // Holds parent NodeMapView* if m_my_editor_mode is a mode that uses NodeMapView


    // Local Variables
    DrFilterHoverHandler   *m_filter_hover;                             // Pointer to an event filter hover handler
    Editor_Mode             m_editor_mode;                              // Editor Mode of this toolbars parent View

    // Mouse Mode Buttons
    QToolButton        *mouse_pointer, *mouse_hand, *mouse_magnify;

    // Mini Tool Bar Widgets
    QWidget            *widgetGroupMouse;
    QWidget            *widgetGroupToggle;
    QWidget            *widgetGroupHandTool;
    QWidget            *widgetGroupZoomTool;

    // Button Groups
    QButtonGroup       *buttonsGroupMouse;
    QButtonGroup       *buttonsGroupToggle;

public:
    // Constructor / Destructor
    EditorViewToolbar(QWidget *parent, DrProject *project, IEditorRelay *editor_relay, Editor_Mode editor_mode,
                      EditorView *view_editor = nullptr, NodeMapView *view_node = nullptr);

    // Building
    QToolButton*    createToolBarButton(const QString &style_sheet_name, HeaderBodyList advisor_text, int w, int h, bool checkable = true, bool enabled = true);
    QLabel*         createToolBarSpacer(int height = 24, int space_on_the_right = 1, bool visible = true);
    QPushButton*    createPushButton(QString name = "toolbarButton", QString text = "Button");
    void            updateButtons(int id);

private slots:
    void            buttonGroupMouseClicked(int id);
    void            buttonGroupToggleClicked(int id);
    void            setAdvisorInfo(QString header, QString body);
};


//####################################################################################
//##    EditorViewRubberBand
//##        A sub classed QRubberBand so we can override paint event for rubber band
//############################
class EditorViewRubberBand : public QRubberBand
{
public:
    // Constructor
    EditorViewRubberBand(Shape shape, QWidget *parent) : QRubberBand (shape, parent) { }

    // Event overrides
    virtual void    paintEvent(QPaintEvent *) override;
};



//####################################################################################
//##    EditorViewToolTip
//##        A parentless widget to be used as a custom tooltip
//############################
class EditorViewToolTip : public QWidget
{
private:
    View_Mode   m_tip_type = View_Mode::None;           // Which type of tool tip to show
    QPoint      m_offset;                               // Stores how much to offset the current tooltip from mouse position
    double      m_angle = 0;                            // Stores angle to show in tooltip
    double      m_x = 0;                                // Stores x value of resizing / moving
    double      m_y = 0;                                // Stores y value of resizing / moving
    int         m_int = 0;                              // Stores zoom scale
    int         m_x_radius = 100;
    int         m_y_radius = 100;

public:
    // Constructor
    EditorViewToolTip(QWidget *parent = nullptr);

    // Event overrides
    virtual void    paintEvent(QPaintEvent *) override;

    // Functions
    void            drawText(QPainter &painter, int left_offset = 0, int top_offset = 0);
    void            startToolTip(View_Mode type, QPoint mouse_position, QVariant data);
    void            stopToolTip();
    void            updateToolTipPosition(QPoint mouse_position);
    void            updateToolTipData(QVariant data);

    // Getters and Setters
    QPoint          getOffset()  { return m_offset;   }
    View_Mode       getTipType() { return m_tip_type; }
    int             getXRadius() { return m_x_radius; }
    int             getYRadius() { return m_y_radius; }

};


#endif // WIDGETS_EDITOR_H




















