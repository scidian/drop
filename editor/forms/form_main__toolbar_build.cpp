//
//      Created by Stephens Nunnally on 3/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>
#include <QMenu>
#include <QRadioButton>
#include <QToolBar>
#include <QToolButton>

#include "editor/event_filters.h"
#include "editor/forms/form_blank.h"
#include "editor/forms/form_fonts.h"
#include "editor/forms/form_main.h"
#include "editor/forms/form_popup.h"
#include "editor/forms/form_settings.h"
#include "editor/globals_editor.h"
#include "editor/helper_editor.h"
#include "editor/style/style.h"
#include "editor/enums_editor.h"
#include "helper.h"


//####################################################################################
//##    Builds FormMain toolbar
//####################################################################################
void FormMain::buildToolBar() {

    // Widgets to use during building
    QToolButton *tool;

    // ***** Initialize toolbar widget
    toolbar = new QToolBar(this);
    toolbar->setObjectName(QStringLiteral("toolbar"));
    toolbar->setFixedHeight(44);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->installEventFilter(new DrFilterClickAndDragWindow(toolbar));

    // ***** This is a container object that holds all toolbar button groups, allowing us to put them in a layout
    widgetToolbar = new QWidget();
    widgetToolbar->setObjectName(QStringLiteral("widgetToolbar"));
    widgetToolbar->setFixedHeight(46);
    widgetToolbarLayout = new QHBoxLayout(widgetToolbar);
    widgetToolbarLayout->setSpacing(3);
    widgetToolbarLayout->setContentsMargins(12, 0, 12, 0);

    // ***** Selectable Button group that keeps track of which mode we are in: World Editor, World Map, UI Editor
    widgetGroupMode = new QWidget();
    widgetGroupMode->setObjectName(QStringLiteral("widgetGroupMode"));
    widgetGroupMode->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutMode = new QHBoxLayout(widgetGroupMode);
        toolbarLayoutMode->setSpacing(3);
        toolbarLayoutMode->setContentsMargins(0, 0, 0, 0);

        buttonsGroupMode = new QButtonGroup();
        buttonsGroupMode->setExclusive(true);
        connect(buttonsGroupMode, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupModeClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonModeWorldMap"), Advisor_Info::Mode_Map, 38, 36, true);
        buttonsGroupMode->addButton(tool, int(Form_Main_Mode::World_Map));
        toolbarLayoutMode->addWidget(tool);
        toolbarLayoutMode->addWidget(createToolbarSpacer());

        tool = createToolbarButton(QStringLiteral("buttonModeWorldEdit"), Advisor_Info::Mode_Editor, 38, 36, true);
        buttonsGroupMode->addButton(tool, int(Form_Main_Mode::World_Editor));
        toolbarLayoutMode->addWidget(tool);
        toolbarLayoutMode->addWidget(createToolbarSpacer());

        tool = createToolbarButton(QStringLiteral("buttonModeUIEdit"), Advisor_Info::Mode_UI, 38, 36, true);
        buttonsGroupMode->addButton(tool, int(Form_Main_Mode::UI_Editor));
        toolbarLayoutMode->addWidget(tool);


    // ***** Mode "Editor" Add-On, Edit: Holds Add , Delete, future Cut / Copy / Paste
    widgetGroupEdit = new QWidget(widgetToolbar);
    widgetGroupEdit->hide();
    widgetGroupEdit->setObjectName(QStringLiteral("widgetGroupEdit"));
    widgetGroupEdit->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutEdit = new QHBoxLayout(widgetGroupEdit);
        toolbarLayoutEdit->setSpacing(5);
        toolbarLayoutEdit->setContentsMargins(0, 0, 0, 0);

        buttonsGroupEdit = new QButtonGroup();
        buttonsGroupEdit->setExclusive(false);
        connect(buttonsGroupEdit, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupEditClicked(int)));

        buttonAdd = createToolbarButton(QStringLiteral("buttonAddThing"), Advisor_Info::Add_Entity, 48, 26, false, true);
        Dr::ApplyDropShadowByType(buttonAdd, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupEdit->addButton(buttonAdd, int(Buttons_Edit::Add));
        toolbarLayoutEdit->addWidget(buttonAdd);

        tool = createToolbarButton(QStringLiteral("buttonDuplicate"), Advisor_Info::Duplicate, 34, 26, false, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupEdit->addButton(tool, int(Buttons_Edit::Duplicate));
        toolbarLayoutEdit->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonDeleteThing"), Advisor_Info::Trash_Can, 34, 26, false, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupEdit->addButton(tool, int(Buttons_Edit::Delete));
        toolbarLayoutEdit->addWidget(tool);


    // ***** Mode "Editor" Add-On, Layering: Holds buttons that send Things to Front / Back
    widgetGroupLayering = new QWidget(widgetToolbar);
    widgetGroupLayering->hide();
    widgetGroupLayering->setObjectName(QStringLiteral("widgetGroupLayering"));
    widgetGroupLayering->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutLayering = new QHBoxLayout(widgetGroupLayering);
        toolbarLayoutLayering->setSpacing(1);
        toolbarLayoutLayering->setContentsMargins(0, 0, 0, 0);

        buttonsGroupLayering = new QButtonGroup();
        buttonsGroupLayering->setExclusive(false);
        connect(buttonsGroupLayering, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupLayeringClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonSendToBack"), Advisor_Info::Send_to_Back, 34, 26, false, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_To_Back));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendBackward"), Advisor_Info::Send_Backward, 34, 26, false, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_Backward));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendForward"), Advisor_Info::Send_Forward, 34, 26, false, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_Forward));
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendToFront"), Advisor_Info::Send_to_Front, 34, 26, false, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_To_Front));
        toolbarLayoutLayering->addWidget(tool);

    // ***** Mode "Editor" Add-On, Transform: Holds buttons that Flip / Rotate things
    widgetGroupTransform = new QWidget(widgetToolbar);
    widgetGroupTransform->hide();
    widgetGroupTransform->setObjectName(QStringLiteral("widgetGroupTransform"));
    widgetGroupTransform->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutTransform = new QHBoxLayout(widgetGroupTransform);
        toolbarLayoutTransform->setSpacing(1);
        toolbarLayoutTransform->setContentsMargins(2, 0, 0, 0);

        buttonsGroupTransform = new QButtonGroup();
        buttonsGroupTransform->setExclusive(false);
        connect(buttonsGroupTransform, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupTransformClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonResetObject"), Advisor_Info::Reset, 34, 26, false, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Reset_Object));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonTransformFlipH"), Advisor_Info::Flip_H, 34, 26, false, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Flip_H));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonTransformFlipV"), Advisor_Info::Flip_V, 34, 26, false, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Flip_V));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonTransformRotateL"), Advisor_Info::Rotate_L, 34, 26, false, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Rotate_L));
        toolbarLayoutTransform->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonTransformRotateR"), Advisor_Info::Rotate_R, 34, 26, false, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupTransform->addButton(tool, int(Buttons_Transform::Rotate_R));
        toolbarLayoutTransform->addWidget(tool);


    // ***** Mode "Editor" Add-On, Grid: Holds buttons that control Snap to Grid, Resize to Grid, Show Grid on Top
    widgetGroupGrid = new QWidget(widgetToolbar);
    widgetGroupGrid->hide();
    widgetGroupGrid->setObjectName(QStringLiteral("widgetGroupGrid"));
    widgetGroupGrid->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutGrid = new QHBoxLayout(widgetGroupGrid);
        toolbarLayoutGrid->setSpacing(0);
        toolbarLayoutGrid->setContentsMargins(0, 0, 0, 0);

        buttonsGroupGrid = new QButtonGroup();
        buttonsGroupGrid->setExclusive(false);
        connect(buttonsGroupGrid, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupGridClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonGridOnTop"), Advisor_Info::Grid_Show_On_Top, 34, 26, true);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupGrid->addButton(tool, int(Buttons_Grid::Grid_On_Top));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Grid_On_Top).toBool());
        toolbarLayoutGrid->addWidget(tool);
        toolbarLayoutGrid->addSpacing(1);

        tool = createToolbarButton(QStringLiteral("buttonResizeToGrid"), Advisor_Info::Resize_To_Grid, 34, 26, true);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupGrid->addButton(tool, int(Buttons_Grid::Resize_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Resize_To_Grid).toBool());
        toolbarLayoutGrid->addWidget(tool);
        toolbarLayoutGrid->addSpacing(1);

        tool = createToolbarButton(QStringLiteral("buttonSnapToGrid"), Advisor_Info::Grid_Snap_To_Grid, 34, 26, true);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupGrid->addButton(tool, int(Buttons_Grid::Snap_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool());
        toolbarLayoutGrid->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSnapOptions"), Advisor_Info::Grid_Snap_Options, 14, 26, false, true);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupGrid->addButton(tool, int(Buttons_Grid::Snap_Options));
        toolbarLayoutGrid->addWidget(tool);


    // ***** Mode "Editor" Add-On, Play: Holds buttons that starts game Engine window
    widgetGroupPlay = new QWidget(widgetToolbar);
    widgetGroupPlay->hide();
    widgetGroupPlay->setObjectName(QStringLiteral("widgetGroupPlay"));
    widgetGroupPlay->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutPlay = new QHBoxLayout(widgetGroupPlay);
        toolbarLayoutPlay->setSpacing(5);
        toolbarLayoutPlay->setContentsMargins(0, 0, 0, 0);

        buttonsGroupPlay = new QButtonGroup();
        buttonsGroupPlay->setExclusive(false);
        connect(buttonsGroupPlay, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupPlayClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonPlay"), Advisor_Info::Play_Game, 48, 26, false, true);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupPlay->addButton(tool, int(Buttons_Play::Play));
        toolbarLayoutPlay->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonPlayStage"), Advisor_Info::Play_Stage, 34, 26, false, true);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        buttonsGroupPlay->addButton(tool, int(Buttons_Play::Play_Stage));
        toolbarLayoutPlay->addWidget(tool);

        toolbarLayoutPlay->addWidget(createToolbarSpacer(24, 34, false));


    // ***** Settings Buttons
    widgetGroupSettings = new QWidget(widgetToolbar);
    widgetGroupSettings->hide();
    widgetGroupSettings->setObjectName(QStringLiteral("widgetGroupSettings"));
    widgetGroupSettings->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutSettings = new QHBoxLayout(widgetGroupSettings);
        toolbarLayoutSettings->setSpacing(5);
        toolbarLayoutSettings->setContentsMargins(0, 0, 0, 0);

        // Physics Playground
        ///tool = createToolbarButton(QStringLiteral("buttonPlayground"), Advisor_Info::Settings_Playground, 34, 26, false);
        ///toolbarLayoutSettings->addWidget(tool);
        ///connect(tool, &QPushButton::clicked, [this] () {
        ///    FormPlayground *playground = new FormPlayground(this);
        ///    playground->show();
        ///});

        tool = createToolbarButton(QStringLiteral("buttonImageViewer"), Advisor_Info::Settings_Image_Viewer, 34, 26, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        toolbarLayoutSettings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            FormBlank *blank_form = new FormBlank(m_project, this);
            blank_form->show();
        });

        tool = createToolbarButton(QStringLiteral("buttonFontBuilder"), Advisor_Info::Settings_Font_Builder, 34, 26, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
        toolbarLayoutSettings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            FormFonts *font_editor = new FormFonts(m_project, this);
            font_editor->show();
        });

        tool = createToolbarButton(QStringLiteral("buttonSettingsEditor"), Advisor_Info::Settings_Manager, 34, 26, false);
        Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
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
    tool->setObjectName( style_sheet_name );
    if (checkable) {
        tool->setCheckable(true);
        tool->setChecked(false);
    }
    tool->setToolTip( advisor_text[0] );
    tool->setEnabled(enabled);
    tool->setFixedSize(w, h);
    m_filter_hover->attachToHoverHandler(tool, advisor_text);
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







