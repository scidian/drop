#-------------------------------------------------
#
# Project created by QtCreator 2018-11-27T11:20:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Drop
TEMPLATE = app

# The following define makes your compiler show warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
QMAKE_CFLAGS += -std=c99

SOURCES += \
    3rd_party/hull_finder.cpp \
    3rd_party/poly_partition.cpp \
    3rd_party/polyline_simplification.cpp \
    3rd_party_chipmunk/chipmunk.c \
    3rd_party_chipmunk/cpArbiter.c \
    3rd_party_chipmunk/cpArray.c \
    3rd_party_chipmunk/cpBBTree.c \
    3rd_party_chipmunk/cpBody.c \
    3rd_party_chipmunk/cpCollision.c \
    3rd_party_chipmunk/cpConstraint.c \
    3rd_party_chipmunk/cpDampedRotarySpring.c \
    3rd_party_chipmunk/cpDampedSpring.c \
    3rd_party_chipmunk/cpGearJoint.c \
    3rd_party_chipmunk/cpGrooveJoint.c \
    3rd_party_chipmunk/cpHashSet.c \
    3rd_party_chipmunk/cpHastySpace.c \
    3rd_party_chipmunk/cpMarch.c \
    3rd_party_chipmunk/cpPinJoint.c \
    3rd_party_chipmunk/cpPivotJoint.c \
    3rd_party_chipmunk/cpPolyShape.c \
    3rd_party_chipmunk/cpPolyline.c \
    3rd_party_chipmunk/cpRatchetJoint.c \
    3rd_party_chipmunk/cpRobust.c \
    3rd_party_chipmunk/cpRotaryLimitJoint.c \
    3rd_party_chipmunk/cpShape.c \
    3rd_party_chipmunk/cpSimpleMotor.c \
    3rd_party_chipmunk/cpSlideJoint.c \
    3rd_party_chipmunk/cpSpace.c \
    3rd_party_chipmunk/cpSpaceComponent.c \
    3rd_party_chipmunk/cpSpaceDebug.c \
    3rd_party_chipmunk/cpSpaceHash.c \
    3rd_party_chipmunk/cpSpaceQuery.c \
    3rd_party_chipmunk/cpSpaceStep.c \
    3rd_party_chipmunk/cpSpatialIndex.c \
    3rd_party_chipmunk/cpSweep1D.c \
    colors/colors.cpp \
    colors/colors_palette_blank.cpp \
    colors/colors_palette_material.cpp \
    colors/colors_palette_rocky_rover.cpp \
    colors/colors_palette_window_themes.cpp \
    editor/tree_advisor.cpp \
    editor/tree_assets.cpp \
    editor/tree_assets__drag_drop.cpp \
    editor/tree_assets__event_filter.cpp \
    editor/tree_assets__key_press.cpp \
    editor/tree_assets__update.cpp \
    editor/tree_inspector.cpp \
    editor/tree_inspector__update.cpp \
    editor/tree_inspector__widgets_checkbox.cpp \
    editor/tree_inspector__widgets_color.cpp \
    editor/tree_inspector__widgets_image.cpp \
    editor/tree_inspector__widgets_line_edit.cpp \
    editor/tree_inspector__widgets_list.cpp \
    editor/tree_inspector__widgets_spinbox.cpp \
    editor/tree_project.cpp \
    editor/tree_project__drag_drop.cpp \
    editor/tree_project__key_press.cpp \
    editor/tree_project__lock_box.cpp \
    editor/tree_project__selection.cpp \
    editor/tree_project__update.cpp \
    editor_view/editor_item.cpp \
    editor_view/editor_item__item_change.cpp \
    editor_view/editor_item__paint.cpp \
    editor_view/editor_scene.cpp \
    editor_view/editor_scene__key_press.cpp \
    editor_view/editor_scene__select.cpp \
    editor_view/editor_scene__update.cpp \
    editor_view/editor_scene_undo_commands.cpp \
    editor_view/editor_view.cpp \
    editor_view/editor_view__drag_drop.cpp \
    editor_view/editor_view__grid.cpp \
    editor_view/editor_view__key_press.cpp \
    editor_view/editor_view__mouse.cpp \
    editor_view/editor_view__mouse_move.cpp \
    editor_view/editor_view__paint.cpp \
    editor_view/editor_view__paint_items.cpp \
    editor_view/editor_view__resize.cpp \
    editor_view/editor_view__rotate.cpp \
    editor_view/editor_view__select.cpp \
    editor_view/editor_view__tooltip.cpp \
    engine/engine.cpp \
    engine/engine_camera.cpp \
    engine/engine_spawner.cpp \
    engine/engine_texture.cpp \
    engine/engine_world.cpp \
    engine/engine_world__build_player.cpp \
    engine/engine_world__build_world.cpp \
    engine/engine_world__clear.cpp \
    engine/engine_world__collision.cpp \
    engine/engine_world__load_characters.cpp \
    engine/engine_world__load_effects.cpp \
    engine/engine_world__load_objects.cpp \
    engine/engine_world__load_stage.cpp \
    engine/engine_world__update.cpp \
    engine/engine_world__update_player.cpp \
    engine/engine_world__update_velocity.cpp \
    engine/form_engine.cpp \
    engine_mesh/engine_vertex_data.cpp \
    engine_mesh/engine_vertex_data__extrude.cpp \
    engine_mesh/engine_vertex_data__smooth.cpp \
    engine_things/engine_thing.cpp \
    engine_things/engine_thing_fire.cpp \
    engine_things/engine_thing_fisheye.cpp \
    engine_things/engine_thing_light.cpp \
    engine_things/engine_thing_mirror.cpp \
    engine_things/engine_thing_object.cpp \
    engine_things/engine_thing_object__create.cpp \
    engine_things/engine_thing_object__update.cpp \
    engine_things/engine_thing_swirl.cpp \
    engine_things/engine_thing_water.cpp \
    enums.cpp \
    forms/form_about.cpp \
    forms/form_blank.cpp \
    forms/form_color_magnifier.cpp \
    forms/form_expire.cpp \
    forms/form_fonts.cpp \
    forms/form_main.cpp \
    forms/form_main__build.cpp \
    forms/form_main__build_editor.cpp \
    forms/form_main__interface_editor.cpp \
    forms/form_main__menu.cpp \
    forms/form_main__menu_build.cpp \
    forms/form_main__toolbar.cpp \
    forms/form_main__toolbar_build.cpp \
    forms/form_main__toolbar_button_click.cpp \
    forms/form_popup.cpp \
    forms/form_popup__add_entity.cpp \
    forms/form_popup__grid_snap.cpp \
    forms/form_popup_colors.cpp \
    forms/form_settings.cpp \
    helper.cpp \
    helper_qt.cpp \
    imaging/imaging_draw.cpp \
    imaging/imaging_draw_devices.cpp \
    imaging/imaging_draw_effects.cpp \
    imaging/imaging_filters.cpp \
    imaging/imaging_misc.cpp \
    imaging/imaging_objects.cpp \
    main.cpp \
    opengl/opengl.cpp \
    opengl/opengl_bind_fbo.cpp \
    opengl/opengl_initialize.cpp \
    opengl/opengl_keyboard.cpp \
    opengl/opengl_math.cpp \
    opengl/opengl_matrix.cpp \
    opengl/opengl_mouse.cpp \
    opengl/opengl_render.cpp \
    opengl/opengl_render_debug.cpp \
    opengl/opengl_render_debug_health.cpp \
    opengl/opengl_render_effects.cpp \
    opengl/opengl_render_fbo.cpp \
    opengl/opengl_render_lights.cpp \
    opengl/opengl_render_objects.cpp \
    opengl/opengl_render_space.cpp \
    playground/form_playground.cpp \
    playground/form_playground__build.cpp \
    playground/form_playground__items.cpp \
    playground/form_playground__view.cpp \
    playground/playground.cpp \
    playground/playground__add_toy.cpp \
    playground/playground__build.cpp \
    playground/playground__clear.cpp \
    playground/playground__update.cpp \
    project/project.cpp \
    project/project__file_open.cpp \
    project/project__file_save.cpp \
    project/project__initialize.cpp \
    project/project_asset.cpp \
    project/project_asset__settings.cpp \
    project/project_asset__settings_shared.cpp \
    project/project_device.cpp \
    project/project_effect.cpp \
    project/project_font.cpp \
    project/project_image.cpp \
    project/project_stage.cpp \
    project/project_stage__settings.cpp \
    project/project_thing.cpp \
    project/project_thing__settings.cpp \
    project/project_thing__settings_effects.cpp \
    project/project_thing__settings_shared.cpp \
    project/project_thing__z_order.cpp \
    project/project_world.cpp \
    project/project_world__settings.cpp \
    properties/thing_shape_list.cpp \
    settings/settings.cpp \
    settings/settings_component.cpp \
    settings/settings_component_property.cpp \
    debug.cpp \
    constants.cpp \
    interface_editor_relay.cpp \
    globals.cpp \
    style/style_format_widgets.cpp \
    style/style_pixmap.cpp \
    style/style_sheet_main.cpp \
    style/style_sheet_toolbar.cpp \
    types/point.cpp \
    types/pointf.cpp \
    widgets/widgets.cpp \
    widgets/widgets_event_filters.cpp \
    widgets/widgets_layout.cpp

