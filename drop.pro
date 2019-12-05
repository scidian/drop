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

CONFIG += c++17
QMAKE_CFLAGS += -std=c99

SOURCES += \
    3rd_party/chipmunk/chipmunk.c \
    3rd_party/chipmunk/cpArbiter.c \
    3rd_party/chipmunk/cpArray.c \
    3rd_party/chipmunk/cpBBTree.c \
    3rd_party/chipmunk/cpBody.c \
    3rd_party/chipmunk/cpCollision.c \
    3rd_party/chipmunk/cpConstraint.c \
    3rd_party/chipmunk/cpDampedRotarySpring.c \
    3rd_party/chipmunk/cpDampedSpring.c \
    3rd_party/chipmunk/cpGearJoint.c \
    3rd_party/chipmunk/cpGrooveJoint.c \
    3rd_party/chipmunk/cpHashSet.c \
    3rd_party/chipmunk/cpHastySpace.c \
    3rd_party/chipmunk/cpMarch.c \
    3rd_party/chipmunk/cpPinJoint.c \
    3rd_party/chipmunk/cpPivotJoint.c \
    3rd_party/chipmunk/cpPolyShape.c \
    3rd_party/chipmunk/cpPolyline.c \
    3rd_party/chipmunk/cpRatchetJoint.c \
    3rd_party/chipmunk/cpRobust.c \
    3rd_party/chipmunk/cpRotaryLimitJoint.c \
    3rd_party/chipmunk/cpShape.c \
    3rd_party/chipmunk/cpSimpleMotor.c \
    3rd_party/chipmunk/cpSlideJoint.c \
    3rd_party/chipmunk/cpSpace.c \
    3rd_party/chipmunk/cpSpaceComponent.c \
    3rd_party/chipmunk/cpSpaceDebug.c \
    3rd_party/chipmunk/cpSpaceHash.c \
    3rd_party/chipmunk/cpSpaceQuery.c \
    3rd_party/chipmunk/cpSpaceStep.c \
    3rd_party/chipmunk/cpSpatialIndex.c \
    3rd_party/chipmunk/cpSweep1D.c \
    3rd_party/hull_finder.cpp \
    3rd_party/poly_partition.cpp \
    3rd_party/polyline_simplification.cpp \
    common/constants.cpp \
    common/types/point.cpp \
    common/types/pointf.cpp \
    editor/colors/colors.cpp \
    editor/colors/palette_blank.cpp \
    editor/colors/palette_material.cpp \
    editor/colors/palette_rocky_rover.cpp \
    editor/colors/palette_window_themes.cpp \
    editor/constants_editor.cpp \
    editor/docks/docks_build.cpp \
    editor/docks/docks_handle.cpp \
    editor/docks/docks_initialize.cpp \
    editor/forms/form_about.cpp \
    editor/forms/form_animation.cpp \
    editor/forms/form_animation__build.cpp \
    editor/forms/form_animation__interface_editor.cpp \
    editor/forms/form_blank.cpp \
    editor/forms/form_color_magnifier.cpp \
    editor/forms/form_expire.cpp \
    editor/forms/form_fonts.cpp \
    editor/forms/form_main.cpp \
    editor/forms/form_main__build.cpp \
    editor/forms/form_main__build_central_widget.cpp \
    editor/forms/form_main__interface_editor.cpp \
    editor/forms/form_main__menu.cpp \
    editor/forms/form_main__menu_build.cpp \
    editor/forms/form_main__toolbar.cpp \
    editor/forms/form_main__toolbar_build.cpp \
    editor/forms/form_main__toolbar_button_click.cpp \
    editor/forms/form_popup.cpp \
    editor/forms/form_popup__add_entity.cpp \
    editor/forms/form_popup__grid_snap.cpp \
    editor/forms/form_popup_colors.cpp \
    editor/forms/form_settings.cpp \
    editor/imaging/imaging_draw.cpp \
    editor/imaging/imaging_draw_devices.cpp \
    editor/imaging/imaging_draw_effects.cpp \
    editor/imaging/imaging_filters.cpp \
    editor/imaging/imaging_misc.cpp \
    editor/imaging/imaging_objects.cpp \
    editor/style/style_format_widgets.cpp \
    editor/style/style_pixmap.cpp \
    editor/style/style_sheet_main.cpp \
    editor/style/style_sheet_toolbar.cpp \
    editor/trees/tree_advisor.cpp \
    editor/trees/tree_assets.cpp \
    editor/trees/tree_assets__category_button.cpp \
    editor/trees/tree_assets__drag_drop.cpp \
    editor/trees/tree_assets__event_filter.cpp \
    editor/trees/tree_assets__key_press.cpp \
    editor/trees/tree_assets__layout.cpp \
    editor/trees/tree_assets__update.cpp \
    editor/trees/tree_inspector.cpp \
    editor/trees/tree_inspector__update.cpp \
    editor/trees/tree_inspector__update_settings.cpp \
    editor/trees/tree_inspector__widgets_checkbox.cpp \
    editor/trees/tree_inspector__widgets_color.cpp \
    editor/trees/tree_inspector__widgets_image.cpp \
    editor/trees/tree_inspector__widgets_line_edit.cpp \
    editor/trees/tree_inspector__widgets_list.cpp \
    editor/trees/tree_inspector__widgets_spinbox.cpp \
    editor/trees/tree_project.cpp \
    editor/trees/tree_project__drag_drop.cpp \
    editor/trees/tree_project__key_press.cpp \
    editor/trees/tree_project__lock_box.cpp \
    editor/trees/tree_project__selection.cpp \
    editor/trees/tree_project__update.cpp \
    editor/view/editor_item.cpp \
    editor/view/editor_item__item_change.cpp \
    editor/view/editor_item__paint.cpp \
    editor/view/editor_scene.cpp \
    editor/view/editor_scene__key_press.cpp \
    editor/view/editor_scene__select.cpp \
    editor/view/editor_scene__update.cpp \
    editor/view/editor_scene_undo_commands.cpp \
    editor/view/editor_view.cpp \
    editor/view/editor_view__drag_drop.cpp \
    editor/view/editor_view__grid.cpp \
    editor/view/editor_view__key_press.cpp \
    editor/view/editor_view__mouse.cpp \
    editor/view/editor_view__mouse_move.cpp \
    editor/view/editor_view__paint.cpp \
    editor/view/editor_view__paint_items.cpp \
    editor/view/editor_view__resize.cpp \
    editor/view/editor_view__rotate.cpp \
    editor/view/editor_view__select.cpp \
    editor/view/editor_view__tooltip.cpp \
    editor/interface_editor_relay.cpp \
    editor/widgets_event_filters.cpp \
    engine/engine.cpp \
    engine/engine_camera.cpp \
    engine/engine_spawner.cpp \
    engine/engine_texture.cpp \
    engine/form_engine.cpp \
    engine/mesh/engine_vertex_data.cpp \
    engine/mesh/engine_vertex_data__extrude.cpp \
    engine/mesh/engine_vertex_data__smooth.cpp \
    engine/opengl/opengl.cpp \
    engine/opengl/opengl_bind_fbo.cpp \
    engine/opengl/opengl_initialize.cpp \
    engine/opengl/opengl_keyboard.cpp \
    engine/opengl/opengl_math.cpp \
    engine/opengl/opengl_matrix.cpp \
    engine/opengl/opengl_mouse.cpp \
    engine/opengl/opengl_render.cpp \
    engine/opengl/opengl_render_debug.cpp \
    engine/opengl/opengl_render_debug_health.cpp \
    engine/opengl/opengl_render_effects.cpp \
    engine/opengl/opengl_render_fbo.cpp \
    engine/opengl/opengl_render_lights.cpp \
    engine/opengl/opengl_render_objects.cpp \
    engine/opengl/opengl_render_space.cpp \
    engine/things/engine_thing.cpp \
    engine/things/engine_thing_fire.cpp \
    engine/things/engine_thing_fisheye.cpp \
    engine/things/engine_thing_light.cpp \
    engine/things/engine_thing_mirror.cpp \
    engine/things/engine_thing_object.cpp \
    engine/things/engine_thing_object__create.cpp \
    engine/things/engine_thing_object__update.cpp \
    engine/things/engine_thing_swirl.cpp \
    engine/things/engine_thing_water.cpp \
    engine/world/engine_world.cpp \
    engine/world/engine_world__build_player.cpp \
    engine/world/engine_world__build_world.cpp \
    engine/world/engine_world__clear.cpp \
    engine/world/engine_world__collision.cpp \
    engine/world/engine_world__load_characters.cpp \
    engine/world/engine_world__load_effects.cpp \
    engine/world/engine_world__load_objects.cpp \
    engine/world/engine_world__load_stage.cpp \
    engine/world/engine_world__update.cpp \
    engine/world/engine_world__update_player.cpp \
    engine/world/engine_world__update_velocity.cpp \
    enums.cpp \
    helper.cpp \
    helper_qt.cpp \
    main.cpp \
    model/project/project.cpp \
    model/project/project__file_open.cpp \
    model/project/project__file_save.cpp \
    model/project/project__initialize.cpp \
    model/project/project_animation.cpp \
    model/project/project_animation_frame.cpp \
    model/project/project_asset.cpp \
    model/project/project_asset__auto_collision.cpp \
    model/project/project_asset__settings.cpp \
    model/project/project_asset__settings_shared.cpp \
    model/project/project_device.cpp \
    model/project/project_effect.cpp \
    model/project/project_font.cpp \
    model/project/project_image.cpp \
    model/project/project_stage.cpp \
    model/project/project_stage__settings.cpp \
    model/project/project_thing.cpp \
    model/project/project_thing__settings.cpp \
    model/project/project_thing__settings_effects.cpp \
    model/project/project_thing__settings_shared.cpp \
    model/project/project_thing__z_order.cpp \
    model/project/project_world.cpp \
    model/project/project_world__settings.cpp \
    model/properties/property_collision.cpp \
    settings/settings.cpp \
    settings/settings_component.cpp \
    settings/settings_component_property.cpp \
    debug.cpp \
    globals.cpp

