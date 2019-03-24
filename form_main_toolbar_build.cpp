//
//      Created by Stephens Nunnally on 3/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>
#include <QToolBar>
#include <QToolButton>

#include "enums_form_main.h"
#include "form_font_editor.h"
#include "form_main.h"
#include "form_settings.h"
#include "globals.h"
#include "library.h"
#include "widgets_event_filters.h"


//####################################################################################
//##    Builds FormMain toolbar
//####################################################################################
void FormMain::buildToolBar()
{
    // Widgets to use during building
    QToolButton *tool;

    // ***** Initialize toolbar widget
    toolbar = new QToolBar(this);
    toolbar->setObjectName(QStringLiteral("toolbar"));
    toolbar->setFixedHeight(44);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->installEventFilter(new ClickAndDragWindow(toolbar));

    // ***** This is a container object that holds all toolbar buttons, allowing us to put them in a layout
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

        tool = createToolbarButton(QStringLiteral("buttonModeWorldMap"), 38, 36, true);
        buttonsGroupMode->addButton(tool, int(Form_Main_Mode::World_Map));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Mode_Map);
        toolbarLayoutMode->addWidget(tool);
        toolbarLayoutMode->addWidget(createToolbarSpacer());

        tool = createToolbarButton(QStringLiteral("buttonModeWorldEdit"), 38, 36, true);
        buttonsGroupMode->addButton(tool, int(Form_Main_Mode::World_Editor));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Mode_Editor);
        toolbarLayoutMode->addWidget(tool);
        toolbarLayoutMode->addWidget(createToolbarSpacer());

        tool = createToolbarButton(QStringLiteral("buttonModeUIEdit"), 38, 36, true);
        buttonsGroupMode->addButton(tool, int(Form_Main_Mode::UI_Editor));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Mode_UI);
        toolbarLayoutMode->addWidget(tool);


    // ***** Mode "Editor" Add-On, Layering: Holds buttons that send objects to front / back
    widgetGroupLayering = new QWidget(widgetToolbar);
    widgetGroupLayering->hide();
    widgetGroupLayering->setObjectName(QStringLiteral("widgetGroupLayering"));
    widgetGroupLayering->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutLayering = new QHBoxLayout(widgetGroupLayering);
        toolbarLayoutLayering->setSpacing(1);
        toolbarLayoutLayering->setContentsMargins(0, 0, 0, 0);

        buttonsGroupLayering = new QButtonGroup();
        buttonsGroupLayering->setExclusive(true);
        connect(buttonsGroupLayering, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupLayeringClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonSendToBack"), 34, 26, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_To_Back));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Not_Set);
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendBackward"), 34, 26, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_Backward));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Not_Set);
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendForward"), 34, 26, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_Forward));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Not_Set);
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendToFront"), 34, 26, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_To_Front));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Not_Set);
        toolbarLayoutLayering->addWidget(tool);

    // ***** Mode "Editor" Add-On, Reset: Holds button that resets object
    widgetGroupReset = new QWidget(widgetToolbar);
    widgetGroupReset->hide();
    widgetGroupReset->setObjectName(QStringLiteral("widgetGroupReset"));
    widgetGroupReset->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutReset = new QHBoxLayout(widgetGroupReset);
        toolbarLayoutReset->setSpacing(5);
        toolbarLayoutReset->setContentsMargins(0, 0, 0, 0);

        buttonsGroupReset = new QButtonGroup();
        buttonsGroupReset->setExclusive(false);
        connect(buttonsGroupReset, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupResetClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonResetObject"), 34, 26, false);
        buttonsGroupReset->addButton(tool, int(Buttons_Reset::Reset_Object));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Not_Set);
        toolbarLayoutReset->addWidget(tool);

    // ***** Mode "Editor" Add-On, Grid: Holds buttons that control snapo to grid, resize to grid, show grid on top
    widgetGroupGrid = new QWidget(widgetToolbar);
    widgetGroupGrid->hide();
    widgetGroupGrid->setObjectName(QStringLiteral("widgetGroupGrid"));
    widgetGroupGrid->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutGrid = new QHBoxLayout(widgetGroupGrid);
        toolbarLayoutGrid->setSpacing(1);
        toolbarLayoutGrid->setContentsMargins(0, 0, 0, 0);

        buttonsGroupGrid = new QButtonGroup();
        buttonsGroupGrid->setExclusive(false);
        connect(buttonsGroupGrid, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupGridClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonSnapToGrid"), 34, 26, true);
        buttonsGroupGrid->addButton(tool, int(Buttons_Grid::Snap_To_Grid));
        tool->setChecked(!Dr::GetPreference(Preferences::World_Editor_Grid_On_Top).toBool());
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Grid_Snap_To_Grid);
        toolbarLayoutGrid->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonResizeToGrid"), 34, 26, true);
        buttonsGroupGrid->addButton(tool, int(Buttons_Grid::Resize_To_Grid));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Not_Set);
        toolbarLayoutGrid->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonGridOnTop"), 34, 26, true);
        buttonsGroupGrid->addButton(tool, int(Buttons_Grid::Grid_On_Top));
        tool->setChecked(!Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool());
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Grid_Show_On_Top);
        toolbarLayoutGrid->addWidget(tool);







    // ***** Settings Buttons
    widgetGroupSettings = new QWidget(widgetToolbar);
    widgetGroupSettings->hide();
    widgetGroupSettings->setObjectName(QStringLiteral("widgetGroupSettings"));
    widgetGroupSettings->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutSettings = new QHBoxLayout(widgetGroupSettings);
        toolbarLayoutSettings->setSpacing(5);
        toolbarLayoutSettings->setContentsMargins(0, 0, 0, 0);

        tool = createToolbarButton(QStringLiteral("buttonFontEditor"), 34, 26, false);
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Settings_Font_Editor);
        toolbarLayoutSettings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            FormFontEditor *font_editor = new FormFontEditor(project, this);
            font_editor->show();
        });

        tool = createToolbarButton(QStringLiteral("buttonSettingsEditor"), 34, 26, false);
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Settings_Editor);
        toolbarLayoutSettings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            FormSettings *settings_editor = new FormSettings(project, this);
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
QToolButton* FormMain::createToolbarButton(const QString &style_sheet_name, int w, int h, bool checkable)
{
    QToolButton *tool = new QToolButton();
    tool->setObjectName( style_sheet_name );
    tool->setCheckable(checkable);
    tool->setChecked(false);
    tool->setFixedSize(w, h);
    return tool;
}

QLabel* FormMain::createToolbarSpacer(int height, int space_on_the_right)   // DEFAULTS: height = 24, space_on_the_right = 1
{
    QLabel *spacer = new QLabel();
    spacer->setObjectName(QStringLiteral("labelSpacer"));
    spacer->setFixedSize( space_on_the_right, height );
    return spacer;
}

QPushButton* FormMain::createPushButton(QString name, QString text)
{
    QFont font;
    font.setPointSize(Dr::FontSize());

    QPushButton *button = new QPushButton();
    button->setObjectName( name );
    button->setFont(font);
    button->setFixedSize(80, 24);
    Dr::ApplyDropShadowByType(button,    Shadow_Types::Button_Shadow);
    button->setText( text );
    return button;
}







