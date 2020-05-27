//
//      Created by Stephens Nunnally on 3/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QButtonGroup>
#include <QDebug>
#include <QMenu>
#include <QRadioButton>
#include <QToolBar>
#include <QToolButton>

#include "core/dr_random.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_main/form_main.h"
#include "editor/form_sound/form_sound_effect.h"
#include "editor/forms/form_blank.h"
#include "editor/forms/form_fonts.h"
#include "editor/forms/form_popup.h"
#include "editor/forms/form_progress_box.h"
#include "editor/forms/form_settings.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/enums_editor.h"

// Local Constants
const int   c_toolbar_height =  40;
const int   c_button_size_w =   32;
const int   c_button_size_h =   26;


//####################################################################################
//##    Builds FormMain ToolBar
//####################################################################################
void FormMain::buildToolBar() {

    // Widgets to use during building
    QToolButton *tool;

    // ***** Initialize toolbar widget
    m_toolbar = new QToolBar(this);
    m_toolbar->setObjectName(QStringLiteral("toolbar"));
    m_toolbar->setFixedHeight(c_toolbar_height);
    m_toolbar->setMovable(false);
    m_toolbar->setFloatable(false);
    m_toolbar->installEventFilter(new DrFilterClickAndDragWindow(m_toolbar));

    // ***** This is a container object that holds all toolbar button groups, allowing us to put them in a layout
    m_widget_toolbar = new QWidget();
    m_widget_toolbar->setObjectName(QStringLiteral("widgetToolBar"));
    m_widget_toolbar->setFixedHeight(c_toolbar_height);
    m_widget_toolbar_layout = new QHBoxLayout(m_widget_toolbar);
    m_widget_toolbar_layout->setSpacing(3);
    m_widget_toolbar_layout->setContentsMargins(12, 0, 12, 0);

    // ***** Selectable Button group that keeps track of which mode we are in: World Graph, World Creator, UI Creator, Sound Creator
    m_widget_group_mode = new QWidget();
    m_widget_group_mode->setObjectName(QStringLiteral("widgetGroupMode"));
    m_widget_group_mode->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutMode = new QHBoxLayout(m_widget_group_mode);
        toolbarLayoutMode->setSpacing(3);
        toolbarLayoutMode->setContentsMargins(0, 1, 0, 0);

        m_buttons_group_mode = new QButtonGroup();
        m_buttons_group_mode->setExclusive(true);
        connect(m_buttons_group_mode, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupModeClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonModeWorldGraph"), Advisor_Info::Mode_World_Graph, c_button_size_w + 4, c_button_size_h + 6, true);
        m_buttons_group_mode->addButton(tool, int(Editor_Mode::World_Graph));
        toolbarLayoutMode->addWidget(tool);
        toolbarLayoutMode->addWidget(createToolBarSpacer(c_button_size_h - 2));

        tool = createToolBarButton(QStringLiteral("buttonModeWorldEdit"), Advisor_Info::Mode_World_Creator, c_button_size_w + 4, c_button_size_h + 6, true);
        m_buttons_group_mode->addButton(tool, int(Editor_Mode::World_Creator));
        toolbarLayoutMode->addWidget(tool);
        toolbarLayoutMode->addWidget(createToolBarSpacer(c_button_size_h - 2));

        tool = createToolBarButton(QStringLiteral("buttonModeUIEdit"), Advisor_Info::Mode_UI_Creator, c_button_size_w + 4, c_button_size_h + 6, true);
        m_buttons_group_mode->addButton(tool, int(Editor_Mode::UI_Creator));
        toolbarLayoutMode->addWidget(tool);
        toolbarLayoutMode->addWidget(createToolBarSpacer(c_button_size_h - 2));

        tool = createToolBarButton(QStringLiteral("buttonModeSound"), Advisor_Info::Mode_Sound_Creator, c_button_size_w + 4, c_button_size_h + 6, true);
        m_buttons_group_mode->addButton(tool, int(Editor_Mode::Sound_Creator));
        toolbarLayoutMode->addWidget(tool);


    // ***** Mode "Editor" Add-On, Edit: Holds Add , Delete, future Cut / Copy / Paste
    m_widget_group_edit = new QWidget(m_widget_toolbar);
    m_widget_group_edit->hide();
    m_widget_group_edit->setObjectName(QStringLiteral("widgetGroupEdit"));
    m_widget_group_edit->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutEdit = new QHBoxLayout(m_widget_group_edit);
        toolbarLayoutEdit->setSpacing(5);
        toolbarLayoutEdit->setContentsMargins(0, 0, 0, 0);

        m_buttons_group_edit = new QButtonGroup();
        m_buttons_group_edit->setExclusive(false);
        connect(m_buttons_group_edit, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupEditClicked(int)));

        m_button_add = createToolBarButton(QStringLiteral("buttonAddThing"), Advisor_Info::Add_Entity, c_button_size_w + 14, c_button_size_h, false, true);
        m_buttons_group_edit->addButton(m_button_add, int(Buttons_Edit::Add));
        toolbarLayoutEdit->addWidget(m_button_add);

        tool = createToolBarButton(QStringLiteral("buttonDuplicate"), Advisor_Info::Duplicate, c_button_size_w, c_button_size_h, false, false);
        m_buttons_group_edit->addButton(tool, int(Buttons_Edit::Duplicate));
        toolbarLayoutEdit->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonDeleteThing"), Advisor_Info::Trash_Can, c_button_size_w, c_button_size_h, false, false);
        m_buttons_group_edit->addButton(tool, int(Buttons_Edit::Delete));
        toolbarLayoutEdit->addWidget(tool);


    // ***** Mode "Editor" Add-On, Layering: Holds buttons that send Things to Front / Back
    m_widget_group_layering = new QWidget(m_widget_toolbar);
    m_widget_group_layering->hide();
    m_widget_group_layering->setObjectName(QStringLiteral("widgetGroupLayering"));
    m_widget_group_layering->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutLayering = new QHBoxLayout(m_widget_group_layering);
        toolbarLayoutLayering->setSpacing(1);
        toolbarLayoutLayering->setContentsMargins(0, 0, 0, 0);

        m_buttons_group_layering = new QButtonGroup();
        m_buttons_group_layering->setExclusive(false);
        connect(m_buttons_group_layering, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupLayeringClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonSendToBack"), Advisor_Info::Send_to_Back, c_button_size_w, c_button_size_h, false, false);
        m_buttons_group_layering->addButton(tool, int(Buttons_Layering::Send_To_Back));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSendBackward"), Advisor_Info::Send_Backward, c_button_size_w, c_button_size_h, false, false);
        m_buttons_group_layering->addButton(tool, int(Buttons_Layering::Send_Backward));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSendForward"), Advisor_Info::Send_Forward, c_button_size_w, c_button_size_h, false, false);
        m_buttons_group_layering->addButton(tool, int(Buttons_Layering::Send_Forward));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSendToFront"), Advisor_Info::Send_to_Front, c_button_size_w, c_button_size_h, false, false);
        m_buttons_group_layering->addButton(tool, int(Buttons_Layering::Send_To_Front));
        toolbarLayoutLayering->addWidget(tool);

    // ***** Mode "Editor" Add-On, Transform: Holds buttons that Flip / Rotate things
    m_widget_group_transform = new QWidget(m_widget_toolbar);
    m_widget_group_transform->hide();
    m_widget_group_transform->setObjectName(QStringLiteral("widgetGroupTransform"));
    m_widget_group_transform->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutTransform = new QHBoxLayout(m_widget_group_transform);
        toolbarLayoutTransform->setSpacing(1);
        toolbarLayoutTransform->setContentsMargins(2, 0, 0, 0);

        m_buttons_group_transform = new QButtonGroup();
        m_buttons_group_transform->setExclusive(false);
        connect(m_buttons_group_transform, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupTransformClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonResetObject"), Advisor_Info::Reset, c_button_size_w, c_button_size_h, false, false);
        m_buttons_group_transform->addButton(tool, int(Buttons_Transform::Reset_Object));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformFlipH"), Advisor_Info::Flip_H, c_button_size_w, c_button_size_h, false, false);
        m_buttons_group_transform->addButton(tool, int(Buttons_Transform::Flip_H));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformFlipV"), Advisor_Info::Flip_V, c_button_size_w, c_button_size_h, false, false);
        m_buttons_group_transform->addButton(tool, int(Buttons_Transform::Flip_V));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformRotateL"), Advisor_Info::Rotate_L, c_button_size_w, c_button_size_h, false, false);
        m_buttons_group_transform->addButton(tool, int(Buttons_Transform::Rotate_L));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformRotateR"), Advisor_Info::Rotate_R, c_button_size_w, c_button_size_h, false, false);
        m_buttons_group_transform->addButton(tool, int(Buttons_Transform::Rotate_R));
        toolbarLayoutTransform->addWidget(tool);


    // ***** Mode "Editor" Add-On, Grid: Holds buttons that control Snap to Grid, Resize to Grid, Show Grid on Top
    m_widget_group_grid_full = new QWidget(m_widget_toolbar);
    m_widget_group_grid_full->hide();
    m_widget_group_grid_full->setObjectName(QStringLiteral("widgetGroupGridFull"));
    m_widget_group_grid_full->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutGridFull = new QHBoxLayout(m_widget_group_grid_full);
        toolbarLayoutGridFull->setSpacing(0);
        toolbarLayoutGridFull->setContentsMargins(0, 0, 0, 0);

        m_buttons_group_grid_full = new QButtonGroup();
        m_buttons_group_grid_full->setExclusive(false);
        connect(m_buttons_group_grid_full, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupGridFullClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonGridOnTop"), Advisor_Info::Grid_Show_On_Top, c_button_size_w, c_button_size_h, true);
        m_buttons_group_grid_full->addButton(tool, int(Buttons_Grid::Grid_On_Top));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Grid_On_Top).toBool());
        toolbarLayoutGridFull->addWidget(tool);
        toolbarLayoutGridFull->addSpacing(1);

        tool = createToolBarButton(QStringLiteral("buttonResizeToGrid"), Advisor_Info::Resize_To_Grid, c_button_size_w, c_button_size_h, true);
        m_buttons_group_grid_full->addButton(tool, int(Buttons_Grid::Resize_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Resize_To_Grid).toBool());
        toolbarLayoutGridFull->addWidget(tool);
        toolbarLayoutGridFull->addSpacing(1);

        tool = createToolBarButton(QStringLiteral("buttonSnapToGrid"), Advisor_Info::Grid_Snap_To_Grid, c_button_size_w, c_button_size_h, true);
        m_buttons_group_grid_full->addButton(tool, int(Buttons_Grid::Snap_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool());
        toolbarLayoutGridFull->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSnapOptions"), Advisor_Info::Grid_Snap_Options, 14, c_button_size_h, false, true);
        m_buttons_group_grid_full->addButton(tool, int(Buttons_Grid::Snap_Options));
        toolbarLayoutGridFull->addWidget(tool);

    // ***** Mode "World Graph" Add-On, Grid: Holds button that controls Snap to Grid
    m_widget_group_grid_simple = new QWidget(m_widget_toolbar);
    m_widget_group_grid_simple->hide();
    m_widget_group_grid_simple->setObjectName(QStringLiteral("widgetGroupGridSimple"));
    m_widget_group_grid_simple->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutGridSimple = new QHBoxLayout(m_widget_group_grid_simple);
        toolbarLayoutGridSimple->setSpacing(0);
        toolbarLayoutGridSimple->setContentsMargins(0, 0, 0, 0);

        m_buttons_group_grid_simple = new QButtonGroup();
        m_buttons_group_grid_simple->setExclusive(false);
        connect(m_buttons_group_grid_simple, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupGridSimpleClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonSnapToMap"), Advisor_Info::Grid_Snap_To_Grid, c_button_size_w, c_button_size_h, true);
        m_buttons_group_grid_simple->addButton(tool, int(Buttons_Grid::Snap_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool());
        toolbarLayoutGridSimple->addWidget(tool);
        toolbarLayoutGridSimple->addWidget(createToolBarSpacer(c_button_size_h - 2, 34, false));


    // ***** Mode "Editor" Add-On, Play: Holds buttons that starts game Engine window
    m_widget_group_play = new QWidget(m_widget_toolbar);
    m_widget_group_play->hide();
    m_widget_group_play->setObjectName(QStringLiteral("widgetGroupPlay"));
    m_widget_group_play->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutPlay = new QHBoxLayout(m_widget_group_play);
        toolbarLayoutPlay->setSpacing(5);
        toolbarLayoutPlay->setContentsMargins(0, 0, 0, 0);

        m_buttons_group_play = new QButtonGroup();
        m_buttons_group_play->setExclusive(false);
        connect(m_buttons_group_play, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupPlayClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonPlay"), Advisor_Info::Play_Game, c_button_size_w + 14, c_button_size_h, false, true);
        m_buttons_group_play->addButton(tool, int(Buttons_Play::Play));
        toolbarLayoutPlay->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonPlayStage"), Advisor_Info::Play_Stage, c_button_size_w, c_button_size_h, false, true);
        m_buttons_group_play->addButton(tool, int(Buttons_Play::Play_Stage));
        toolbarLayoutPlay->addWidget(tool);

        toolbarLayoutPlay->addWidget(createToolBarSpacer(c_button_size_h - 2, 34, false));


    // ***** Settings Buttons
    m_widget_group_settings = new QWidget(m_widget_toolbar);
    m_widget_group_settings->hide();
    m_widget_group_settings->setObjectName(QStringLiteral("widgetGroupSettings"));
    m_widget_group_settings->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutSettings = new QHBoxLayout(m_widget_group_settings);
        toolbarLayoutSettings->setSpacing(5);
        toolbarLayoutSettings->setContentsMargins(0, 0, 0, 0);

        // Physics Playground
        ///tool = createToolBarButton(QStringLiteral("buttonPlayground"), Advisor_Info::Settings_Playground, c_button_size_w, c_button_size_h, false);
        ///toolbarLayoutSettings->addWidget(tool);
        ///connect(tool, &QPushButton::clicked, [this] () {
        ///    FormPlayground *playground = new FormPlayground(this);
        ///    playground->show();
        ///});

        tool = createToolBarButton(QStringLiteral("buttonImageViewer"), Advisor_Info::Settings_Image_Viewer, c_button_size_w, c_button_size_h, false);
        toolbarLayoutSettings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            FormProgressBox *progress_box = new FormProgressBox("Detecting Image Shape...", "Cancel", 1, this);
            progress_box->setShowIfWaitIsLongerThan(0.25);
            progress_box->moveToNextItem();
            for (int i = 0; i <= 1000; ++i) {
                Dr::Sleep(3);
                if (i % 10 == 0) {
                    if (Dr::RandomInt(0, 8) == 3) {
                        for (int j = 0; j < Dr::RandomDouble(100000, 100000000); j++) {
                            double d = sqrt(j);
                            d = std::pow(d, 2.0);
                        }
                    }
                    progress_box->setDisplayText("Detecting Image Shape...  " + std::to_string(i / 10) + "% ");
                }
                progress_box->updateValue(i / 10);
                if (progress_box->isCanceled()) break;
            }
            progress_box->stopProgress();
        });

        tool = createToolBarButton(QStringLiteral("buttonFontBuilder"), Advisor_Info::Settings_Font_Builder, c_button_size_w, c_button_size_h, false);
        toolbarLayoutSettings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            ///FormBlank *blank_form = new FormBlank(m_project, this);
            ///blank_form->show();
            FormFonts *font_editor = new FormFonts(m_project, this);
            font_editor->show();
        });

        tool = createToolBarButton(QStringLiteral("buttonSettingsEditor"), Advisor_Info::Settings_Manager, c_button_size_w, c_button_size_h, false);
        toolbarLayoutSettings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            FormSettings *settings_editor = new FormSettings(m_project, this);
            settings_editor->show();
        });


    // ********** Set up initial toolbar
    // Clears the containers that keeps track of whats added to the toolbar layout
    m_toolbar_widgets.clear();
    m_toolbar_spacers.clear();

    // Set the initial toolbar as the mode buttons only
    m_widget_toolbar_layout->addWidget(m_widget_group_mode);
    m_widget_toolbar_layout->addStretch();

    // Add the toolbar onto FormMain
    m_toolbar->addWidget(m_widget_toolbar);
    m_widget_toolbar->setFixedWidth( this->width() );
    this->addToolBar(Qt::ToolBarArea::TopToolBarArea, m_toolbar);
}




//####################################################################################
//##    Button creation calls
//####################################################################################
QToolButton* FormMain::createToolBarButton(const QString &style_sheet_name, HeaderBodyList advisor_text, int w, int h, bool checkable, bool enabled) {
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
    ///Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
    return tool;
}

// DEFAULTS: height = 24, space_on_the_right = 1, visible = true
QLabel* FormMain::createToolBarSpacer(int height, int space_on_the_right, bool visible) {
    QLabel *spacer = new QLabel();
    if (visible)
        spacer->setObjectName(QStringLiteral("labelSpacer"));
    else
        spacer->setObjectName(QStringLiteral("labelSpacerNotVisible"));
    spacer->setFixedSize( space_on_the_right, height );
    return spacer;
}

QPushButton* FormMain::createPushButton(QString name, QString text) {
    QFont font = Dr::CustomFont();

    QPushButton *button = new QPushButton();
    button->setObjectName( name );
    button->setFont(font);
    button->setFixedSize(80, 24);
    Dr::ApplyDropShadowByType(button,    Shadow_Types::Button_Shadow);
    button->setText( text );
    return button;
}








