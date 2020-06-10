//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      FormMain - Class that holds our main form window
//
//      FormMain Modes:
//          World Graph         (World / UI Layout)
//          World Creator       (2D Physics World)
//          UI Creator
//          Sound Creator
//      Future:
//          Stage Graph?        (2D Physics World Stage Layout)
//
//      Main Components of FormMain:
//          Tool Bar
//          Advisor (Dock)
//          Inspector (Dock)
//
//          Components That Have Items that Can Appear in Inspector:
//              Asset List
//              View (Editor / Node Graph / Sound Mixer)
//                  Status Bar
//              Project Tree
//              Variable List
//              Bottom Area (Labels, Stages?)
//
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>
#include <QWidgetAction>

#include "3rd_party/soloud/soloud.h"
#include "core/colors/colors.h"
#include "editor/interface_editor_relay.h"
#include "project/constants_comps_and_props.h"

// Necessary forward declarations
class DrFilterHoverHandler;
class DrProject;
class EditorScene;
class EditorView;
class FormPopup;
class MixerScene;
class MixerView;
class NodeScene;
class NodeView;
class TreeAssets;
class TreeAdvisor;
class TreeInspector;
class TreeProject;
class ViewRubberBand;
class ViewToolbar;
class VisualFrame;


//####################################################################################
//##    FormMain
//##        Main editor window
//############################
class FormMain : public QMainWindow, public IEditorRelay
{
    Q_OBJECT

public:
    // Constructor / Destructor
    explicit FormMain(QWidget *parent = nullptr, std::string file_to_open = "");
    virtual ~FormMain() override;


private:
    // ***** Local Variables
    DrProject              *m_project           { nullptr };                        // INITIAL DEFINITION of DrProject!!!!! Holds current open game project!
    DrProject              *m_external_images   { nullptr };                        // Project that loads included images
    DrFilterHoverHandler   *m_filter_hover      { nullptr };                        // Pointer to an event filter hover handler

    Editor_Mode             m_current_mode      { Editor_Mode::Program_Loading };   // Holds what state FormMain is in


    // ***** Menu Widgets
    QMenuBar        *m_menu_bar;
    QAction         *m_action_undo, *m_action_redo;


    // ***** ToolBar Widgets
    QList<QWidget*>      m_toolbar_widgets { };
    QList<QLayoutItem*>  m_toolbar_spacers { };

    QToolBar        *m_toolbar;
    QWidget         *m_widget_toolbar;
    QHBoxLayout     *m_widget_toolbar_layout;

    // Shared Groups
    QWidget             *m_widget_group_mode;           QButtonGroup    *m_buttons_group_mode;
    QWidget             *m_widget_group_edit;           QButtonGroup    *m_buttons_group_edit;          QToolButton *m_button_add;
    QWidget             *m_widget_group_play;           QButtonGroup    *m_buttons_group_play;
    QWidget             *m_widget_group_settings;

    // View "Editor" Groups
    QWidget             *m_widget_group_layering;       QButtonGroup    *m_buttons_group_layering;
    QWidget             *m_widget_group_transform;      QButtonGroup    *m_buttons_group_transform;
    QWidget             *m_widget_group_grid_full;      QButtonGroup    *m_buttons_group_grid_full;

    // View "Node" Groups
    QWidget             *m_widget_group_grid_simple;    QButtonGroup    *m_buttons_group_grid_simple;

    // View "Mixer" Groups
    QWidget             *m_widget_group_visual;


    // ***** Shared FormMain Widgets
    QDockWidget    *m_dock_advisor;         TreeAdvisor    *m_tree_advisor;
    QDockWidget    *m_dock_assets;          TreeAssets     *m_tree_assets;
    QDockWidget    *m_dock_inspector;       TreeInspector  *m_tree_inspector;


    // ***** "Empty" Widget
    QWidget        *m_widget_central_clear;


    // ***** "World Creator" (Editor) Widgets
    QWidget         *m_widget_central_editor;
    TreeProject         *m_tree_project;                        // Shows Project Entities
    EditorScene         *m_scene_editor;                        // Holds the currently selected Stage
    EditorView          *m_view_editor;                         // Renders the World Editor View
    QScrollArea         *m_area_bottom;
    QFrame              *m_status_bar;
    ViewToolbar         *m_toolbar_editor;

    //       "World Creator" (Editor) Status Bar Widgets
    QLabel                  *m_label_selected,  *m_label_info,      *m_label_mouse_position;

    //       "World Creator" (Editor) Labels to display info
    QLabel                  *label_1,           *label_2,           *label_3,           *label_mouse_1,     *label_mouse_2;
    QLabel                  *label_object_1,    *label_object_2,    *label_object_3,    *label_object_4,    *label_object_5;
    QLabel                  *label_position,    *label_center,      *label_scale,       *label_rotate,      *label_z_order,     *label_pos_flag;
    QLabel                  *label_bottom;


    // ***** "World Graph" Widgets
    QWidget         *m_widget_central_world_graph;
    NodeScene           *m_scene_world_graph;
    NodeView            *m_view_world_graph;
    ViewToolbar         *m_toolbar_world_graph;


