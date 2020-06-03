//
//      Created by Stephens Nunnally on 5/24/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef WIDGETS_TOOLBAR_H
#define WIDGETS_TOOLBAR_H

#include <QButtonGroup>
#include <QElapsedTimer>
#include <QFrame>
#include <QLabel>
#include <QGraphicsView>
#include <QMap>
#include <QPushButton>
#include <QRubberBand>
#include <QSlider>
#include <QToolButton>

#include "core/types/dr_pointf.h"
#include "editor/constants_advisor_info.h"
#include "editor/enums_editor.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrFilterHoverHandler;
class EditorView;
class MixerView;
class NodeView;


//####################################################################################
//##    ViewToolbar
//##        A mini toolbar for use with a View
//############################
class ViewToolbar : public QFrame
{
    Q_OBJECT

private:
    // External Borrowed Pointers
    DrProject              *m_project;                                  // Pointer to currently loaded project
    IEditorRelay           *m_editor_relay;                             // Pointer to IEditorRelay class of parent form
    EditorView             *m_view_editor       { nullptr };            // Holds parent EditorView* if m_my_editor_mode is a mode that uses EditorView
    NodeView               *m_view_node         { nullptr };            // Holds parent NodeView*   if m_my_editor_mode is a mode that uses NodeView


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
    ViewToolbar(QWidget *parent, DrProject *project, IEditorRelay *editor_relay, Editor_Mode editor_mode,
                EditorView *view_editor = nullptr, NodeView *view_node = nullptr, MixerView *view_mixer = nullptr);

    // Building
    void            buildToolbar();
    QToolButton*    createToolBarButton(const QString &style_sheet_name, HeaderBodyList advisor_text, bool checkable = true, bool enabled = true);
    QLabel*         createToolBarSpacer(int height = 24, int space_on_the_right = 1, bool visible = true);
    QPushButton*    createPushButton(QString name = "toolbarButton", QString text = "Button");
    void            updateButtons(int id);

private slots:
    void            buttonGroupMouseClicked(int id);
    void            buttonGroupToggleClicked(int id);
    void            setAdvisorInfo(QString header, QString body);
};


//####################################################################################
//##    ToolbarSlider
//##        Allows us to use Style Sheets with QSlider and still have Tick Marks painted
//############################
class ToolbarSlider : public QSlider
{
    Q_OBJECT

private:
    QColor          m_tick_color = Qt::white;

public:
    ToolbarSlider(QWidget *parent = nullptr) : QSlider(parent) { }
    virtual ~ToolbarSlider() override { }

    // Event Overrides
    virtual void    paintEvent(QPaintEvent *event) override;

    // Getters / Setters
    void            setTickColor(QColor color) {    m_tick_color =  color; }

public slots:
    void            updateValue(int value);
};


#endif // WIDGETS_TOOLBAR_H










