//
//      Created by Stephens Nunnally on 1/7/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>
#include <QMenu>
#include <QSpinBox>

#include "editor/event_filters.h"
#include "editor/forms/form_main.h"
#include "editor/helper_library.h"
#include "editor/style/style.h"


//####################################################################################
//##    Builds FormMain View Toolbar
//####################################################################################
void FormMain::buildViewToolBar(QWidget *parent) {

    viewToolBar = new QFrame(parent);
    viewToolBar->setObjectName("viewToolBar");
    viewToolBar->setFixedHeight(32);
        QHBoxLayout *view_toolbar_layout = new QHBoxLayout(viewToolBar);
        view_toolbar_layout->setObjectName(QStringLiteral("viewToolBarLayout"));
        view_toolbar_layout->setSpacing(4);
        view_toolbar_layout->setContentsMargins(6, 0, 6, 0);

        // ***** Selectable Button group that keeps track of which mouse mode we are in: Pointer, Hand, Mganify, etc.
        widgetGroupMouse = new QWidget();
        widgetGroupMouse->setObjectName(QStringLiteral("widgetGroupMouse"));
        widgetGroupMouse->setFixedHeight(32);
            QHBoxLayout *toolbarLayoutMouse = new QHBoxLayout(widgetGroupMouse);
            toolbarLayoutMouse->setSpacing(3);
            toolbarLayoutMouse->setContentsMargins(0, 0, 0, 0);

            buttonsGroupMouse = new QButtonGroup();
            buttonsGroupMouse->setExclusive(true);
            connect(buttonsGroupMouse, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupMouseClicked(int)));

            QToolButton *mouse_pointer = createToolbarButton(QStringLiteral("mousePointer"), Advisor_Info::Mouse_Pointer, 24, 24, true);
            mouse_pointer->setChecked(true);
            buttonsGroupMouse->addButton(mouse_pointer, int(Mouse_Mode::Pointer));
            toolbarLayoutMouse->addWidget(mouse_pointer);

            QToolButton *mouse_hand = createToolbarButton(QStringLiteral("mouseHand"), Advisor_Info::Mouse_Hand, 24, 24, true);
            buttonsGroupMouse->addButton(mouse_hand,    int(Mouse_Mode::Hand));
            toolbarLayoutMouse->addWidget(mouse_hand);

            QToolButton *mouse_magnify = createToolbarButton(QStringLiteral("mouseMagnify"), Advisor_Info::Mouse_Magnify, 24, 24, true);
            buttonsGroupMouse->addButton(mouse_magnify, int(Mouse_Mode::Magnify));
            toolbarLayoutMouse->addWidget(mouse_magnify);

        view_toolbar_layout->addWidget(widgetGroupMouse);
        view_toolbar_layout->addSpacing(2);
        view_toolbar_layout->addWidget(createToolbarSpacer(16));


        // ***** Mouse Mode Add-On, Zoom: Current Zoom Percentage Drop Down, Fit To View, Slider
        widgetGroupZoomTool = new QWidget();
        widgetGroupZoomTool->setObjectName(QStringLiteral("widgetGroupZoomTool"));
        widgetGroupZoomTool->setFixedHeight(32);
            QHBoxLayout *toolbarLayoutZoom = new QHBoxLayout(widgetGroupZoomTool);
            toolbarLayoutZoom->setSpacing(0);
            toolbarLayoutZoom->setContentsMargins(4, 0, 0, 0);

            QSpinBox *zoom_spin = new QSpinBox();
                zoom_spin->setObjectName(QStringLiteral("zoomSpin"));
                zoom_spin->setFixedHeight(22);
                zoom_spin->setFont(Dr::CustomFont());
                zoom_spin->setAttribute(Qt::WA_MacShowFocusRect, 0);
                zoom_spin->setRange(1, 3200);
                zoom_spin->setSingleStep(50);
                zoom_spin->setSuffix("%");
                zoom_spin->setValue(100);
                int max_width = Dr::CheckFontWidth(Dr::CustomFont(), "3200%") + 20;
                zoom_spin->setFixedWidth(max_width);

                zoom_spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
                toolbarLayoutZoom->addWidget(zoom_spin);

            QPushButton *drop_button = new QPushButton();
            drop_button->setObjectName(QStringLiteral("spinBoxSelect"));
            drop_button->setFont(Dr::CustomFont());
            drop_button->setFixedWidth(24);

                QStringList options;
                options << tr(" 50%")
                        << tr("100%")
                        << tr("200%")
                        << tr("400%");

                QMenu *menu = new QMenu(this);
                menu->setObjectName(QStringLiteral("menuComboBox"));
                menu->setMinimumWidth(130);

                QActionGroup *group;
                group = new QActionGroup(menu);
                group->setExclusive(true);

                int string_count = 0;
                for (auto string : options) {
                    QAction *action = new QAction(string);
                    group->addAction(action);
                    action->setCheckable(false);
                    menu->addAction(action);

    //                if (property->getValue().toInt() == string_count) {
    //                    action->setChecked(true);
    //                    button->setText(string);
    //                }
                }
                drop_button->setMenu(menu);
                menu->installEventFilter(new DrFilterPopUpMenuRelocater(menu, 2, -max_width));
                toolbarLayoutZoom->addWidget(drop_button);


        widgetGroupZoomTool->hide();
        view_toolbar_layout->addWidget(widgetGroupZoomTool);


        // ***** Toggled View Options, Debug On/Off, etc
        view_toolbar_layout->addStretch();
        widgetGroupToggle = new QWidget(widgetToolbar);
        widgetGroupToggle->setObjectName(QStringLiteral("widgetGroupToggle"));
        widgetGroupToggle->setFixedHeight(32);
            QHBoxLayout *toolbarLayoutToggle = new QHBoxLayout(widgetGroupToggle);
            toolbarLayoutToggle->setSpacing(3);
            toolbarLayoutToggle->setContentsMargins(0, 0, 0, 0);

            buttonsGroupToggle = new QButtonGroup();
            buttonsGroupToggle->setExclusive(false);
            connect(buttonsGroupToggle, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupToggleClicked(int)));

            QToolButton *debug_on_off = createToolbarButton("debugOnOff", Advisor_Info::Debug_On_Off, 23, 23, true);
            Dr::ApplyDropShadowByType(debug_on_off, Shadow_Types::Button_Shadow_Thin);
            buttonsGroupToggle->addButton(debug_on_off, int(Buttons_Toggle::DebugOnOff));
            debug_on_off->setChecked(false);
            toolbarLayoutToggle->addWidget(debug_on_off);

        view_toolbar_layout->addWidget(createToolbarSpacer(16));
        view_toolbar_layout->addSpacing(2);
        view_toolbar_layout->addWidget(widgetGroupToggle);

}


