HEADERS += \
    3rd_party/delaunator.h \
    3rd_party/hull_finder.h \
    3rd_party/poly_partition.h \
    3rd_party/polyline_simplification.h \
    3rd_party/vec3.h \
    3rd_party_chipmunk/chipmunk.h \
    3rd_party_chipmunk/chipmunk_ffi.h \
    3rd_party_chipmunk/chipmunk_private.h \
    3rd_party_chipmunk/chipmunk_structs.h \
    3rd_party_chipmunk/chipmunk_types.h \
    3rd_party_chipmunk/chipmunk_unsafe.h \
    3rd_party_chipmunk/cpArbiter.h \
    3rd_party_chipmunk/cpBB.h \
    3rd_party_chipmunk/cpBody.h \
    3rd_party_chipmunk/cpConstraint.h \
    3rd_party_chipmunk/cpDampedRotarySpring.h \
    3rd_party_chipmunk/cpDampedSpring.h \
    3rd_party_chipmunk/cpGearJoint.h \
    3rd_party_chipmunk/cpGrooveJoint.h \
    3rd_party_chipmunk/cpHastySpace.h \
    3rd_party_chipmunk/cpMarch.h \
    3rd_party_chipmunk/cpPinJoint.h \
    3rd_party_chipmunk/cpPivotJoint.h \
    3rd_party_chipmunk/cpPolyShape.h \
    3rd_party_chipmunk/cpPolyline.h \
    3rd_party_chipmunk/cpRatchetJoint.h \
    3rd_party_chipmunk/cpRobust.h \
    3rd_party_chipmunk/cpRotaryLimitJoint.h \
    3rd_party_chipmunk/cpShape.h \
    3rd_party_chipmunk/cpSimpleMotor.h \
    3rd_party_chipmunk/cpSlideJoint.h \
    3rd_party_chipmunk/cpSpace.h \
    3rd_party_chipmunk/cpSpatialIndex.h \
    3rd_party_chipmunk/cpTransform.h \
    3rd_party_chipmunk/cpVect.h \
    3rd_party_chipmunk/prime.h \
    colors/colors.h \
    constants_engine.h \
    editor/tree_advisor.h \
    editor/tree_assets.h \
    editor/tree_inspector.h \
    editor/tree_project.h \
    editor_view/editor_item.h \
    editor_view/editor_scene.h \
    editor_view/editor_scene_undo_commands.h \
    editor_view/editor_view.h \
    engine/engine.h \
    engine/engine_camera.h \
    engine/engine_spawner.h \
    engine/engine_texture.h \
    engine/engine_world.h \
    engine/form_engine.h \
    engine_mesh/engine_mesh.h \
    engine_mesh/engine_vertex_data.h \
    engine_things/engine_thing.h \
    engine_things/engine_thing_fire.h \
    engine_things/engine_thing_fisheye.h \
    engine_things/engine_thing_light.h \
    engine_things/engine_thing_mirror.h \
    engine_things/engine_thing_object.h \
    engine_things/engine_thing_swirl.h \
    engine_things/engine_thing_water.h \
    enums.h \
    enums_dr_settings.h \
    enums_engine.h \
    forms/form_about.h \
    forms/form_blank.h \
    forms/form_color_magnifier.h \
    forms/form_expire.h \
    forms/form_fonts.h \
    forms/form_main.h \
    forms/form_popup.h \
    forms/form_settings.h \
    globals_engine.h \
    helper.h \
    helper_qt.h \
    imaging/imaging.h \
    opengl/opengl.h \
    playground/form_playground.h \
    playground/playground.h \
    playground/playground_toy.h \
    project/project.h \
    project/project_asset.h \
    project/project_device.h \
    project/project_effect.h \
    project/project_font.h \
    project/project_image.h \
    project/project_stage.h \
    project/project_thing.h \
    project/project_world.h \
    properties/thing_shape_list.h \
    settings/settings.h \
    settings/settings_component.h \
    settings/settings_component_property.h \
    debug.h \
    constants.h \
    interface_editor_relay.h \
    globals.h \
    enums_form_main.h \
    style/style.h \
    types/point.h \
    types/pointf.h \
    widgets/widgets.h \
    widgets/widgets_event_filters.h \
    widgets/widgets_layout.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc \
    shaders.qrc

DISTFILES += \
    drop_icon.ico \
    drop_icons.icns

ICON = drop_icons.icns
RC_ICONS = drop_icon.ico


