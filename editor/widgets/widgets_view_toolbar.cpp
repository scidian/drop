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
#include "editor/event_filters/event_filters.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/view_editor/editor_view.h"
#include "editor/view_node/node_view.h"
#include "editor/widgets/widgets_view.h"
#include "editor/widgets/widgets_inspector.h"
#include "editor/widgets/widgets_view_toolbar.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ViewToolbar::ViewToolbar(QWidget *parent, DrProject *project, IEditorRelay *editor_relay, Editor_Mode editor_mode,
                         EditorView *view_editor, NodeView *view_node, MixerView *view_mixer)
    : QFrame(parent) {

    // Make sure we have the proper class pointer to the Editor_Mode requested
    switch (editor_mode) {
        case Editor_Mode::Component_Graph:
        case Editor_Mode::World_Graph:
            if (view_node == nullptr)   editor_mode = Editor_Mode::Clear;
            break;
        case Editor_Mode::Animation_Creator:
        case Editor_Mode::UI_Creator:
        case Editor_Mode::World_Creator:
            if (view_editor == nullptr) editor_mode = Editor_Mode::Clear;
            break;
        case Editor_Mode::Sound_Creator:
            if (view_mixer == nullptr)  editor_mode = Editor_Mode::Clear;
        case Editor_Mode::Clear:
        case Editor_Mode::Program_Loading:            
            view_editor =   nullptr;
            view_mixer =    nullptr;
            view_node =     nullptr;
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
void ViewToolbar::setAdvisorInfo(QString header, QString body) {
    m_editor_relay->setAdvisorInfo(header, body);
}


//####################################################################################
//##    buttonGroupMouse SLOT and functions
//####################################################################################
void ViewToolbar::buttonGroupMouseClicked(int id) {
    m_editor_relay->viewUpdateToolbar(id);
}

void ViewToolbar::updateButtons(int id) {
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
void ViewToolbar::buttonGroupToggleClicked(int id) {
    Buttons_Toggle clicked = static_cast<Buttons_Toggle>(id);

    if (clicked == Buttons_Toggle::CameraOnOff) {
        Dr::SetPreference(Preferences::Editor_Show_Camera_Boxes, buttonsGroupToggle->button(id)->isChecked());
        m_editor_relay->getViewEditor()->update();

    } else if (clicked == Buttons_Toggle::DebugOnOff) {
        Dr::SetPreference(Preferences::Editor_Show_Collision_Shapes, buttonsGroupToggle->button(id)->isChecked());
        m_editor_relay->getViewEditor()->update();
    }
}


//####################################################################################
//##    Button creation calls
//####################################################################################
QToolButton* ViewToolbar::createToolBarButton(const QString &style_sheet_name, HeaderBodyList advisor_text, bool checkable, bool enabled) {
    QToolButton *tool = new QToolButton();
    tool->setObjectName(style_sheet_name);
    if (checkable) {
        tool->setCheckable(true);
        tool->setChecked(false);
    }
    tool->setToolTip(advisor_text[0]);
    tool->setEnabled(enabled);
    m_filter_hover->attachToHoverHandler(tool, advisor_text);
    return tool;
}

// DEFAULTS: height = 24, space_on_the_right = 1, visible = true
QLabel* ViewToolbar::createToolBarSpacer(int height, int space_on_the_right, bool visible) {
    QLabel *spacer = new QLabel();
    if (visible) spacer->setObjectName(QStringLiteral("labelSpacer"));
    else         spacer->setObjectName(QStringLiteral("labelSpacerNotVisible"));
    spacer->setFixedSize( space_on_the_right, height );
    return spacer;
}

QPushButton* ViewToolbar::createPushButton(QString name, QString text) {
    QFont font = Dr::CustomFont();
    QPushButton *button = new QPushButton();
    button->setObjectName( name );
    button->setFont(font);
    button->setFixedSize(80, 24);
    Dr::ApplyDropShadowByType(button,    Shadow_Types::Button_Shadow);
    button->setText( text );
    return button;
}














