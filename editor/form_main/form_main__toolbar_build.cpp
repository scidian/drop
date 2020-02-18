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
#include "editor/event_filters.h"
#include "editor/form_main/form_main.h"
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
//##    Builds FormMain Toolbar
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
    widgetToolbar = new QWidget();
    widgetToolbar->setObjectName(QStringLiteral("widgetToolbar"));
    widgetToolbar->setFixedHeight(c_toolbar_height);
    widgetToolbarLayout = new QHBoxLayout(widgetToolbar);
    widgetToolbarLayout->setSpacing(3);
    widgetToolbarLayout->setContentsMargins(12, 0, 12, 0);

    // ***** Selectable Button group that keeps track of which mode we are in: World Editor, World Map, UI Editor
    widgetGroupMode = new QWidget();
    widgetGroupMode->setObjectName(QStringLiteral("widgetGroupMode"));
    widgetGroupMode->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutMode = new QHBoxLayout(widgetGroupMode);
        toolbarLayoutMode->setSpacing(3);
        toolbarLayoutMode->setContentsMargins(0, 1, 0, 0);

        buttonsGroupMode = new QButtonGroup();
        buttonsGroupMode->setExclusive(true);
        connect(buttonsGroupMode, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupModeClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonModeWorldMap"), Advisor_Info::Mode_Map, c_button_size_w + 4, c_button_size_h + 6, true);
        buttonsGroupMode->addButton(tool, int(Form_Main_Mode::World_Map));
        toolbarLayoutMode->addWidget(tool);
        toolbarLayoutMode->addWidget(createToolbarSpacer(c_button_size_h - 2));

        tool = createToolbarButton(QStringLiteral("buttonModeWorldEdit"), Advisor_Info::Mode_Editor, c_button_size_w + 4, c_button_size_h + 6, true);
        buttonsGroupMode->addButton(tool, int(Form_Main_Mode::World_Editor));
        toolbarLayoutMode->addWidget(tool);
        toolbarLayoutMode->addWidget(createToolbarSpacer(c_button_size_h - 2));

        tool = createToolbarButton(QStringLiteral("buttonModeUIEdit"), Advisor_Info::Mode_UI, c_button_size_w + 4, c_button_size_h + 6, true);
        buttonsGroupMode->addButton(tool, int(Form_Main_Mode::UI_Editor));
        toolbarLayoutMode->addWidget(tool);


    // ***** Mode "Editor" Add-On, Edit: Holds Add , Delete, future Cut / Copy / Paste
    widgetGroupEdit = new QWidget(widgetToolbar);
    widgetGroupEdit->hide();
    widgetGroupEdit->setObjectName(QStringLiteral("widgetGroupEdit"));
    widgetGroupEdit->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutEdit = new QHBoxLayout(widgetGroupEdit);
        toolbarLayoutEdit->setSpacing(5);
        toolbarLayoutEdit->setContentsMargins(0, 0, 0, 0);

        buttonsGroupEdit = new QButtonGroup();
        buttonsGroupEdit->setExclusive(false);
        connect(buttonsGroupEdit, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupEditClicked(int)));

        buttonAdd = createToolbarButton(QStringLiteral("buttonAddThing"), Advisor_Info::Add_Entity, c_button_size_w + 14, c_button_size_h, false, true);
        buttonsGroupEdit->addButton(buttonAdd, int(Buttons_Edit::Add));
        toolbarLayoutEdit->addWidget(buttonAdd);

        tool = createToolbarButton(QStringLiteral("buttonDuplicate"), Advisor_Info::Duplicate, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupEdit->addButton(tool, int(Buttons_Edit::Duplicate));
        toolbarLayoutEdit->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonDeleteThing"), Advisor_Info::Trash_Can, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupEdit->addButton(tool, int(Buttons_Edit::Delete));
        toolbarLayoutEdit->addWidget(tool);


    // ***** Mode "Editor" Add-On, Layering: Holds buttons that send Things to Front / Back
    widgetGroupLayering = new QWidget(widgetToolbar);
    widgetGroupLayering->hide();
    widgetGroupLayering->setObjectName(QStringLiteral("widgetGroupLayering"));
    widgetGroupLayering->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutLayering = new QHBoxLayout(widgetGroupLayering);
        toolbarLayoutLayering->setSpacing(1);
        toolbarLayoutLayering->setContentsMargins(0, 0, 0, 0);

        buttonsGroupLayering = new QButtonGroup();
        buttonsGroupLayering->setExclusive(false);
        connect(buttonsGroupLayering, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupLayeringClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonSendToBack"), Advisor_Info::Send_to_Back, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_To_Back));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendBackward"), Advisor_Info::Send_Backward, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_Backward));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendForward"), Advisor_Info::Send_Forward, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_Forward));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendToFront"), Advisor_Info::Send_to_Front, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_To_Front));
        toolbarLayoutLayering->addWidget(tool);

    // ***** Mode "Editor" Add-On, Transform: Holds buttons that Flip / Rotate things
    widgetGroupTransform = new QWidget(widgetToolbar);
    widgetGroupTransform->hide();
    widgetGroupTransform->setObjectName(QStringLiteral("widgetGroupTransform"));
    widgetGroupTransform->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutTransform = new QHBoxLayout(widgetGroupTransform);
        toolbarLayoutTransform->setSpacing(1);
        toolbarLayoutTransform->setContentsMargins(2, 0, 0, 0);

        buttonsGroupTransform = new QButtonGroup();
        buttonsGroupTransform->setExclusive(false);
        connect(buttonsGroupTransform, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupTransformClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonResetObject"), Advisor_Info::Reset, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Reset_Object));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonTransformFlipH"), Advisor_Info::Flip_H, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Flip_H));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonTransformFlipV"), Advisor_Info::Flip_V, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Flip_V));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonTransformRotateL"), Advisor_Info::Rotate_L, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Rotate_L));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonTransformRotateR"), Advisor_Info::Rotate_R, c_button_size_w, c_button_size_h, false, false);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Rotate_R));
        toolbarLayoutTransform->addWidget(tool);


    // ***** Mode "Editor" Add-On, Grid: Holds buttons that control Snap to Grid, Resize to Grid, Show Grid on Top
    widgetGroupGrid = new QWidget(widgetToolbar);
    widgetGroupGrid->hide();
    widgetGroupGrid->setObjectName(QStringLiteral("widgetGroupGrid"));
    widgetGroupGrid->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutGrid = new QHBoxLayout(widgetGroupGrid);
        toolbarLayoutGrid->setSpacing(0);
        toolbarLayoutGrid->setContentsMargins(0, 0, 0, 0);

        buttonsGroupGrid = new QButtonGroup();
        buttonsGroupGrid->setExclusive(false);
        connect(buttonsGroupGrid, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupGridClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonGridOnTop"), Advisor_Info::Grid_Show_On_Top, c_button_size_w, c_button_size_h, true);
        buttonsGroupGrid->addButton(tool, int(Buttons_Grid::Grid_On_Top));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Grid_On_Top).toBool());
        toolbarLayoutGrid->addWidget(tool);
        toolbarLayoutGrid->addSpacing(1);

        tool = createToolbarButton(QStringLiteral("buttonResizeToGrid"), Advisor_Info::Resize_To_Grid, c_button_size_w, c_button_size_h, true);
        buttonsGroupGrid->addButton(tool, int(Buttons_Grid::Resize_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Resize_To_Grid).toBool());
        toolbarLayoutGrid->addWidget(tool);
        toolbarLayoutGrid->addSpacing(1);

        tool = createToolbarButton(QStringLiteral("buttonSnapToGrid"), Advisor_Info::Grid_Snap_To_Grid, c_button_size_w, c_button_size_h, true);
        buttonsGroupGrid->addButton(tool, int(Buttons_Grid::Snap_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool());
        toolbarLayoutGrid->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSnapOptions"), Advisor_Info::Grid_Snap_Options, 14, c_button_size_h, false, true);
        buttonsGroupGrid->addButton(tool, int(Buttons_Grid::Snap_Options));
        toolbarLayoutGrid->addWidget(tool);


    // ***** Mode "Editor" Add-On, Play: Holds buttons that starts game Engine window
    widgetGroupPlay = new QWidget(widgetToolbar);
    widgetGroupPlay->hide();
    widgetGroupPlay->setObjectName(QStringLiteral("widgetGroupPlay"));
    widgetGroupPlay->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutPlay = new QHBoxLayout(widgetGroupPlay);
        toolbarLayoutPlay->setSpacing(5);
        toolbarLayoutPlay->setContentsMargins(0, 0, 0, 0);

        buttonsGroupPlay = new QButtonGroup();
        buttonsGroupPlay->setExclusive(false);
        connect(buttonsGroupPlay, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupPlayClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonPlay"), Advisor_Info::Play_Game, c_button_size_w + 14, c_button_size_h, false, true);
        buttonsGroupPlay->addButton(tool, int(Buttons_Play::Play));
        toolbarLayoutPlay->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonPlayStage"), Advisor_Info::Play_Stage, c_button_size_w, c_button_size_h, false, true);
        buttonsGroupPlay->addButton(tool, int(Buttons_Play::Play_Stage));
        toolbarLayoutPlay->addWidget(tool);

        toolbarLayoutPlay->addWidget(createToolbarSpacer(c_button_size_h - 2, 34, false));


    // ***** Settings Buttons
    widgetGroupSettings = new QWidget(widgetToolbar);
    widgetGroupSettings->hide();
    widgetGroupSettings->setObjectName(QStringLiteral("widgetGroupSettings"));
    widgetGroupSettings->setFixedHeight(c_toolbar_height);
        QHBoxLayout *toolbarLayoutSettings = new QHBoxLayout(widgetGroupSettings);
        toolbarLayoutSettings->setSpacing(5);
        toolbarLayoutSettings->setContentsMargins(0, 0, 0, 0);

        // Physics Playground
        ///tool = createToolbarButton(QStringLiteral("buttonPlayground"), Advisor_Info::Settings_Playground, c_button_size_w, c_button_size_h, false);
        ///toolbarLayoutSettings->addWidget(tool);
        ///connect(tool, &QPushButton::clicked, [this] () {
        ///    FormPlayground *playground = new FormPlayground(this);
        ///    playground->show();
        ///});

        tool = createToolbarButton(QStringLiteral("buttonImageViewer"), Advisor_Info::Settings_Image_Viewer, c_button_size_w, c_button_size_h, false);
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

        tool = createToolbarButton(QStringLiteral("buttonFontBuilder"), Advisor_Info::Settings_Font_Builder, c_button_size_w, c_button_size_h, false);
        toolbarLayoutSettings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            ///FormBlank *blank_form = new FormBlank(m_project, this);
            ///blank_form->show();
            FormFonts *font_editor = new FormFonts(m_project, this);
            font_editor->show();
        });

        tool = createToolbarButton(QStringLiteral("buttonSettingsEditor"), Advisor_Info::Settings_Manager, c_button_size_w, c_button_size_h, false);
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
    widgetToolbarLayout->addWidget(widgetGroupMode);
    widgetToolbarLayout->addStretch();

    // Add the toolbar onto FormMain
    toolbar->addWidget(widgetToolbar);
    widgetToolbar->setFixedWidth( this->width() );
    this->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolbar);
}




//####################################################################################
//##    Button creation calls
//####################################################################################
QToolButton* FormMain::createToolbarButton(const QString &style_sheet_name, HeaderBodyList advisor_text, int w, int h, bool checkable, bool enabled) {
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
QLabel* FormMain::createToolbarSpacer(int height, int space_on_the_right, bool visible) {
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








