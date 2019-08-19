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

SOURCES += \
    chipmunk/chipmunk.c \
    chipmunk/cpArbiter.c \
    chipmunk/cpArray.c \
    chipmunk/cpBBTree.c \
    chipmunk/cpBody.c \
    chipmunk/cpCollision.c \
    chipmunk/cpConstraint.c \
    chipmunk/cpDampedRotarySpring.c \
    chipmunk/cpDampedSpring.c \
    chipmunk/cpGearJoint.c \
    chipmunk/cpGrooveJoint.c \
    chipmunk/cpHashSet.c \
    chipmunk/cpHastySpace.c \
    chipmunk/cpMarch.c \
    chipmunk/cpPinJoint.c \
    chipmunk/cpPivotJoint.c \
    chipmunk/cpPolyShape.c \
    chipmunk/cpPolyline.c \
    chipmunk/cpRatchetJoint.c \
    chipmunk/cpRobust.c \
    chipmunk/cpRotaryLimitJoint.c \
    chipmunk/cpShape.c \
    chipmunk/cpSimpleMotor.c \
    chipmunk/cpSlideJoint.c \
    chipmunk/cpSpace.c \
    chipmunk/cpSpaceComponent.c \
    chipmunk/cpSpaceDebug.c \
    chipmunk/cpSpaceHash.c \
    chipmunk/cpSpaceQuery.c \
    chipmunk/cpSpaceStep.c \
    chipmunk/cpSpatialIndex.c \
    chipmunk/cpSweep1D.c \
    colors/colors.cpp \
    colors/colors_palette_blank.cpp \
    colors/colors_palette_material.cpp \
    colors/colors_palette_rocky_rover.cpp \
    colors/colors_palette_window_themes.cpp \
    editor/editor_item.cpp \
    editor/editor_item_item_change.cpp \
    editor/editor_item_paint.cpp \
    editor/editor_scene.cpp \
    editor/editor_scene_key_press.cpp \
    editor/editor_scene_select.cpp \
    editor/editor_scene_undo_commands.cpp \
    editor/editor_scene_update.cpp \
    editor/editor_tree_advisor.cpp \
    editor/editor_tree_assets.cpp \
    editor/editor_tree_assets_mouse.cpp \
    editor/editor_tree_assets_update.cpp \
    editor/editor_tree_inspector.cpp \
    editor/editor_tree_inspector_update.cpp \
    editor/editor_tree_inspector_widgets.cpp \
    editor/editor_tree_inspector_widgets2.cpp \
    editor/editor_tree_inspector_widgets3.cpp \
    editor/editor_tree_project.cpp \
    editor/editor_tree_project_drag_drop.cpp \
    editor/editor_tree_project_selection.cpp \
    editor/editor_tree_project_update.cpp \
    editor/editor_view.cpp \
    editor/editor_view_drag_drop.cpp \
    editor/editor_view_grid.cpp \
    editor/editor_view_key_press.cpp \
    editor/editor_view_mouse.cpp \
    editor/editor_view_mouse_move.cpp \
    editor/editor_view_paint.cpp \
    editor/editor_view_paint_items.cpp \
    editor/editor_view_resize.cpp \
    editor/editor_view_rotate.cpp \
    editor/editor_view_select.cpp \
    editor/editor_view_tooltip.cpp \
    engine/engine.cpp \
    engine/engine_camera.cpp \
    engine/engine_texture.cpp \
    engine/engine_thing.cpp \
    engine/engine_thing_fire.cpp \
    engine/engine_thing_fisheye.cpp \
    engine/engine_thing_light.cpp \
    engine/engine_thing_mirror.cpp \
    engine/engine_thing_object.cpp \
    engine/engine_thing_object_create.cpp \
    engine/engine_thing_water.cpp \
    engine/engine_world.cpp \
    engine/engine_world_build_player.cpp \
    engine/engine_world_build_world.cpp \
    engine/engine_world_clear.cpp \
    engine/engine_world_collision.cpp \
    engine/engine_world_load.cpp \
    engine/engine_world_update.cpp \
    engine/engine_world_update_player.cpp \
    engine/form_engine.cpp \
    enums.cpp \
    forms/form_blank.cpp \
    forms/form_color_magnifier.cpp \
    forms/form_fonts.cpp \
    forms/form_main.cpp \
    forms/form_main_build.cpp \
    forms/form_main_build_editor.cpp \
    forms/form_main_interface_editor.cpp \
    forms/form_main_menu.cpp \
    forms/form_main_toolbar.cpp \
    forms/form_main_toolbar_build.cpp \
    forms/form_main_toolbar_button_click.cpp \
    forms/form_popup.cpp \
    forms/form_popup_colors.cpp \
    forms/form_popup_grid_snap.cpp \
    forms/form_settings.cpp \
    helper.cpp \
    image_filter.cpp \
    library/poly_partition.cpp \
    main.cpp \
    opengl/opengl.cpp \
    opengl/opengl_initialize.cpp \
    opengl/opengl_keyboard.cpp \
    opengl/opengl_math.cpp \
    opengl/opengl_mouse.cpp \
    opengl/opengl_render.cpp \
    opengl/opengl_render_bind.cpp \
    opengl/opengl_render_debug.cpp \
    opengl/opengl_render_debug_health.cpp \
    opengl/opengl_render_effects.cpp \
    opengl/opengl_render_lights.cpp \
    opengl/opengl_render_objects.cpp \
    opengl/opengl_render_space.cpp \
    opengl/opengl_render_test_cube.cpp \
    playground/form_playground.cpp \
    playground/form_playground_build.cpp \
    playground/form_playground_items.cpp \
    playground/form_playground_view.cpp \
    playground/playground.cpp \
    playground/playground_add_toy.cpp \
    playground/playground_build.cpp \
    playground/playground_clear.cpp \
    playground/playground_update.cpp \
    project/project.cpp \
    project/project_asset.cpp \
    project/project_effect.cpp \
    project/project_font.cpp \
    project/project_image.cpp \
    project/project_world.cpp \
    project/project_world_stage.cpp \
    project/project_world_stage_thing.cpp \
    settings/settings.cpp \
    settings/settings_component.cpp \
    settings/settings_component_property.cpp \
    debug.cpp \
    constants.cpp \
    interface_editor_relay.cpp \
    globals.cpp \
    style/style_format_widgets.cpp \
    style/style_sheet_main.cpp \
    style/style_sheet_toolbar.cpp \
    widgets/widgets.cpp \
    widgets/widgets_event_filters.cpp \
    widgets/widgets_layout.cpp

