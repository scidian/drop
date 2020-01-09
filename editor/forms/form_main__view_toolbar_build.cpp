//
//      Created by Stephens Nunnally on 1/7/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>
#include <QDebug>
#include <QMenu>
#include <QSlider>
#include <QSpinBox>

#include "editor/event_filters.h"
#include "editor/forms/form_main.h"
#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "editor/style/style.h"
#include "editor/view/editor_view.h"
#include "editor/widgets/widgets.h"


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
            toolbarLayoutZoom->setContentsMargins(3, 0, 0, 0);

            DrQSpinSlot *zoom_spin =   new DrQSpinSlot();
            QPushButton *drop_button = new QPushButton();
            DrQSlider   *zoom_slider = new DrQSlider();

            // Zoom Spin Box
            zoom_spin->setObjectName(QStringLiteral("zoomSpin"));
            zoom_spin->setFixedHeight(22);
            zoom_spin->setFont(Dr::CustomFont());
            zoom_spin->setAttribute(Qt::WA_MacShowFocusRect, 0);
            zoom_spin->setRange(1, 3200);
            zoom_spin->setSingleStep(50);
            zoom_spin->setSuffix("%");
            zoom_spin->setValue(50);
            int max_width = Dr::CheckFontWidth(Dr::CustomFont(), "3200%") + 20;
            zoom_spin->setFixedWidth(max_width);
            zoom_spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
                connect (zoom_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this] (int i) {
                    if (this->getStageView() != nullptr)
                        this->getStageView()->zoomToScale(static_cast<double>(i / 100.0));
                    static int count = 0;
                    qDebug() << "Zoom Spin Signal: " << count++;
                });
                connect(viewEditor, SIGNAL(zoomSpinUpdate(int)), zoom_spin, SLOT(updateValue(int)));
            toolbarLayoutZoom->addWidget(zoom_spin);

            // Zoom Spin Box Drop Down Menu Button
            drop_button->setObjectName(QStringLiteral("spinBoxSelect"));
            drop_button->setFont(Dr::CustomFont());
            drop_button->setFixedWidth(23);
                QMenu *menu = new QMenu(this);
                    menu->setObjectName(QStringLiteral("menuComboBox"));
                    menu->setMinimumWidth(130);
                QActionGroup *group;
                    group = new QActionGroup(menu);
                    group->setExclusive(true);
                QStringList options { "25", "50", "100", "200", "400", "800", "1600", "3200" };
                    for (auto string : options) {
                        QString action_text = string + tr("%");
                        if (string == "50") action_text += " - (default)";
                        // Add spaces in front of short numbers
                        for (int add_space = string.length(); add_space < 4; ++add_space) {
                            action_text = "   " + action_text;
                        }
                        // Add each option to menu
                        QAction *action = new QAction(action_text);
                        action->setCheckable(false);
                        action->setProperty(User_Property::Integer, QVariant::fromValue(string.toInt()));
                        group->addAction(action);
                        menu->addAction(action);
                        connect(action, &QAction::triggered, [this, action]() {
                            if (this->getStageView() != nullptr)
                                this->getStageView()->zoomToScale(static_cast<double>(action->property(User_Property::Integer).toInt()) / 100.0);
                            static int count = 0;
                            qDebug() << "Zoom Menu Signal: " << count++;
                        });
                    }
            drop_button->setMenu(menu);
            menu->installEventFilter(new DrFilterPopUpMenuRelocater(menu, 2, -max_width));
            toolbarLayoutZoom->addWidget(drop_button);

            // Zoom Slider
            zoom_slider->setRange(-5, 50);
            zoom_slider->setFixedWidth(165);
            zoom_slider->setMaximumHeight(26);
            zoom_slider->setValue(20);
            zoom_slider->setTickInterval(5);
            zoom_slider->setSingleStep(1);
            zoom_slider->setTickPosition(QSlider::TickPosition::TicksAbove);
            zoom_slider->setTickColor(QColor("#505050"));
            zoom_slider->setOrientation(Qt::Orientation::Horizontal);
                connect(zoom_slider, &QSlider::valueChanged, this, [this, zoom_slider] () {
                    if (this->getStageView() != nullptr)
                        this->getStageView()->zoomToPower(zoom_slider->value() * 10);
                    static int count = 0;
                    qDebug() << "Zoom Slider Signal: " << count++;
                });
                connect(viewEditor, SIGNAL(zoomSliderUpdate(int)), zoom_slider, SLOT(updateValue(int)));
            toolbarLayoutZoom->addSpacing(9);
            toolbarLayoutZoom->addWidget(zoom_slider);
            toolbarLayoutZoom->addStretch();


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


















