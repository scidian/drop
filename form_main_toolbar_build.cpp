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
#include "form_main.h"
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
    toolbar = new DrToolBar(this);
    toolbar->setObjectName(QStringLiteral("toolbar"));
    toolbar->setFixedHeight(44);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);

    // ***** This is a container object that holds all toolbar buttons, allowing us to put them in a layout
    widgetToolbar = new QWidget();
    widgetToolbar->setObjectName(QStringLiteral("widgetToolbar"));
    widgetToolbar->setFixedHeight(46);
    widgetToolbarLayout = new QHBoxLayout(widgetToolbar);
    widgetToolbarLayout->setSpacing(3);
    widgetToolbarLayout->setContentsMargins(12, 0, 12, 0);

    // ***** Holds which mode we are in: World Editor, World Map, UI Editor
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
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Mode_Map);
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendBackward"), 34, 26, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_Backward));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Mode_Editor);
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendForward"), 34, 26, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_Forward));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Mode_UI);
        toolbarLayoutLayering->addWidget(tool);

        tool = createToolbarButton(QStringLiteral("buttonSendToFront"), 34, 26, false);
        buttonsGroupLayering->addButton(tool, int(Buttons_Layering::Send_To_Front));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Mode_UI);
        toolbarLayoutLayering->addWidget(tool);

    // ***** Mode "Editor" Add-On, Layering: Holds buttons that send objects to front / back
    widgetGroupReset = new QWidget(widgetToolbar);
    widgetGroupReset->hide();
    widgetGroupReset->setObjectName(QStringLiteral("widgetGroupReset"));
    widgetGroupReset->setFixedHeight(46);
        QHBoxLayout *toolbarLayoutReset = new QHBoxLayout(widgetGroupReset);
        toolbarLayoutReset->setSpacing(1);
        toolbarLayoutReset->setContentsMargins(0, 0, 0, 0);

        buttonsGroupReset = new QButtonGroup();
        buttonsGroupReset->setExclusive(true);
        connect(buttonsGroupReset, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupResetClicked(int)));

        tool = createToolbarButton(QStringLiteral("buttonResetObject"), 34, 26, false);
        buttonsGroupReset->addButton(tool, int(Buttons_Reset::Reset_Object));
        m_widget_hover->attachToHoverHandler(tool, Advisor_Info::Mode_Map);
        toolbarLayoutReset->addWidget(tool);


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







