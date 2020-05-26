//
//      Created by Stephens Nunnally on 5/23/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>

#include "core/colors/colors.h"
#include "core/imaging/imaging.h"
#include "editor/form_main/form_main.h"
#include "editor/event_filters/event_filters.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/view_editor/editor_view.h"
#include "editor/view_node/node_view.h"
#include "editor/widgets/widgets_view.h"
#include "editor/widgets/widgets_inspector.h"
#include "editor/widgets/widgets_view_toolbar.h"

// Local Constants
const int   c_toolbar_height =  36;
const int   c_button_size_w =   32;
const int   c_button_size_h =   26;


//####################################################################################
//##    Build Toolbar
//####################################################################################
void ViewToolbar::buildToolbar() {

    this->setObjectName("viewToolBar");
    this->setFixedHeight(c_toolbar_height);
        QHBoxLayout *view_toolbar_layout = new QHBoxLayout(this);
        view_toolbar_layout->setObjectName(QStringLiteral("viewToolBarLayout"));
        view_toolbar_layout->setSpacing(4);
        view_toolbar_layout->setContentsMargins(6, 0, 6, 0);

        // ***** Selectable Button group that keeps track of which mouse mode we are in: Pointer, Hand, Mganify, etc.
        widgetGroupMouse = new QWidget();
        widgetGroupMouse->setObjectName(QStringLiteral("widgetGroupMouse"));
        widgetGroupMouse->setFixedHeight(c_toolbar_height);
            QHBoxLayout *toolbarLayoutMouse = new QHBoxLayout(widgetGroupMouse);
            toolbarLayoutMouse->setSpacing(3);
            toolbarLayoutMouse->setContentsMargins(0, 0, 0, 0);

            buttonsGroupMouse = new QButtonGroup();
            buttonsGroupMouse->setExclusive(true);
            connect(buttonsGroupMouse, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupMouseClicked(int)));

            mouse_pointer = createToolBarButton(QStringLiteral("mousePointer"), Advisor_Info::Mouse_Pointer, c_button_size_w, c_button_size_h + 2, true);
            mouse_pointer->setChecked(true);
            buttonsGroupMouse->addButton(mouse_pointer, int(Mouse_Mode::Pointer));
            toolbarLayoutMouse->addWidget(mouse_pointer);

            mouse_hand = createToolBarButton(QStringLiteral("mouseHand"), Advisor_Info::Mouse_Hand, c_button_size_w, c_button_size_h + 2, true);
            buttonsGroupMouse->addButton(mouse_hand,    int(Mouse_Mode::Hand));
            toolbarLayoutMouse->addWidget(mouse_hand);

            mouse_magnify = createToolBarButton(QStringLiteral("mouseMagnify"), Advisor_Info::Mouse_Magnify, c_button_size_w, c_button_size_h + 2, true);
            buttonsGroupMouse->addButton(mouse_magnify, int(Mouse_Mode::Magnify));
            toolbarLayoutMouse->addWidget(mouse_magnify);

        view_toolbar_layout->addWidget(widgetGroupMouse);
        view_toolbar_layout->addSpacing(2);
        view_toolbar_layout->addWidget(createToolBarSpacer(c_button_size_h - 2));


        // ***** Mouse Mode Add-On, Hand: Center Point Label / Input
        widgetGroupHandTool = new QWidget();
        widgetGroupHandTool->setObjectName(QStringLiteral("widgetGroupHandTool"));
        widgetGroupHandTool->setFixedHeight(c_toolbar_height);
            QHBoxLayout *toolbarLayoutHand = new QHBoxLayout(widgetGroupHandTool);
            toolbarLayoutHand->setSpacing(0);
            toolbarLayoutHand->setContentsMargins(4, 0, 0, 0);
            toolbarLayoutHand->setAlignment(Qt::AlignmentFlag::AlignVCenter);

            QLabel *center_point = new QLabel();
            center_point->setFont(Dr::CustomFont());
            center_point->setAlignment(Qt::AlignmentFlag::AlignCenter);
            center_point->setText("Center Point: ");
            toolbarLayoutHand->addWidget(center_point);

            InspectorDoubleSpinSlot *point_x = new InspectorDoubleSpinSlot();
            InspectorDoubleSpinSlot *point_y = new InspectorDoubleSpinSlot();

            // Point X Box
            point_x->setToolTip("Center X Coordinate");
            point_x->setFixedHeight(22);
            point_x->setFont(Dr::CustomFont());
            point_x->setDecimals(1);
            point_x->setAttribute(Qt::WA_MacShowFocusRect, 0);
            point_x->setRange(-1000000000.0, 1000000000.0);
            point_x->setPrefix("X: ");
            point_x->setSingleStep(100.0);
            point_x->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
            point_x->setValue(0);
            point_x->setUpdateTolerance(5.0);
            int max_point_width = Dr::CheckFontWidth(Dr::CustomFont(), "X: -1000000000.0");
            point_x->setFixedWidth(max_point_width);
            m_filter_hover->attachToHoverHandler(point_x, "Center X Coordinate", "Current X coordinate location of the center of the view.");
            toolbarLayoutHand->addSpacing(5);
            toolbarLayoutHand->addWidget(point_x);
            if (m_view_node != nullptr) {
                connect (point_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this, point_y] (double d) {
                         m_view_node->centerOn(QPointF(d, -point_y->value()));
                });
                connect(m_view_node, SIGNAL(updateCenterPointX(double)), point_x, SLOT(updateValue(double)));

            } else if (m_view_editor != nullptr) {
                connect (point_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this, point_y] (double d) {
                         m_view_editor->centerOn(QPointF(d, -point_y->value()));
                });
                connect(m_view_editor, SIGNAL(updateCenterPointX(double)), point_x, SLOT(updateValue(double)));
            }

            // Point Y Box
            point_y->setToolTip("Center Y Coordinate");
            point_y->setFixedHeight(22);
            point_y->setFont(Dr::CustomFont());
            point_y->setDecimals(1);
            point_y->setAttribute(Qt::WA_MacShowFocusRect, 0);
            point_y->setRange(-1000000000.0, 1000000000.0);
            point_y->setPrefix("Y: ");
            point_y->setSingleStep(100.0);
            point_y->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
            point_y->setValue(0);
            point_y->setUpdateTolerance(5.0);
            point_y->setFixedWidth(max_point_width);
            m_filter_hover->attachToHoverHandler(point_y, "Center Y Coordinate", "Current Y coordinate location of the center of the view.");
            toolbarLayoutHand->addSpacing(6);
            toolbarLayoutHand->addWidget(point_y);
            if (m_view_node != nullptr) {
                connect (point_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this, point_x] (double d) {
                         m_view_node->centerOn(QPointF(point_x->value(), -d));
                });
                connect(m_view_node, SIGNAL(updateCenterPointY(double)), point_y, SLOT(updateValue(double)));

            } else if (m_view_editor != nullptr) {
                connect (point_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this, point_x] (double d) {
                         m_view_editor->centerOn(QPointF(point_x->value(), -d));
                });
                connect(m_view_editor, SIGNAL(updateCenterPointY(double)), point_y, SLOT(updateValue(double)));
            }

            // Reset to Center
            QToolButton *move_to_center = new QToolButton();
            move_to_center->setToolTip("Center View to Zero");
            move_to_center->setFixedSize(25, 22);
            move_to_center->setObjectName("buttonImageMiniButton");
                QPixmap center_icon(":/assets/toolbar_icons/toolbar_reset_center.png");
                center_icon = QPixmap::fromImage( Dr::ColorizeImage(center_icon.toImage(), Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Light))) );
                move_to_center->setIcon( QIcon(center_icon.scaled(QSize(15, 15), Qt::KeepAspectRatio, Qt::SmoothTransformation)) );
            if (m_view_node != nullptr || m_view_editor != nullptr) {
                connect(move_to_center, &QToolButton::pressed, [this, point_x, point_y]() {
                    point_x->updateValue(0.0);
                    point_y->updateValue(0.0);
                    m_editor_relay->viewCenterOnPoint(QPointF(0, 0));
                });
            }
            m_filter_hover->attachToHoverHandler(move_to_center, "Center View to Zero", "Moves the centers of the World Editor to coordinate (0, 0).");
            toolbarLayoutHand->addSpacing(6);
            toolbarLayoutHand->addWidget(move_to_center);

            toolbarLayoutHand->addStretch();

        widgetGroupHandTool->hide();
        view_toolbar_layout->addWidget(widgetGroupHandTool);


        // ***** Mouse Mode Add-On, Zoom: Current Zoom Percentage Drop Down, Fit To View, Slider
        widgetGroupZoomTool = new QWidget();
        widgetGroupZoomTool->setObjectName(QStringLiteral("widgetGroupZoomTool"));
        widgetGroupZoomTool->setFixedHeight(c_toolbar_height);
            QHBoxLayout *toolbarLayoutZoom = new QHBoxLayout(widgetGroupZoomTool);
            toolbarLayoutZoom->setSpacing(0);
            toolbarLayoutZoom->setContentsMargins(3, 0, 0, 0);
            toolbarLayoutZoom->setAlignment(Qt::AlignmentFlag::AlignVCenter);

            InspectorSpinSlot   *zoom_spin =   new InspectorSpinSlot();
            QPushButton         *drop_button = new QPushButton();
            ToolbarSlider       *zoom_slider = new ToolbarSlider();

            // Zoom Spin Box
            zoom_spin->setObjectName(QStringLiteral("zoomSpin"));
            zoom_spin->setToolTip("Zoom Level");
            zoom_spin->setFixedHeight(22);
            zoom_spin->setFont(Dr::CustomFont());
            zoom_spin->setAttribute(Qt::WA_MacShowFocusRect, 0);
            if (m_view_node != nullptr) {
                zoom_spin->setRange(3,  400);
            } else if (m_view_editor != nullptr) {
                zoom_spin->setRange(1, 3200);
            }
            zoom_spin->setSingleStep(50);
            zoom_spin->setSuffix("%");
            zoom_spin->setValue(50);
            int max_zoom_width = Dr::CheckFontWidth(Dr::CustomFont(), "3200%") + 20;
            zoom_spin->setFixedWidth(max_zoom_width);
            zoom_spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
            if (m_view_node != nullptr) {
                connect (zoom_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this] (int i) {
                         m_view_node->zoomToScale(static_cast<double>(i / 100.0));
                });
                connect(m_view_node, SIGNAL(updateZoomSpin(int)), zoom_spin, SLOT(updateValue(int)));

            } else if (m_view_editor != nullptr) {
                connect (zoom_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this] (int i) {
                         m_view_editor->zoomToScale(static_cast<double>(i / 100.0));
                });
                connect(m_view_editor, SIGNAL(updateZoomSpin(int)), zoom_spin, SLOT(updateValue(int)));
            }
            m_filter_hover->attachToHoverHandler(zoom_spin, "Zoom Level", "Set zoom level. <b>Min:</b> 1% - <b>Max:</b> 3200%");
            toolbarLayoutZoom->addWidget(zoom_spin);

            // Zoom Spin Box Drop Down Menu Button
            drop_button->setObjectName(QStringLiteral("spinBoxSelect"));
            drop_button->setToolTip("Preset Zoom Level");
            drop_button->setFont(Dr::CustomFont());
            drop_button->setFixedWidth(23);
                QMenu *menu = new QMenu(this);
                    menu->setObjectName(QStringLiteral("menuComboBox"));
                    menu->setMinimumWidth(130);
                QActionGroup *group;
                    group = new QActionGroup(menu);
                    group->setExclusive(true);
                QStringList options;
                if (m_view_node != nullptr) {
                    options = QStringList{ "25", "50", "100", "200", "400" };
                } else if (m_view_editor != nullptr) {
                    options = QStringList{ "25", "50", "100", "200", "400", "800", "1600", "3200" };
                }
                    for (auto string : options) {
                        QString action_text = string + tr("%");
                        if (m_view_node != nullptr) {
                            if (string == "100") action_text += " - (default)";
                        } else if (m_view_editor != nullptr) {
                            if (string == "50")  action_text += " - (default)";
                        }
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
                        if (m_view_node != nullptr) {
                            connect(action, &QAction::triggered, [this, action]() {
                                    m_view_node->zoomToScale(static_cast<double>(action->property(User_Property::Integer).toInt()) / 100.0);
                            });

                        } else if (m_view_editor != nullptr) {
                            connect(action, &QAction::triggered, [this, action]() {
                                    m_view_editor->zoomToScale(static_cast<double>(action->property(User_Property::Integer).toInt()) / 100.0);
                            });
                        }
                    }
            drop_button->setMenu(menu);
            menu->installEventFilter(new DrFilterPopUpMenuRelocater(menu, 2, -max_zoom_width));
            m_filter_hover->attachToHoverHandler(drop_button, "Preset Zoom Level", "Select from preset zoom levels.");
            toolbarLayoutZoom->addWidget(drop_button);

            // Fit to View Button
            QToolButton *fit_to_view = new QToolButton();
            fit_to_view->setToolTip("Fit to View");
            fit_to_view->setFixedSize(25, 22);
            fit_to_view->setObjectName("buttonImageMiniButton");
                QPixmap fit_icon(":/assets/toolbar_icons/toolbar_fit_to_view.png");
                fit_icon = QPixmap::fromImage( Dr::ColorizeImage(fit_icon.toImage(), Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Light))) );
                fit_to_view->setIcon( QIcon(fit_icon.scaled(QSize(10, 10), Qt::KeepAspectRatio, Qt::SmoothTransformation)) );
            if (m_view_node != nullptr) {
                connect(fit_to_view, &QPushButton::pressed, [this]() { m_view_node->zoomToContents(); });

            } else if (m_view_editor != nullptr) {
                connect(fit_to_view, &QPushButton::pressed, [this]() { m_view_editor->zoomToContents(); });
            }
            m_filter_hover->attachToHoverHandler(fit_to_view, "Fit to View", "Fits entire contents into World Editor.");
            toolbarLayoutZoom->addSpacing(8);
            toolbarLayoutZoom->addWidget(fit_to_view);

            // Zoom Slider
            zoom_slider->setToolTip("Select Zoom Level");
            if (m_view_node != nullptr) {
                zoom_slider->setRange( 0, 35);
            } else if (m_view_editor != nullptr) {
                zoom_slider->setRange(-5, 50);
            }
            zoom_slider->setFixedWidth(165);
            zoom_slider->setMaximumHeight(26);
            zoom_slider->setValue(20);
            zoom_slider->setTickInterval(5);
            zoom_slider->setSingleStep(1);
            zoom_slider->setTickPosition(QSlider::TickPosition::TicksAbove);
            zoom_slider->setTickColor(QColor("#505050"));
            zoom_slider->setOrientation(Qt::Orientation::Horizontal);
            if (m_view_node != nullptr) {
                connect(zoom_slider, &QSlider::valueChanged, this, [this, zoom_slider] () {
                        m_view_node->zoomToPower(zoom_slider->value() * 10);
                });
                connect(m_view_node, SIGNAL(updateZoomSlider(int)), zoom_slider, SLOT(updateValue(int)));

            } else if (m_view_editor != nullptr) {
                connect(zoom_slider, &QSlider::valueChanged, this, [this, zoom_slider] () {
                        m_view_editor->zoomToPower(zoom_slider->value() * 10);
                });
                connect(m_view_editor, SIGNAL(updateZoomSlider(int)), zoom_slider, SLOT(updateValue(int)));
            }
            m_filter_hover->attachToHoverHandler(zoom_slider, "Select Zoom Level", "Slide to select zoom level.");
            toolbarLayoutZoom->addSpacing(8);
            toolbarLayoutZoom->addWidget(zoom_slider);

            toolbarLayoutZoom->addStretch();

        widgetGroupZoomTool->hide();
        view_toolbar_layout->addWidget(widgetGroupZoomTool);


        // ***** Toggled View Options, Debug On/Off, etc
        view_toolbar_layout->addStretch();
        if (m_editor_mode == Editor_Mode::World_Editor) {
            widgetGroupToggle = new QWidget();
            widgetGroupToggle->setObjectName(QStringLiteral("widgetGroupToggle"));
            widgetGroupToggle->setFixedHeight(c_toolbar_height);
                QHBoxLayout *toolbarLayoutToggle = new QHBoxLayout(widgetGroupToggle);
                toolbarLayoutToggle->setSpacing(3);
                toolbarLayoutToggle->setContentsMargins(0, 0, 0, 0);

                buttonsGroupToggle = new QButtonGroup();
                buttonsGroupToggle->setExclusive(false);
                connect(buttonsGroupToggle, SIGNAL(buttonClicked(int)), this, SLOT(buttonGroupToggleClicked(int)));

                QToolButton *camera_on_off = createToolBarButton("cameraOnOff", Advisor_Info::Camera_On_Off, c_button_size_w, c_button_size_h, true);
                buttonsGroupToggle->addButton(camera_on_off, int(Buttons_Toggle::CameraOnOff));
                camera_on_off->setChecked(false);
                toolbarLayoutToggle->addWidget(camera_on_off);

                QToolButton *debug_on_off = createToolBarButton("debugOnOff", Advisor_Info::Debug_On_Off, c_button_size_w, c_button_size_h, true);
                buttonsGroupToggle->addButton(debug_on_off, int(Buttons_Toggle::DebugOnOff));
                debug_on_off->setChecked(false);
                toolbarLayoutToggle->addWidget(debug_on_off);

            view_toolbar_layout->addWidget(createToolBarSpacer(c_button_size_h - 2));
            view_toolbar_layout->addSpacing(2);
            view_toolbar_layout->addWidget(widgetGroupToggle);
        }
}



























