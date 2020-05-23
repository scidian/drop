//
//      Created by Stephens Nunnally on 5/22/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPushButton>
#include <QToolButton>

#include "core/colors/colors.h"
#include "core/imaging/imaging.h"
#include "editor/form_main/form_main.h"
#include "editor/event_filters.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/view_editor/editor_view.h"
#include "editor/view_node_map/node_map_view.h"
#include "editor/widgets/widgets_editor.h"
#include "editor/widgets/widgets_inspector.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
EditorViewToolbar::EditorViewToolbar(QWidget *parent, DrProject *project, IEditorRelay *editor_relay, Editor_Mode editor_mode,
                                     EditorView *view_editor, NodeMapView *view_node)
    : QFrame(parent) {

    // Make sure we have the proper class pointer to the Editor_Mode requested
    switch (editor_mode) {
        case Editor_Mode::Component_Map:
        case Editor_Mode::World_Map:
            if (view_node == nullptr)   editor_mode = Editor_Mode::Clear;
            break;
        case Editor_Mode::Animation_Editor:
        case Editor_Mode::UI_Editor:
        case Editor_Mode::World_Editor:
            if (view_editor == nullptr) editor_mode = Editor_Mode::Clear;
            break;
        case Editor_Mode::Clear:
        case Editor_Mode::Program_Loading:
            view_node =   nullptr;
            view_editor = nullptr;
            break;
    }

    m_project =         project;
    m_editor_relay =    editor_relay;
    m_editor_mode =     editor_mode;
    m_view_editor =     view_editor;
    m_view_node =       view_node;

    // Initialize hover handler
    m_filter_hover = new DrFilterHoverHandler(this);
    connect(m_filter_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    // Build this Toolbar
    buildToolbar();
}


//####################################################################################
//##    SLOT: Catches signals from m_filter_hover and passes to InterfaceEditorRelay
//####################################################################################
void EditorViewToolbar::setAdvisorInfo(QString header, QString body) {
    m_editor_relay->setAdvisorInfo(header, body);
}


//####################################################################################
//##    buttonGroupMouse SLOT and functions
//####################################################################################
void EditorViewToolbar::buttonGroupMouseClicked(int id) {
    m_editor_relay->viewUpdateToolbar(id);
}

void EditorViewToolbar::updateButtons(int id) {
    Mouse_Mode clicked = static_cast<Mouse_Mode>(id);

    if (clicked == Mouse_Mode::Pointer) {
        mouse_pointer->setChecked(true);
        widgetGroupHandTool->setVisible(false);
        widgetGroupZoomTool->setVisible(false);

    } else if (clicked == Mouse_Mode::Hand) {
        mouse_hand->setChecked(true);
        widgetGroupZoomTool->setVisible(false);
        widgetGroupHandTool->setVisible(true);

    } else if (clicked == Mouse_Mode::Magnify) {
        mouse_magnify->setChecked(true);
        widgetGroupHandTool->setVisible(false);
        widgetGroupZoomTool->setVisible(true);
    }
}


//####################################################################################
//##    buttonGroupToggle SLOT and functions
//####################################################################################
void EditorViewToolbar::buttonGroupToggleClicked(int id) {
    Buttons_Toggle clicked = static_cast<Buttons_Toggle>(id);

    if (clicked == Buttons_Toggle::CameraOnOff) {
        Dr::SetPreference(Preferences::World_Editor_Show_Camera_Boxes, buttonsGroupToggle->button(id)->isChecked());
        m_editor_relay->getEditorView()->update();

    } else if (clicked == Buttons_Toggle::DebugOnOff) {
        Dr::SetPreference(Preferences::World_Editor_Show_Collision_Shapes, buttonsGroupToggle->button(id)->isChecked());
        m_editor_relay->getEditorView()->update();
    }
}


//####################################################################################
//##    Button creation calls
//####################################################################################
QToolButton* EditorViewToolbar::createToolBarButton(const QString &style_sheet_name, HeaderBodyList advisor_text, int w, int h, bool checkable, bool enabled) {
    QToolButton *tool = new QToolButton();
    tool->setObjectName(style_sheet_name);
    if (checkable) {
        tool->setCheckable(true);
        tool->setChecked(false);
    }
    tool->setToolTip(advisor_text[0]);
    tool->setEnabled(enabled);
    tool->setFixedSize(w, h);
    m_filter_hover->attachToHoverHandler(tool, advisor_text);
    return tool;
}

// DEFAULTS: height = 24, space_on_the_right = 1, visible = true
QLabel* EditorViewToolbar::createToolBarSpacer(int height, int space_on_the_right, bool visible) {
    QLabel *spacer = new QLabel();
    if (visible) spacer->setObjectName(QStringLiteral("labelSpacer"));
    else         spacer->setObjectName(QStringLiteral("labelSpacerNotVisible"));
    spacer->setFixedSize( space_on_the_right, height );
    return spacer;
}

QPushButton* EditorViewToolbar::createPushButton(QString name, QString text) {
    QFont font = Dr::CustomFont();
    QPushButton *button = new QPushButton();
    button->setObjectName( name );
    button->setFont(font);
    button->setFixedSize(80, 24);
    Dr::ApplyDropShadowByType(button,    Shadow_Types::Button_Shadow);
    button->setText( text );
    return button;
}














