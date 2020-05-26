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
#include "editor/form_sound/form_sound.h"
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
    toolbar = new QToolBar(this);
    toolbar->setObjectName(QStringLiteral("toolbar"));
    toolbar->setFixedHeight(c_toolbar_height);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->installEventFilter(new DrFilterClickAndDragWindow(toolbar));

    // ***** This is a container object that holds all toolbar button groups, allowing us to put them in a layout
    widgetToolBar = new QWidget();
    widgetToolBar->setObjectName(QStringLiteral("widgetToolBar"));
    widgetToolBar->setFixedHeight(c_toolbar_height);
    widgetToolBarLayout = new QHBoxLayout(widgetToolBar);
    widgetToolBarLayout->setSpacing(3);
    widgetToolBarLayout->setContentsMargins(12, 0, 12, 0);

    // ***** Selectable Button group that keeps track of which mode we are in: World Map, World Editor, UI Editor
    widgetGroupMode = new QWidget();
    widgetGroupMode->setObjectName(QStringLiteral("widgetGroupMode"));
    widgetGroupMode->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutMode = new QHBoxLayout(widgetGroupMode);
        toolbarLayoutMode->setSpacing(3);
        toolbarLayoutMode->setContentsMargins(0, 1, 0, 0);

        buttonsGroupMode = new QButtonGroup();
        buttonsGroupMode->setExclusive(true);
        connect(buttonsGroupMode, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupModeClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonModeWorldMap"), Advisor_Info::Mode_Map, c_button_size_w + 4, c_button_size_h + 6, true);
        buttonsGroupMode->addButton(tool, int(Editor_Mode::World_Map));
        toolbarLayoutMode->addWidget(tool);
        toolbarLayoutMode->addWidget(createToolBarSpacer(c_button_size_h - 2));

        tool = createToolBarButton(QStringLiteral("buttonModeWorldEdit"), Advisor_Info::Mode_Editor, c_button_size_w + 4, c_button_size_h + 6, true);
        buttonsGroupMode->addButton(tool, int(Editor_Mode::World_Editor));
        toolbarLayoutMode->addWidget(tool);
        toolbarLayoutMode->addWidget(createToolBarSpacer(c_button_size_h - 2));

        tool = createToolBarButton(QStringLiteral("buttonModeUIEdit"), Advisor_Info::Mode_UI, c_button_size_w + 4, c_button_size_h + 6, true);
        buttonsGroupMode->addButton(tool, int(Editor_Mode::UI_Editor));
        toolbarLayoutMode->addWidget(tool);


    // ***** Mode "Editor" Add-On, Edit: Holds Add , Delete, future Cut / Copy / Paste
    widgetGroupEdit = new QWidget(widgetToolBar);
    widgetGroupEdit->hide();
    widgetGroupEdit->setObjectName(QStringLiteral("widgetGroupEdit"));
    widgetGroupEdit->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutEdit = new QHBoxLayout(widgetGroupEdit);
        toolbarLayoutEdit->setSpacing(5);
        toolbarLayoutEdit->setContentsMargins(0, 0, 0, 0);

        buttonsGroupEdit = new QButtonGroup();
        buttonsGroupEdit->setExclusive(false);
        connect(buttonsGroupEdit, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupEditClicked(int)));

        buttonAdd = createToolBarButton(QStringLiteral("buttonAddThing"), Advisor_Info::Add_Entity, c_button_size_w + 14, c_button_size_h, false, true);
        buttonsGroupEdit->addButton(buttonAdd, int(Buttons_Edit::Add));
        toolbarLayoutEdit->addWidget(buttonAdd);

        tool = createToolBarButton(QStringLiteral("buttonDuplicate"), Advisor_Info::Duplicate, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupEdit->addButton(tool, int(Buttons_Edit::Duplicate));
        toolbarLayoutEdit->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonDeleteThing"), Advisor_Info::Trash_Can, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupEdit->addButton(tool, int(Buttons_Edit::Delete));
        toolbarLayoutEdit->addWidget(tool);


    // ***** Mode "Editor" Add-On, Layering: Holds buttons that send Things to Front / Back
    widgetGroupLayering = new QWidget(widgetToolBar);
    widgetGroupLayering->hide();
    widgetGroupLayering->setObjectName(QStringLiteral("widgetGroupLayering"));
    widgetGroupLayering->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutLayering = new QHBoxLayout(widgetGroupLayering);
        toolbarLayoutLayering->setSpacing(1);
        toolbarLayoutLayering->setContentsMargins(0, 0, 0, 0);

        buttonsGroupLayering = new QButtonGroup();
        buttonsGroupLayering->setExclusive(false);
        connect(buttonsGroupLayering, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupLayeringClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonSendToBack"), Advisor_Info::Send_to_Back, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_To_Back));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSendBackward"), Advisor_Info::Send_Backward, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_Backward));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSendForward"), Advisor_Info::Send_Forward, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_Forward));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSendToFront"), Advisor_Info::Send_to_Front, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_To_Front));
        toolbarLayoutLayering->addWidget(tool);

    // ***** Mode "Editor" Add-On, Transform: Holds buttons that Flip / Rotate things
    widgetGroupTransform = new QWidget(widgetToolBar);
    widgetGroupTransform->hide();
    widgetGroupTransform->setObjectName(QStringLiteral("widgetGroupTransform"));
    widgetGroupTransform->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutTransform = new QHBoxLayout(widgetGroupTransform);
        toolbarLayoutTransform->setSpacing(1);
        toolbarLayoutTransform->setContentsMargins(2, 0, 0, 0);

        buttonsGroupTransform = new QButtonGroup();
        buttonsGroupTransform->setExclusive(false);
        connect(buttonsGroupTransform, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupTransformClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonResetObject"), Advisor_Info::Reset, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Reset_Object));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformFlipH"), Advisor_Info::Flip_H, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Flip_H));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformFlipV"), Advisor_Info::Flip_V, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Flip_V));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformRotateL"), Advisor_Info::Rotate_L, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Rotate_L));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformRotateR"), Advisor_Info::Rotate_R, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Rotate_R));
        toolbarLayoutTransform->addWidget(tool);


    // ***** Mode "Editor" Add-On, Grid: Holds buttons that control Snap to Grid, Resize to Grid, Show Grid on Top
    widgetGroupGridFull = new QWidget(widgetToolBar);
    widgetGroupGridFull->hide();
    widgetGroupGridFull->setObjectName(QStringLiteral("widgetGroupGridFull"));
    widgetGroupGridFull->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutGridFull = new QHBoxLayout(widgetGroupGridFull);
        toolbarLayoutGridFull->setSpacing(0);
        toolbarLayoutGridFull->setContentsMargins(0, 0, 0, 0);

        buttonsGroupGridFull = new QButtonGroup();
        buttonsGroupGridFull->setExclusive(false);
        connect(buttonsGroupGridFull, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupGridFullClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonGridOnTop"), Advisor_Info::Grid_Show_On_Top, c_button_size_w, c_button_size_h, true);
        buttonsGroupGridFull->addButton(tool, int(Buttons_Grid::Grid_On_Top));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Grid_On_Top).toBool());
        toolbarLayoutGridFull->addWidget(tool);
        toolbarLayoutGridFull->addSpacing(1);

        tool = createToolBarButton(QStringLiteral("buttonResizeToGrid"), Advisor_Info::Resize_To_Grid, c_button_size_w, c_button_size_h, true);
        buttonsGroupGridFull->addButton(tool, int(Buttons_Grid::Resize_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Resize_To_Grid).toBool());
        toolbarLayoutGridFull->addWidget(tool);
        toolbarLayoutGridFull->addSpacing(1);

        tool = createToolBarButton(QStringLiteral("buttonSnapToGrid"), Advisor_Info::Grid_Snap_To_Grid, c_button_size_w, c_button_size_h, true);
        buttonsGroupGridFull->addButton(tool, int(Buttons_Grid::Snap_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool());
        toolbarLayoutGridFull->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSnapOptions"), Advisor_Info::Grid_Snap_Options, 14, c_button_size_h, false, true);
        buttonsGroupGridFull->addButton(tool, int(Buttons_Grid::Snap_Options));
        toolbarLayoutGridFull->addWidget(tool);

    // ***** Mode "World Map" Add-On, Grid: Holds button that controls Snap to Grid
    widgetGroupGridSimple = new QWidget(widgetToolBar);
    widgetGroupGridSimple->hide();
    widgetGroupGridSimple->setObjectName(QStringLiteral("widgetGroupGridSimple"));
    widgetGroupGridSimple->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutGridSimple = new QHBoxLayout(widgetGroupGridSimple);
        toolbarLayoutGridSimple->setSpacing(0);
        toolbarLayoutGridSimple->setContentsMargins(0, 0, 0, 0);

        buttonsGroupGridSimple = new QButtonGroup();
        buttonsGroupGridSimple->setExclusive(false);
        connect(buttonsGroupGridSimple, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupGridSimpleClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonSnapToMap"), Advisor_Info::Grid_Snap_To_Grid, c_button_size_w, c_button_size_h, true);
        buttonsGroupGridSimple->addButton(tool, int(Buttons_Grid::Snap_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool());
        toolbarLayoutGridSimple->addWidget(tool);
        toolbarLayoutGridSimple->addWidget(createToolBarSpacer(c_button_size_h - 2, 34, false));


    // ***** Mode "Editor" Add-On, Play: Holds buttons that starts game Engine window
    widgetGroupPlay = new QWidget(widgetToolBar);
    widgetGroupPlay->hide();
    widgetGroupPlay->setObjectName(QStringLiteral("widgetGroupPlay"));
    widgetGroupPlay->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutPlay = new QHBoxLayout(widgetGroupPlay);
        toolbarLayoutPlay->setSpacing(5);
        toolbarLayoutPlay->setContentsMargins(0, 0, 0, 0);

        buttonsGroupPlay = new QButtonGroup();
        buttonsGroupPlay->setExclusive(false);
        connect(buttonsGroupPlay, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupPlayClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonPlay"), Advisor_Info::Play_Game, c_button_size_w + 14, c_button_size_h, false, true);
        buttonsGroupPlay->addButton(tool, int(Buttons_Play::Play));
        toolbarLayoutPlay->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonPlayStage"), Advisor_Info::Play_Stage, c_button_size_w, c_button_size_h, false, true);
        buttonsGroupPlay->addButton(tool, int(Buttons_Play::Play_Stage));
        toolbarLayoutPlay->addWidget(tool);

        toolbarLayoutPlay->addWidget(createToolBarSpacer(c_button_size_h - 2, 34, false));


    // ***** Settings Buttons
    widgetGroupSettings = new QWidget(widgetToolBar);
    widgetGroupSettings->hide();
    widgetGroupSettings->setObjectName(QStringLiteral("widgetGroupSettings"));
    widgetGroupSettings->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutSettings = new QHBoxLayout(widgetGroupSettings);
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

        tool = createToolBarButton(QStringLiteral("buttonSoundFiles"), Advisor_Info::Settings_Sound_Files, c_button_size_w, c_button_size_h, false);
        toolbarLayoutSettings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            FormSound *sound_files = new FormSound(m_project, this);
            sound_files->show();
        });

        tool = createToolBarButton(QStringLiteral("buttonSettingsEditor"), Advisor_Info::Settings_Manager, c_button_size_w, c_button_size_h, false);
        toolbarLayoutSettings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            FormSettings *settings_editor = new FormSettings(m_project, this);
            settings_editor->show();
        });


    // ********** Set up initial toolbar
    // Clears the containers that keeps track of whats added to the toolbar layout
    toolbarWidgets.clear();
    toolbarSpacers.clear();

    // Set the initial toolbar as the mode buttons only
    widgetToolBarLayout->addWidget(widgetGroupMode);
    widgetToolBarLayout->addStretch();

    // Add the toolbar onto FormMain
    toolbar->addWidget(widgetToolBar);
    widgetToolBar->setFixedWidth( this->width() );
    this->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolbar);
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








