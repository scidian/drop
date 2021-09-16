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
#include <QDial>
#include <QMenu>
#include <QRadioButton>
#include <QToolBar>
#include <QToolButton>

#include "core/dr_random.h"
#include "core/dr_time.h"
#include "core/sound.h"
#include "editor/enums_editor.h"
#include "editor/event_filters/event_filters.h"
#include "editor/form_main/form_main.h"
#include "editor/form_sound/form_sound_effect.h"
#include "editor/forms/form_blank.h"
#include "editor/forms/form_fonts.h"
#include "editor/forms/form_popup.h"
#include "editor/forms/form_progress_box.h"
#include "editor/forms/form_settings.h"
#include "editor/form_sound/visualizer.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/widgets/widgets_inspector.h"
#include "editor/widgets/widgets_view.h"


// Local Constants
const int   c_spacer_height =   28;


//####################################################################################
//##    Builds FormMain ToolBar
//####################################################################################
void FormMain::buildToolBar() {

    // Widgets to use during building
    QToolButton *tool;

    // ***** Initialize toolbar widget
    m_toolbar = new QToolBar(this);
    m_toolbar->setObjectName(QStringLiteral("toolbar"));
    m_toolbar->setWindowTitle( QMainWindow::tr("Toolbar") );
    m_toolbar->setMovable(false);
    m_toolbar->setFloatable(false);
    m_toolbar->installEventFilter(new DrFilterClickAndDragWindow(m_toolbar));

    // ***** This is a container object that holds all toolbar button groups, allowing us to put them in a layout
    m_widget_toolbar = new QWidget();
    m_widget_toolbar->setObjectName(QStringLiteral("widgetToolBar"));
    m_widget_toolbar_layout = new QHBoxLayout(m_widget_toolbar);
    m_widget_toolbar_layout->setSpacing(3);
    m_widget_toolbar_layout->setContentsMargins(12, 0, 12, 0);

    // ******************** Shared Groups ********************

    // ***** Mode Group: Selectable button group that keeps track of which mode we are in: World Graph, World Creator, UI Creator, Sound Creator, etc
    m_widget_group_mode = new QWidget();
    m_widget_group_mode->setObjectName(QStringLiteral("widgetGroupMode"));
        QHBoxLayout *toolbar_layout_mode = new QHBoxLayout(m_widget_group_mode);
        toolbar_layout_mode->setSpacing(3);
        toolbar_layout_mode->setContentsMargins(0, 1, 0, 0);

        m_buttons_group_mode = new QButtonGroup();
        m_buttons_group_mode->setExclusive(true);
        connect(m_buttons_group_mode, SIGNAL(idClicked(int)), this, SLOT(buttonGroupModeClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonModeWorldGraph"), Advisor_Info::Mode_World_Graph, true);
        m_buttons_group_mode->addButton(tool, int(Editor_Mode::World_Graph));
        toolbar_layout_mode->addWidget(tool);
        toolbar_layout_mode->addWidget(createToolBarSpacer(c_spacer_height));

        tool = createToolBarButton(QStringLiteral("buttonModeWorldEdit"), Advisor_Info::Mode_World_Creator, true);
        m_buttons_group_mode->addButton(tool, int(Editor_Mode::World_Creator));
        toolbar_layout_mode->addWidget(tool);
        toolbar_layout_mode->addWidget(createToolBarSpacer(c_spacer_height));

        tool = createToolBarButton(QStringLiteral("buttonModeUIEdit"), Advisor_Info::Mode_UI_Creator, true);
        m_buttons_group_mode->addButton(tool, int(Editor_Mode::UI_Creator));
        toolbar_layout_mode->addWidget(tool);
        toolbar_layout_mode->addWidget(createToolBarSpacer(c_spacer_height));

        tool = createToolBarButton(QStringLiteral("buttonModeSound"), Advisor_Info::Mode_Sound_Creator, true);
        m_buttons_group_mode->addButton(tool, int(Editor_Mode::Sound_Creator));
        toolbar_layout_mode->addWidget(tool);

    // ***** Edit Group: Holds Add , Delete, future Cut / Copy / Paste
    m_widget_group_edit = new QWidget(m_widget_toolbar);
    m_widget_group_edit->hide();
    m_widget_group_edit->setObjectName(QStringLiteral("widgetGroupEdit"));
        QHBoxLayout *toolbar_layout_edit = new QHBoxLayout(m_widget_group_edit);
        toolbar_layout_edit->setSpacing(5);
        toolbar_layout_edit->setContentsMargins(0, 0, 0, 0);

        m_buttons_group_edit = new QButtonGroup();
        m_buttons_group_edit->setExclusive(false);
        connect(m_buttons_group_edit, SIGNAL(idClicked(int)), this, SLOT(buttonGroupEditClicked(int)));

        m_button_add = createToolBarButton(QStringLiteral("buttonAddThing"), Advisor_Info::Add_Entity, false, true);
        m_buttons_group_edit->addButton(m_button_add, int(Buttons_Edit::Add));
        toolbar_layout_edit->addWidget(m_button_add);

        tool = createToolBarButton(QStringLiteral("buttonDuplicate"), Advisor_Info::Duplicate, false, false);
        m_buttons_group_edit->addButton(tool, int(Buttons_Edit::Duplicate));
        toolbar_layout_edit->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonDeleteThing"), Advisor_Info::Trash_Can, false, false);
        m_buttons_group_edit->addButton(tool, int(Buttons_Edit::Delete));
        toolbar_layout_edit->addWidget(tool);

    // ***** Play Group: Holds buttons that starts game Engine window
    m_widget_group_play = new QWidget(m_widget_toolbar);
    m_widget_group_play->hide();
    m_widget_group_play->setObjectName(QStringLiteral("widgetGroupPlay"));
        QHBoxLayout *toolbar_layout_play = new QHBoxLayout(m_widget_group_play);
        toolbar_layout_play->setSpacing(5);
        toolbar_layout_play->setContentsMargins(0, 0, 0, 0);

        m_buttons_group_play = new QButtonGroup();
        m_buttons_group_play->setExclusive(false);
        connect(m_buttons_group_play, SIGNAL(idClicked(int)), this, SLOT(buttonGroupPlayClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonPlay"), Advisor_Info::Play_Game, false, true);
        m_buttons_group_play->addButton(tool, int(Buttons_Play::Play));
        toolbar_layout_play->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonPlayStage"), Advisor_Info::Play_Stage, false, true);
        m_buttons_group_play->addButton(tool, int(Buttons_Play::Play_Stage));
        toolbar_layout_play->addWidget(tool);


    // ***** Settings Buttons
    m_widget_group_settings = new QWidget(m_widget_toolbar);
    m_widget_group_settings->hide();
    m_widget_group_settings->setObjectName(QStringLiteral("widgetGroupSettings"));
        QHBoxLayout *toolbar_layout_settings = new QHBoxLayout(m_widget_group_settings);
        toolbar_layout_settings->setSpacing(5);
        toolbar_layout_settings->setContentsMargins(0, 0, 0, 0);

        // Physics Playground
        ///tool = createToolBarButton(QStringLiteral("buttonPlayground"), Advisor_Info::Settings_Playground, c_button_size_w, c_button_size_h, false);
        ///toolbarLayoutSettings->addWidget(tool);
        ///connect(tool, &QPushButton::clicked, [this] () {
        ///    FormPlayground *playground = new FormPlayground(this);
        ///    playground->show();
        ///});

        tool = createToolBarButton(QStringLiteral("buttonImageViewer"), Advisor_Info::Settings_Image_Viewer, false);
        toolbar_layout_settings->addWidget(tool);
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

        tool = createToolBarButton(QStringLiteral("buttonFontBuilder"), Advisor_Info::Settings_Font_Builder, false);
        toolbar_layout_settings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            ///FormBlank *blank_form = new FormBlank(m_project, this);
            ///blank_form->show();
            FormFonts *font_editor = new FormFonts(m_project, this);
            font_editor->show();
        });

        tool = createToolBarButton(QStringLiteral("buttonSettingsEditor"), Advisor_Info::Settings_Manager, false);
        toolbar_layout_settings->addWidget(tool);
        connect(tool, &QPushButton::clicked, [this] () {
            FormSettings *settings_editor = new FormSettings(m_project, this);
            settings_editor->show();
        });


    // ******************** View "Editor" Groups ********************

    // ***** View "Editor" Add-On, Layering: Holds buttons that send Things to Front / Back
    m_widget_group_layering = new QWidget(m_widget_toolbar);
    m_widget_group_layering->hide();
    m_widget_group_layering->setObjectName(QStringLiteral("widgetGroupLayering"));
        QHBoxLayout *toolbar_layout_layering = new QHBoxLayout(m_widget_group_layering);
        toolbar_layout_layering->setSpacing(1);
        toolbar_layout_layering->setContentsMargins(0, 0, 0, 0);

        m_buttons_group_layering = new QButtonGroup();
        m_buttons_group_layering->setExclusive(false);
        connect(m_buttons_group_layering, SIGNAL(idClicked(int)), this, SLOT(buttonGroupLayeringClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonSendToBack"), Advisor_Info::Send_to_Back, false, false);
        m_buttons_group_layering->addButton(tool, int(Buttons_Layering::Send_To_Back));
        toolbar_layout_layering->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSendBackward"), Advisor_Info::Send_Backward, false, false);
        m_buttons_group_layering->addButton(tool, int(Buttons_Layering::Send_Backward));
        toolbar_layout_layering->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSendForward"), Advisor_Info::Send_Forward, false, false);
        m_buttons_group_layering->addButton(tool, int(Buttons_Layering::Send_Forward));
        toolbar_layout_layering->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSendToFront"), Advisor_Info::Send_to_Front, false, false);
        m_buttons_group_layering->addButton(tool, int(Buttons_Layering::Send_To_Front));
        toolbar_layout_layering->addWidget(tool);

    // ***** View "Editor" Add-On, Transform: Holds buttons that Flip / Rotate things
    m_widget_group_transform = new QWidget(m_widget_toolbar);
    m_widget_group_transform->hide();
    m_widget_group_transform->setObjectName(QStringLiteral("widgetGroupTransform"));
        QHBoxLayout *toolbar_layout_transform = new QHBoxLayout(m_widget_group_transform);
        toolbar_layout_transform->setSpacing(1);
        toolbar_layout_transform->setContentsMargins(2, 0, 0, 0);

        m_buttons_group_transform = new QButtonGroup();
        m_buttons_group_transform->setExclusive(false);
        connect(m_buttons_group_transform, SIGNAL(idClicked(int)), this, SLOT(buttonGroupTransformClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonResetObject"), Advisor_Info::Reset, false, false);
        m_buttons_group_transform->addButton(tool, int(Buttons_Transform::Reset_Object));
        toolbar_layout_transform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformFlipH"), Advisor_Info::Flip_H, false, false);
        m_buttons_group_transform->addButton(tool, int(Buttons_Transform::Flip_H));
        toolbar_layout_transform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformFlipV"), Advisor_Info::Flip_V, false, false);
        m_buttons_group_transform->addButton(tool, int(Buttons_Transform::Flip_V));
        toolbar_layout_transform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformRotateL"), Advisor_Info::Rotate_L, false, false);
        m_buttons_group_transform->addButton(tool, int(Buttons_Transform::Rotate_L));
        toolbar_layout_transform->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonTransformRotateR"), Advisor_Info::Rotate_R, false, false);
        m_buttons_group_transform->addButton(tool, int(Buttons_Transform::Rotate_R));
        toolbar_layout_transform->addWidget(tool);

    // ***** View "Editor" Add-On, Grid: Holds buttons that control Snap to Grid, Resize to Grid, Show Grid on Top
    m_widget_group_grid_full = new QWidget(m_widget_toolbar);
    m_widget_group_grid_full->hide();
    m_widget_group_grid_full->setObjectName(QStringLiteral("widgetGroupGridFull"));
        QHBoxLayout *toolbar_layout_grid_full = new QHBoxLayout(m_widget_group_grid_full);
        toolbar_layout_grid_full->setSpacing(0);
        toolbar_layout_grid_full->setContentsMargins(0, 0, 0, 0);

        m_buttons_group_grid_full = new QButtonGroup();
        m_buttons_group_grid_full->setExclusive(false);
        connect(m_buttons_group_grid_full, SIGNAL(idClicked(int)), this, SLOT(buttonGroupGridFullClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonGridOnTop"), Advisor_Info::Grid_Show_On_Top, true);
        m_buttons_group_grid_full->addButton(tool, int(Buttons_Grid::Grid_On_Top));
        tool->setChecked(Dr::GetPreference(Preferences::Editor_Grid_On_Top).toBool());
        toolbar_layout_grid_full->addWidget(tool);
        toolbar_layout_grid_full->addSpacing(1);

        tool = createToolBarButton(QStringLiteral("buttonResizeToGrid"), Advisor_Info::Resize_To_Grid, true);
        m_buttons_group_grid_full->addButton(tool, int(Buttons_Grid::Resize_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::Editor_Resize_To_Grid).toBool());
        toolbar_layout_grid_full->addWidget(tool);
        toolbar_layout_grid_full->addSpacing(1);

        tool = createToolBarButton(QStringLiteral("buttonSnapToGrid"), Advisor_Info::Grid_Snap_To_Grid, true);
        m_buttons_group_grid_full->addButton(tool, int(Buttons_Grid::Snap_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::Editor_Snap_To_Grid).toBool());
        toolbar_layout_grid_full->addWidget(tool);

        tool = createToolBarButton(QStringLiteral("buttonSnapOptions"), Advisor_Info::Grid_Snap_Options, false, true);
        m_buttons_group_grid_full->addButton(tool, int(Buttons_Grid::Snap_Options));
        toolbar_layout_grid_full->addWidget(tool);


    // ******************** View "Node" Groups ********************

    // ***** View "Node" Add-On, Grid: Holds button that controls Snap to Grid
    m_widget_group_grid_simple = new QWidget(m_widget_toolbar);
    m_widget_group_grid_simple->hide();
    m_widget_group_grid_simple->setObjectName(QStringLiteral("widgetGroupGridSimple"));
        QHBoxLayout *toolbar_layout_grid_simple = new QHBoxLayout(m_widget_group_grid_simple);
        toolbar_layout_grid_simple->setSpacing(0);
        toolbar_layout_grid_simple->setContentsMargins(0, 0, 0, 0);

        m_buttons_group_grid_simple = new QButtonGroup();
        m_buttons_group_grid_simple->setExclusive(false);
        connect(m_buttons_group_grid_simple, SIGNAL(idClicked(int)), this, SLOT(buttonGroupGridSimpleClicked(int)));

        tool = createToolBarButton(QStringLiteral("buttonSnapToMap"), Advisor_Info::Grid_Snap_To_Grid, true);
        m_buttons_group_grid_simple->addButton(tool, int(Buttons_Grid::Snap_To_Grid));
        tool->setChecked(Dr::GetPreference(Preferences::Editor_Snap_To_Grid).toBool());
        toolbar_layout_grid_simple->addWidget(tool);


    // ******************** View "Mixer" Groups ********************

    // ***** View "Mixer" Add-On, Visual: Holds Volume Dial and Visualizer Frame
    m_widget_group_visual = new QWidget(m_widget_toolbar);
    m_widget_group_visual->hide();
    m_widget_group_visual->setObjectName(QStringLiteral("widgetGroupVisual"));
        QHBoxLayout *toolbar_layout_visual = new QHBoxLayout(m_widget_group_visual);
        toolbar_layout_visual->setSpacing(8);
        toolbar_layout_visual->setContentsMargins(0, 0, 0, 0);

        InspectorSpinSlot   *volume_spin = new InspectorSpinSlot();
        ViewDial            *volume_dial = new ViewDial(nullptr, Dial_Style::Knotch_Line, true);

        // Visualizer
        m_visualizer = new VisualFrame();
        m_visualizer->setObjectName(QStringLiteral("visualizerFrame"));
        m_visualizer->setFixedHeight(10.0 + Dr::Scale(20.0));
        m_visualizer->setFixedWidth(50.0 + Dr::Scale(50.0));
        m_visualizer->turnOnAutoScale(m_visualizer->width(), m_visualizer->height());
        m_visualizer->showBorder(true);
        toolbar_layout_visual->addWidget(m_visualizer);

        // Volume Dial
        volume_dial->setObjectName(QStringLiteral("dialMasterVolume"));
        volume_dial->setRange(0, 100);
        volume_dial->setNotchesVisible(false);
        volume_dial->setWrapping(false);
        volume_dial->setValue(Dr::GetPreference(Preferences::Mixer_Master_Volume).toDouble());
        volume_dial->setToolTip("Master Volume");
        connect(volume_dial, &QDial::valueChanged, [volume_dial, volume_spin] () {
            double volume = static_cast<double>(volume_dial->value());
            Dr::SetPreference(Preferences::Mixer_Master_Volume, volume);
            Dr::GetSoLoud()->setGlobalVolume(volume / 100.0);
            volume_spin->blockSignals(true);
            volume_spin->setValue(volume);
            volume_spin->blockSignals(false);
        });
        toolbar_layout_visual->addWidget(volume_dial);

        // Volume Spin Box
        volume_spin->setToolTip("Master Volume");
        volume_spin->setFont(Dr::CustomFont());
        volume_spin->setAttribute(Qt::WA_MacShowFocusRect, 0);
        volume_spin->setRange(0, 100);
        volume_spin->setSingleStep(1);
        volume_spin->setSuffix("%");
        volume_spin->setValue(Dr::GetPreference(Preferences::Mixer_Master_Volume).toDouble());
        volume_spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
        connect (volume_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, [volume_dial] (int i) {
            double volume = static_cast<double>(i);
            Dr::SetPreference(Preferences::Mixer_Master_Volume, volume);
            Dr::GetSoLoud()->setGlobalVolume(volume / 100.0);
            volume_dial->blockSignals(true);
            volume_dial->setValue(volume);
            volume_dial->blockSignals(false);
        });
        toolbar_layout_visual->addWidget(volume_spin);


    // ******************** Set up initial toolbar ********************

    // Clears the containers that keeps track of whats added to the toolbar layout
    m_toolbar_widgets.clear();
    m_toolbar_spacers.clear();

    // Set the initial toolbar as the mode buttons only
    m_widget_toolbar_layout->addWidget(m_widget_group_mode);
    ///m_widget_toolbar_layout->addStretch();

    // Add the toolbar onto FormMain
    m_toolbar->addWidget(m_widget_toolbar);
    m_widget_toolbar->setFixedWidth(this->width());
    this->addToolBar(Qt::ToolBarArea::TopToolBarArea, m_toolbar);
}




//####################################################################################
//##    Button creation calls
//####################################################################################
QToolButton* FormMain::createToolBarButton(const QString &style_sheet_name, HeaderBodyList advisor_text, bool checkable, bool enabled) {
    QToolButton *tool = new QToolButton();
    tool->setObjectName(style_sheet_name);
    if (checkable) {
        tool->setCheckable(true);
        tool->setChecked(false);
    }
    tool->setToolTip(advisor_text[0]);
    tool->setEnabled(enabled);
    m_filter_hover->attachToHoverHandler(tool, advisor_text);
    ///Dr::ApplyDropShadowByType(tool, Shadow_Types::Button_Shadow_Thin);
    return tool;
}

// DEFAULTS: height = 24, space_on_the_right = 1, visible = true
QLabel* FormMain::createToolBarSpacer(int height, int space_on_the_right, bool visible) {
    QString object_name = visible ? QStringLiteral("labelSpacer") : QStringLiteral("labelSpacerNotVisible");

    QLabel *spacer = new QLabel();
    spacer->setObjectName(object_name);
    spacer->setFixedSize( space_on_the_right, height );
    return spacer;
}















