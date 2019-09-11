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
    3rd_party/hullfinder.cpp \
    3rd_party/poly_partition.cpp \
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
    editor_view/editor_item.cpp \
    editor_view/editor_item_item_change.cpp \
    editor_view/editor_item_paint.cpp \
    editor_view/editor_scene.cpp \
    editor_view/editor_scene_key_press.cpp \
    editor_view/editor_scene_select.cpp \
    editor_view/editor_scene_undo_commands.cpp \
    editor_view/editor_scene_update.cpp \
    editor_view/editor_view.cpp \
    editor_view/editor_view_drag_drop.cpp \
    editor_view/editor_view_grid.cpp \
    editor_view/editor_view_key_press.cpp \
    editor_view/editor_view_mouse.cpp \
    editor_view/editor_view_mouse_move.cpp \
    editor_view/editor_view_paint.cpp \
    editor_view/editor_view_paint_items.cpp \
    editor_view/editor_view_resize.cpp \
    editor_view/editor_view_rotate.cpp \
    editor_view/editor_view_select.cpp \
    editor_view/editor_view_tooltip.cpp \
    engine/engine.cpp \
    engine/engine_camera.cpp \
    engine/engine_texture.cpp \
    engine/engine_world.cpp \
    engine/engine_world_build_player.cpp \
    engine/engine_world_build_world.cpp \
    engine/engine_world_clear.cpp \
    engine/engine_world_collision.cpp \
    engine/engine_world_load.cpp \
    engine/engine_world_update.cpp \
    engine/engine_world_update_player.cpp \
    engine/form_engine.cpp \
    engine_mesh/engine_vertex_data.cpp \
    engine_mesh/engine_vertex_data_extrude.cpp \
    engine_things/engine_thing.cpp \
    engine_things/engine_thing_fire.cpp \
    engine_things/engine_thing_fisheye.cpp \
    engine_things/engine_thing_light.cpp \
    engine_things/engine_thing_mirror.cpp \
    engine_things/engine_thing_object.cpp \
    engine_things/engine_thing_object_create.cpp \
    engine_things/engine_thing_swirl.cpp \
    engine_things/engine_thing_water.cpp \
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
    style/style_pixmap.cpp \
    style/style_sheet_main.cpp \
    style/style_sheet_toolbar.cpp \
    widgets/widgets.cpp \
    widgets/widgets_event_filters.cpp \
    widgets/widgets_layout.cpp

HEADERS += \
    3rd_party/delaunator.h \
    3rd_party/hullfinder.h \
    3rd_party/poly_partition.h \
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
    editor/editor_tree_advisor.h \
    editor/editor_tree_assets.h \
    editor/editor_tree_inspector.h \
    editor/editor_tree_project.h \
    editor_view/editor_item.h \
    editor_view/editor_scene.h \
    editor_view/editor_scene_undo_commands.h \
    editor_view/editor_view.h \
    engine/engine.h \
    engine/engine_camera.h \
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
    forms/form_blank.h \
    forms/form_color_magnifier.h \
    forms/form_fonts.h \
    forms/form_main.h \
    forms/form_popup.h \
    forms/form_settings.h \
    helper.h \
    imaging/imaging.h \
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