HEADERS += \
    3rd_party/chipmunk/chipmunk.h \
    3rd_party/chipmunk/chipmunk_ffi.h \
    3rd_party/chipmunk/chipmunk_private.h \
    3rd_party/chipmunk/chipmunk_structs.h \
    3rd_party/chipmunk/chipmunk_types.h \
    3rd_party/chipmunk/chipmunk_unsafe.h \
    3rd_party/chipmunk/cpArbiter.h \
    3rd_party/chipmunk/cpBB.h \
    3rd_party/chipmunk/cpBody.h \
    3rd_party/chipmunk/cpConstraint.h \
    3rd_party/chipmunk/cpDampedRotarySpring.h \
    3rd_party/chipmunk/cpDampedSpring.h \
    3rd_party/chipmunk/cpGearJoint.h \
    3rd_party/chipmunk/cpGrooveJoint.h \
    3rd_party/chipmunk/cpHastySpace.h \
    3rd_party/chipmunk/cpMarch.h \
    3rd_party/chipmunk/cpPinJoint.h \
    3rd_party/chipmunk/cpPivotJoint.h \
    3rd_party/chipmunk/cpPolyShape.h \
    3rd_party/chipmunk/cpPolyline.h \
    3rd_party/chipmunk/cpRatchetJoint.h \
    3rd_party/chipmunk/cpRobust.h \
    3rd_party/chipmunk/cpRotaryLimitJoint.h \
    3rd_party/chipmunk/cpShape.h \
    3rd_party/chipmunk/cpSimpleMotor.h \
    3rd_party/chipmunk/cpSlideJoint.h \
    3rd_party/chipmunk/cpSpace.h \
    3rd_party/chipmunk/cpSpatialIndex.h \
    3rd_party/chipmunk/cpTransform.h \
    3rd_party/chipmunk/cpVect.h \
    3rd_party/chipmunk/prime.h \
    3rd_party/delaunator.h \
    3rd_party/hull_finder.h \
    3rd_party/poly_partition.h \
    3rd_party/polyline_simplification.h \
    3rd_party/vec3.h \
    common/constants.h \
    common/types/point.h \
    common/types/pointf.h \
    editor/colors/colors.h \
    editor/constants_editor.h \
    editor/docks/docks.h \
    editor/event_filters.h \
    editor/forms/form_about.h \
    editor/forms/form_animation.h \
    editor/forms/form_blank.h \
    editor/forms/form_color_magnifier.h \
    editor/forms/form_expire.h \
    editor/forms/form_fonts.h \
    editor/forms/form_main.h \
    editor/forms/form_popup.h \
    editor/forms/form_settings.h \
    editor/imaging/imaging.h \
    editor/style/style.h \
    editor/trees/tree_advisor.h \
    editor/trees/tree_assets.h \
    editor/trees/tree_inspector.h \
    editor/trees/tree_project.h \
    editor/view/editor_item.h \
    editor/view/editor_scene.h \
    editor/view/editor_scene_undo_commands.h \
    editor/view/editor_view.h \
    editor/interface_editor_relay.h \
    engine/constants_engine.h \
    engine/engine.h \
    engine/engine_camera.h \
    engine/engine_spawner.h \
    engine/engine_texture.h \
    engine/form_engine.h \
    engine/mesh/engine_mesh.h \
    engine/mesh/engine_vertex_data.h \
    engine/opengl/opengl.h \
    engine/things/engine_thing.h \
    engine/things/engine_thing_fire.h \
    engine/things/engine_thing_fisheye.h \
    engine/things/engine_thing_light.h \
    engine/things/engine_thing_mirror.h \
    engine/things/engine_thing_object.h \
    engine/things/engine_thing_swirl.h \
    engine/things/engine_thing_water.h \
    engine/world/engine_world.h \
    enums.h \
    enums_dr_settings.h \
    enums_engine.h \
    globals_engine.h \
    helper.h \
    helper_qt.h \
    model/constants_keys.h \
    model/project/project.h \
    model/project/project_animation.h \
    model/project/project_asset.h \
    model/project/project_device.h \
    model/project/project_effect.h \
    model/project/project_font.h \
    model/project/project_image.h \
    model/project/project_stage.h \
    model/project/project_thing.h \
    model/project/project_world.h \
    model/properties/property_collision.h \
    settings/settings.h \
    settings/settings_component.h \
    settings/settings_component_property.h \
    debug.h \
    globals.h \
    enums_form_main.h

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