    // ***** "Sound Creator" Widgets
    QWidget         *m_widget_central_sound_creator;
    MixerScene          *m_scene_mixer;
    MixerView           *m_view_mixer;
    VisualFrame         *m_visualizer;                                      // Widget that displays visualizer for current sounds playing
    QTimer              *m_visual_timer;                                    // Event timer that calls update for visualizer
    QElapsedTimer        m_last_play_time;                                  // Tracks when sound was last still playing


public:
    // Getters
    DrProject*          getProject()                { return m_project; }
    DrProject*          getExternalImageProject()   { return m_external_images; }

    // Misc functions
    void                setLabelText(Label_Names label_name, QString new_text);

    // Event Handlers
    virtual void        closeEvent(QCloseEvent *event) override;                                            // Inherited from QWidget
    virtual bool        eventFilter(QObject *obj, QEvent *event) override;                                  // Inherited from QObject
    virtual void        resizeEvent(QResizeEvent *event) override;                                          // Inherited from QWidget


    //############################ START: Interface Relay Implementations    
    virtual TreeAdvisor*        getAdvisor() override       { return m_tree_advisor; }
    virtual TreeAssets*         getAssetTree() override     { return m_tree_assets; }
    virtual TreeInspector*      getInspector() override     { return m_tree_inspector; }
    virtual TreeProject*        getProjectTree() override   { return m_tree_project; }

    virtual EditorView*         getViewEditor() override    {
        if (getEditorMode() == Editor_Mode::World_Creator)    return m_view_editor;
        return nullptr;
    }
    virtual MixerView*          getViewMixer() override     {
        if (getEditorMode() == Editor_Mode::Sound_Creator)    return nullptr;
        return nullptr;
    }
    virtual NodeView*           getViewNode() override      {
        if (getEditorMode() == Editor_Mode::World_Graph)      return m_view_world_graph;
        return nullptr;
    }

    virtual Editor_Mode         getEditorMode() override;
    virtual void                setEditorMode(Editor_Mode new_mode) override;

    virtual void        buildAssetTree() override;
    virtual void        buildInspector(QList<long> entity_key_list, bool force_rebuild = false) override;
    virtual void        buildProjectTree(bool total_rebuild = false) override;
    virtual void        buildScene(long from_key) override;

    virtual void        updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, std::list<DrSettings*> changed_items,
                                                           std::list<ComponentProperty> property_names) override;
    virtual void        updateInspectorEnabledProperties() override;
    virtual void        updateItemSelection(Editor_Widgets selected_from, QList<long> optional_key_list = QList<long>{ }) override;

    virtual DrProject*  currentProject() override;
    virtual double      currentViewGridAngle() override;
    virtual QPointF     currentViewGridScale() override;
    virtual View_Mode   currentViewMode() override;
    virtual double      currentViewZoom() override;
    virtual QPointF     roundPointToGrid(QPointF point_in_scene) override;
    virtual void        viewCenterOnPoint(QPointF center_point) override;
    virtual void        viewFitToContents() override;
    virtual void        viewUpdateToolbar(int button_id) override;
    virtual void        viewZoomToScale(double zoom_scale) override;

public slots:
    virtual void        setAdvisorInfo(HeaderBodyList header_body_list) override;
    virtual void        setAdvisorInfo(QString header, QString body) override;
    virtual void        setMousePosition(std::string x, std::string y) override;
    //############################ END: Interface Relay Implementations


private:
    // Form Building / Setup
    void            buildMenu();
    void            buildSceneAfterLoading(long stage_key);
    void            buildToolBar();
    void            buildCentralWidgetClear();
    void            buildCentralWidgetWorldGraph();
    void            buildCentralWidgetWorldCreator();
    void            buildCentralWidgetSoundCreator();
    void            changePalette(Color_Scheme new_color_scheme);
    QLabel*         createLabel(QWidget *parent, QString object_name, QRect label_rect, QFont &label_font);
    void            initializeFormMain();
    void            rebuildFormMain(Editor_Mode new_mode);

    // File Menu Functions
    bool            askShouldSave(QString before_what);
    void            menuNew(bool test_project = false);
    void            menuOpen();
    void            menuSave(bool save_as = false);
    // Edit Menu Functions
    void            menuUndo();
    void            menuRedo();
    // Debug Menu Functions
    void            menuListChildren();
    // Help Menu Functions
    void            menuAbout();

    // ToolBar Functions
    void            addToolBarGroup(QWidget *group, bool add_spacer = true);
    void            buttonGroupModeSetChecked(int id);
    void            clearToolBar();
    QToolButton*    createToolBarButton(const QString &style_sheet_name, HeaderBodyList advisor_text, bool checkable = false, bool enabled = true);
    QLabel*         createToolBarSpacer(int height = 24, int space_on_the_right = 1, bool visible = true);

public:
    void            setToolBar(Editor_Mode new_mode);
    void            updateToolBar();


public slots:
    // Main ToolBar
    void            buttonGroupEditClicked(int id);
    void            buttonGroupGridFullClicked(int id);
    void            buttonGroupGridSimpleClicked(int id);
    void            buttonGroupLayeringClicked(int id);
    void            buttonGroupModeClicked(int id);
    void            buttonGroupPlayClicked(int id);
    void            buttonGroupTransformClicked(int id);

    void            buildSceneAfterWaiting(long stage_key);

    void            centerViewTimer(QPointF center_point);
    void            editMenuAboutToShow();
    void            editMenuAboutToHide();

    // Sound Creator
    void            drawVisuals();


signals:
    // Undo Stack Signals
    void            newStageSelected(DrProject *project, EditorScene *scene, long old_stage, long new_stage);

};



#endif // MAINWINDOW_H