HEADERS += \
    chipmunk/chipmunk.h \
    chipmunk/chipmunk_ffi.h \
    chipmunk/chipmunk_private.h \
    chipmunk/chipmunk_structs.h \
    chipmunk/chipmunk_types.h \
    chipmunk/chipmunk_unsafe.h \
    chipmunk/cpArbiter.h \
    chipmunk/cpBB.h \
    chipmunk/cpBody.h \
    chipmunk/cpConstraint.h \
    chipmunk/cpDampedRotarySpring.h \
    chipmunk/cpDampedSpring.h \
    chipmunk/cpGearJoint.h \
    chipmunk/cpGrooveJoint.h \
    chipmunk/cpHastySpace.h \
    chipmunk/cpMarch.h \
    chipmunk/cpPinJoint.h \
    chipmunk/cpPivotJoint.h \
    chipmunk/cpPolyShape.h \
    chipmunk/cpPolyline.h \
    chipmunk/cpRatchetJoint.h \
    chipmunk/cpRobust.h \
    chipmunk/cpRotaryLimitJoint.h \
    chipmunk/cpShape.h \
    chipmunk/cpSimpleMotor.h \
    chipmunk/cpSlideJoint.h \
    chipmunk/cpSpace.h \
    chipmunk/cpSpatialIndex.h \
    chipmunk/cpTransform.h \
    chipmunk/cpVect.h \
    chipmunk/prime.h \
    colors/colors.h \
    editor/editor_item.h \
    editor/editor_scene.h \
    editor/editor_scene_undo_commands.h \
    editor/editor_tree_advisor.h \
    editor/editor_tree_assets.h \
    editor/editor_tree_inspector.h \
    editor/editor_tree_project.h \
    editor/editor_view.h \
    engine/engine.h \
    engine/engine_camera.h \
    engine/engine_texture.h \
    engine/engine_thing.h \
    engine/engine_thing_fire.h \
    engine/engine_thing_fisheye.h \
    engine/engine_thing_light.h \
    engine/engine_thing_mirror.h \
    engine/engine_thing_object.h \
    engine/engine_thing_water.h \
    engine/engine_world.h \
    engine/form_engine.h \
    enums.h \
    enums_dr_settings.h \
    enums_engine.h \
    forms/form_blank.h \
    forms/form_color_magnifier.h \
    forms/form_fonts.h \
    forms/form_main.h \
    forms/form_popup.h \
    forms/form_settings.h \
    helper.h \
    image_filter.h \
    library/poly_partition.h \
    opengl/opengl.h \
    playground/form_playground.h \
    playground/playground.h \
    playground/playground_toy.h \
    project/project.h \
    project/project_asset.h \
    project/project_effect.h \
    project/project_font.h \
    project/project_image.h \
    project/project_world.h \
    project/project_world_stage.h \
    project/project_world_stage_thing.h \
    settings/settings.h \
    settings/settings_component.h \
    settings/settings_component_property.h \
    debug.h \
    constants.h \
    interface_editor_relay.h \
    globals.h \
    enums_form_main.h \
    style/style.h \
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

DISTFILES +=
