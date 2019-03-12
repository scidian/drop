//
//      Created by Stephens Nunnally on 3/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>
#include <QDockWidget>
#include <QToolBar>
#include <QToolButton>

#include "form_main.h"
#include "globals.h"
#include "library.h"


//####################################################################################
//##    DrToolBar Function Calls
//####################################################################################
DrToolBar::~DrToolBar() { }

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
//##    buttonGroupEditor SLOT and functions
//####################################################################################
void FormMain::buttonGroupEditorClicked(int id)
{
    buttonGroupEditorSetChecked(id);

    Form_Main_Mode new_id =     static_cast<Form_Main_Mode>(id);
    Form_Main_Mode current_id = static_cast<Form_Main_Mode>(Dr::GetPreference(Preferences::Form_Main_Mode).toInt());
    if (current_id == new_id) return;

    if (new_id == Form_Main_Mode::World_Map) {
        this->buildWindow(Form_Main_Mode::Clear);
    } else if (new_id == Form_Main_Mode::World_Editor) {
        this->buildWindow(Form_Main_Mode::World_Editor);
    } else if (new_id == Form_Main_Mode::UI_Editor) {
        this->buildWindow(Form_Main_Mode::Clear);
    }
}

void FormMain::buttonGroupEditorSetChecked(int id)
{
    QList<QAbstractButton*> buttons = buttonGroupEditor->buttons();
    for (auto button : buttons) {
        bool is_button = (buttonGroupEditor->button(id) == button);
        button->setChecked(is_button);
        button->setDown(!is_button);
    }
}


//####################################################################################
//##    Builds FormMain toolbar for WorldEditor mode
//####################################################################################
DrToolBar* FormMain::buildWindowModeWorldEditorToolbar()
{
    // Widgets to use during building
    QPushButton *button;
    QToolButton *tool;

    // Some fonts to use
    QFont font, fontLarger;
    font.setPointSize(Dr::FontSize());
    fontLarger.setPointSize(Dr::FontSize() + 2);

    // Initialize toolbar widget
    DrToolBar *toolbar = new DrToolBar(this);
    toolbar->setObjectName(QStringLiteral("toolbar"));
    toolbar->setFixedHeight(44);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);

    // This is a container object that holds all toolbar buttons, allowing us to put them in a layout
    widgetToolbar = new QWidget();
    widgetToolbar->setObjectName(QStringLiteral("widgetToolbar"));
    widgetToolbar->setFixedHeight(46);
    QHBoxLayout *toolbarLayout = new QHBoxLayout(widgetToolbar);
    toolbarLayout->setSpacing(3);
    toolbarLayout->setContentsMargins(12, 0, 12, 0);

        // ***** Holds which mode we are in: World Editor, World Map, UI Editor
        buttonGroupEditor = new QButtonGroup(this);
        buttonGroupEditor->setExclusive(true);
        connect(buttonGroupEditor, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupEditorClicked(int)));

        tool = createToolbarButtonCheckable(QStringLiteral("buttonModeWorldMap"));
        buttonGroupEditor->addButton(tool, int(Form_Main_Mode::World_Map));
        toolbarLayout->addWidget(tool);
        toolbarLayout->addWidget(createToolbarSpacer());

        tool = createToolbarButtonCheckable(QStringLiteral("buttonModeWorldEdit"));
        buttonGroupEditor->addButton(tool, int(Form_Main_Mode::World_Editor));
        toolbarLayout->addWidget(tool);
        toolbarLayout->addWidget(createToolbarSpacer());

        tool = createToolbarButtonCheckable(QStringLiteral("buttonModeUIEdit"));
        buttonGroupEditor->addButton(tool, int(Form_Main_Mode::UI_Editor));
        toolbarLayout->addWidget(tool);




        toolbarLayout->addStretch();

        button = new QPushButton();
        button->setObjectName(QStringLiteral("toolbarButton"));
        button->setFont(font);
        button->setFixedSize(80, 24);
        //Dr::ApplyDropShadowByType(button,    Shadow_Types::Button_Shadow);
        button->setText( tr("Atlases") );
        toolbarLayout->addWidget(button);

    toolbar->addWidget(widgetToolbar);
    widgetToolbar->setFixedWidth( this->width() );


    return toolbar;
}


QToolButton* FormMain::createToolbarButtonCheckable(const QString &name)
{
    QToolButton *tool = new QToolButton();
    tool->setObjectName( name );
    tool->setCheckable(true);
    tool->setChecked(false);
    tool->setFixedSize(38, 36);
    return tool;
}

QLabel* FormMain::createToolbarSpacer()
{
    QLabel *spacer = new QLabel();
    spacer->setObjectName(QStringLiteral("labelSpacer"));
    spacer->setFixedSize( 1, 24 );
    return spacer;
}











