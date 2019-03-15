//
//      Created by Stephens Nunnally on 3/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>
#include <QToolBar>
#include <QToolButton>

#include "editor_tree_widgets.h"
#include "form_main.h"
#include "globals.h"
#include "library.h"

enum class Buttons_Layering {
    Send_To_Back,
    Send_Backward,
    Send_To_Front,
    Send_Forward,
};

//####################################################################################
//##        Constructor / Destructor
//####################################################################################
DrToolBar::DrToolBar(QWidget *parent) : QToolBar(parent) { }
DrToolBar::~DrToolBar() { }


//####################################################################################
//##    DrToolBar Function Calls
//####################################################################################
// Save the press position (this is relative to the current widget)
void DrToolBar::mousePressEvent(QMouseEvent* event) {
    pressPos= event->pos();
    isMoving= true;
}

// Calculate difference between the press position and the new Mouse position, relative to the current widget
void DrToolBar::mouseMoveEvent(QMouseEvent* event) {
    if (isMoving) {
        QPoint diff = event->pos() - pressPos;
        window()->move( window()->pos() + diff );
    }
}
void DrToolBar::mouseReleaseEvent(QMouseEvent*) { isMoving = false; }


//####################################################################################
//##    buttonGroupMode SLOT and functions
//####################################################################################
void FormMain::buttonGroupModeClicked(int id)
{
    buttonGroupModeSetChecked(id);

    Form_Main_Mode new_id =     static_cast<Form_Main_Mode>(id);
    Form_Main_Mode current_id = static_cast<Form_Main_Mode>(Dr::GetPreference(Preferences::Form_Main_Mode).toInt());
    if (current_id == new_id) return;

    if (new_id == Form_Main_Mode::World_Map) {
        this->setFormMainMode(Form_Main_Mode::Clear);
    } else if (new_id == Form_Main_Mode::World_Editor) {
        this->setFormMainMode(Form_Main_Mode::World_Editor);
    } else if (new_id == Form_Main_Mode::UI_Editor) {
        this->setFormMainMode(Form_Main_Mode::Clear);
    }
}

void FormMain::buttonGroupModeSetChecked(int id)
{
    QList<QAbstractButton*> buttons = buttonsGroupMode->buttons();
    for (auto button : buttons) {
        bool is_button = (buttonsGroupMode->button(id) == button);
        button->setChecked(is_button);
        button->setDown(!is_button);
    }
}


//####################################################################################
//##    buttonGroupLayering SLOT and functions
//####################################################################################
void FormMain::buttonGroupLayeringClicked(int id)
{
    Buttons_Layering clicked = static_cast<Buttons_Layering>(id);

    if (clicked == Buttons_Layering::Send_To_Back) {

    }
}


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
//##    Clears and recreates the toolbar based on the new mode
//####################################################################################
void FormMain::clearToolbar()
{
    for (auto widget : toolbarWidgets) {
        widgetToolbarLayout->removeWidget( widget );
        widget->hide();
    }
    // Use iterator to delete items in list
    for(auto it = toolbarSpacers.begin(); it != toolbarSpacers.end(); ) {
        widgetToolbarLayout->removeItem( *it );
        it = toolbarSpacers.erase(it);
    }
}

void FormMain::setToolbar(Form_Main_Mode new_mode)
{
    switch (new_mode) {
    case Form_Main_Mode::World_Editor:
        addToolbarGroup( widgetGroupLayering, true );

//        for (auto button : buttonsGroupLayering->buttons() ) {
//            button->setEnabled(false);
//        }

        break;

    case Form_Main_Mode::Clear:
        break;

    default:    ;
    }
}

void FormMain::addToolbarGroup(QWidget *group, bool add_spacer)
{
    toolbarWidgets.append(group);
    widgetToolbarLayout->addWidget(group);
    group->show();

    if (add_spacer) {
        QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
        toolbarSpacers.append(spacer);
        widgetToolbarLayout->addSpacerItem(spacer);
    }
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

QLabel* FormMain::createToolbarSpacer()
{
    QLabel *spacer = new QLabel();
    spacer->setObjectName(QStringLiteral("labelSpacer"));
    spacer->setFixedSize( 1, 24 );
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











