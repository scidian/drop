//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QFocusEvent>
#include <QMainWindow>
#include <QScrollBar>
#include <QVBoxLayout>

#include "editor/docks/docks.h"
#include "editor/helper_library.h"
#include "editor/trees/tree_advisor.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_wave_form.h"


namespace Dr {


//####################################################################################
//##    Builds Advisor Dock
//####################################################################################
QDockWidget* BuildDockAdvisor(DrProject *project, IEditorRelay *editor_relay, TreeAdvisor *&tree_advisor) {
    QSizePolicy size_policy_less(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy_less.setHorizontalStretch(1);
    size_policy_less.setVerticalStretch(1);

    QDockWidget *dock_advisor = new QDockWidget(nullptr, Qt::WindowType::Window | Qt::WindowType::FramelessWindowHint);
    dock_advisor->setWindowTitle( QMainWindow::tr("Advisor") );
    dock_advisor->setObjectName(QStringLiteral("dockAdvisor"));
    dock_advisor->setSizePolicy(size_policy_less);
    dock_advisor->setFont(Dr::CustomFont());
    dock_advisor->setFeatures(QDockWidget::DockWidgetMovable); /// | QDockWidget::DockWidgetFloatable); /// | QDockWidget::DockWidgetClosable);
    dock_advisor->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        QWidget *widget_advisor = new QWidget();
        widget_advisor->setObjectName(QStringLiteral("widgetAdvisor"));
        widget_advisor->setSizePolicy(size_policy_less);
        //widgetAdvisor->setMaximumHeight(180);
            QVBoxLayout *vertical_layout_advisor = new QVBoxLayout(widget_advisor);
            vertical_layout_advisor->setObjectName(QStringLiteral("verticalLayoutAdvisor"));
            vertical_layout_advisor->setSpacing(0);
            vertical_layout_advisor->setContentsMargins(0, 0, 0, 0);

                // ***** Load our custom TreeAdvisor for the helpful advisor text
                tree_advisor = new TreeAdvisor(widget_advisor, project, editor_relay);
                tree_advisor->setObjectName(QStringLiteral("treeAdvisor"));
                tree_advisor->setColumnCount(1);
                tree_advisor->setFont(Dr::CustomFontLarger());
                tree_advisor->setProperty("showDropIndicator", false);
                tree_advisor->setDragEnabled(false);
                tree_advisor->setDragDropOverwriteMode(false);
                tree_advisor->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
                tree_advisor->setDefaultDropAction(Qt::DropAction::IgnoreAction);
                tree_advisor->setAlternatingRowColors(false);
                tree_advisor->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
                tree_advisor->setIndentation(12);
                tree_advisor->setRootIsDecorated(false);
                tree_advisor->setItemsExpandable(false);
                tree_advisor->setExpandsOnDoubleClick(false);
                tree_advisor->setHeaderHidden(true);
                tree_advisor->setFrameShape(QFrame::NoFrame);
            vertical_layout_advisor->addWidget(tree_advisor);

        dock_advisor->setWidget(widget_advisor);
    return dock_advisor;
}



//####################################################################################
//##    Builds Assets Dock
//####################################################################################
QDockWidget* BuildDockAssets(DrProject *project, IEditorRelay *editor_relay, TreeAssets *&tree_assets) {
    QSizePolicy size_policy_preferred_vertical(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy_preferred_vertical.setHorizontalStretch(0);
    size_policy_preferred_vertical.setVerticalStretch(1);

    QDockWidget *m_dock_assets = new QDockWidget();
    m_dock_assets->setWindowTitle( "" );
    m_dock_assets->setObjectName(QStringLiteral("dockAssets"));
    m_dock_assets->setFont(Dr::CustomFont());
    m_dock_assets->setFeatures(QDockWidget::DockWidgetMovable); /// | QDockWidget::DockWidgetFloatable);  /// | QDockWidget::DockWidgetClosable);
    m_dock_assets->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        QWidget *widget_assest_editor = new QWidget();
        widget_assest_editor->setObjectName(QStringLiteral("widgetAssestsEditor"));
        widget_assest_editor->setSizePolicy(size_policy_preferred_vertical);
            QVBoxLayout *vertical_layout_asset = new QVBoxLayout(widget_assest_editor);
            vertical_layout_asset->setObjectName(QStringLiteral("verticalLayoutAsset"));
            vertical_layout_asset->setSpacing(0);
            vertical_layout_asset->setContentsMargins(0, 0, 0, 0);

                // ***** Load our custom TreeAssets for the Asset List
                tree_assets = new TreeAssets(widget_assest_editor, project, editor_relay);
                tree_assets->setObjectName(QStringLiteral("treeAssetEditor"));
                tree_assets->setColumnCount(1);
                tree_assets->setFont(Dr::CustomFont());
                tree_assets->setProperty("showDropIndicator", false);
                tree_assets->setDragEnabled(false);
                tree_assets->setDragDropOverwriteMode(false);
                tree_assets->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
                tree_assets->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
                tree_assets->setAlternatingRowColors(false);
                tree_assets->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
                tree_assets->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
                tree_assets->setIndentation(0);
                tree_assets->setRootIsDecorated(false);
                tree_assets->setItemsExpandable(true);
                tree_assets->setExpandsOnDoubleClick(false);
                tree_assets->setHeaderHidden(true);
                tree_assets->setFrameShape(QFrame::NoFrame);
                tree_assets->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
                tree_assets->setUniformRowHeights(false);
                tree_assets->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
                tree_assets->verticalScrollBar()->setSingleStep(25);

            vertical_layout_asset->insertWidget(0, tree_assets);

        m_dock_assets->setWidget(widget_assest_editor);
    return m_dock_assets;
}


//####################################################################################
//##    Builds Inspector Dock
//####################################################################################
QDockWidget* BuildDockInspector(DrProject *project, IEditorRelay *editor_relay, TreeInspector *&tree_inspector) {
    QSizePolicy size_policy_preferred_vertical(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy_preferred_vertical.setHorizontalStretch(0);
    size_policy_preferred_vertical.setVerticalStretch(1);

    QDockWidget *dock_inspector = new QDockWidget();
    dock_inspector->setWindowTitle( QMainWindow::tr("Inspector") );
    dock_inspector->setObjectName(QStringLiteral("dockInspector"));
    dock_inspector->setSizePolicy(size_policy_preferred_vertical);
    dock_inspector->setFont(Dr::CustomFont());
    dock_inspector->setFeatures(QDockWidget::DockWidgetMovable); /// | QDockWidget::DockWidgetFloatable);
    dock_inspector->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        QWidget *widget_inspector = new QWidget();
        widget_inspector->setObjectName(QStringLiteral("widgetInspector"));
        widget_inspector->setSizePolicy(size_policy_preferred_vertical);
            QVBoxLayout *vertical_layout_inspector = new QVBoxLayout(widget_inspector);
            vertical_layout_inspector->setObjectName(QStringLiteral("verticalLayoutObject"));
            vertical_layout_inspector->setSpacing(0);
            vertical_layout_inspector->setContentsMargins(0, 0, 0, 0);

                // ***** Load our custom TreeInspector for the Stage List
                tree_inspector = new TreeInspector(widget_inspector, project, editor_relay);
                tree_inspector->setObjectName(QStringLiteral("treeObject"));
                tree_inspector->setColumnCount(1);
                tree_inspector->setFont(Dr::CustomFont());
                tree_inspector->setProperty("showDropIndicator", false);
                tree_inspector->setDragEnabled(false);
                tree_inspector->setDragDropOverwriteMode(false);
                tree_inspector->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
                tree_inspector->setDefaultDropAction(Qt::DropAction::TargetMoveAction);
                tree_inspector->setAlternatingRowColors(false);
                tree_inspector->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
                tree_inspector->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
                ///tree_inspector->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
                tree_inspector->setIndentation(0);
                tree_inspector->setRootIsDecorated(false);
                tree_inspector->setItemsExpandable(true);
                tree_inspector->setExpandsOnDoubleClick(false);
                tree_inspector->setHeaderHidden(true);
                tree_inspector->setFrameShape(QFrame::NoFrame);
                tree_inspector->setUniformRowHeights(false);
                tree_inspector->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
                tree_inspector->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                tree_inspector->verticalScrollBar()->setSingleStep(25);

            vertical_layout_inspector->addWidget(tree_inspector);
        dock_inspector->setWidget(widget_inspector);
    return dock_inspector;
}


//####################################################################################
//##    Builds Wave Form Dock
//####################################################################################
QDockWidget* BuildDockWaveForm(DrProject *project, IEditorRelay *editor_relay, TreeWaveForm *&tree_wave_form) {
    QSizePolicy size_policy_less(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy_less.setHorizontalStretch(1);
    size_policy_less.setVerticalStretch(1);

    QDockWidget *dock_wave_form = new QDockWidget(nullptr, Qt::WindowType::Window | Qt::WindowType::FramelessWindowHint);
    dock_wave_form->setWindowTitle( QMainWindow::tr("Wave Form") );
    dock_wave_form->setObjectName(QStringLiteral("dockWaveForm"));
    dock_wave_form->setSizePolicy(size_policy_less);
    dock_wave_form->setFont(Dr::CustomFont());
    dock_wave_form->setFeatures(QDockWidget::DockWidgetMovable); /// | QDockWidget::DockWidgetFloatable); /// | QDockWidget::DockWidgetClosable);
    dock_wave_form->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

        QWidget *widget_wave_form = new QWidget();
        widget_wave_form->setObjectName(QStringLiteral("widgetWaveForm"));
        widget_wave_form->setSizePolicy(size_policy_less);
        //widget_wave_form->setMaximumHeight(180);
            QVBoxLayout *vertical_layout_wave_form = new QVBoxLayout(widget_wave_form);
            vertical_layout_wave_form->setObjectName(QStringLiteral("verticalLayoutWaveForm"));
            vertical_layout_wave_form->setSpacing(0);
            vertical_layout_wave_form->setContentsMargins(0, 0, 0, 0);

                // ***** Load our custom TreeAdvisor for the helpful advisor text
                tree_wave_form = new TreeWaveForm(widget_wave_form, project, editor_relay);
                tree_wave_form->setObjectName(QStringLiteral("treeWaveForm"));
                tree_wave_form->setColumnCount(1);
                tree_wave_form->setFont(Dr::CustomFontLarger());
                tree_wave_form->setProperty("showDropIndicator", false);
                tree_wave_form->setDragEnabled(false);
                tree_wave_form->setDragDropOverwriteMode(false);
                tree_wave_form->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
                tree_wave_form->setDefaultDropAction(Qt::DropAction::IgnoreAction);
                tree_wave_form->setAlternatingRowColors(false);
                tree_wave_form->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
                tree_wave_form->setIndentation(12);
                tree_wave_form->setRootIsDecorated(false);
                tree_wave_form->setItemsExpandable(false);
                tree_wave_form->setExpandsOnDoubleClick(false);
                tree_wave_form->setHeaderHidden(true);
                tree_wave_form->setFrameShape(QFrame::NoFrame);
            vertical_layout_wave_form->addWidget(tree_wave_form);

        dock_wave_form->setWidget(widget_wave_form);
    return dock_wave_form;
}



}   // namespace Dr



















