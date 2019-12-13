####################################################################################
##
##                   DROP
##           by Scidian Software
##
############################
TARGET =    Drop
TEMPLATE =  app
CONFIG +=   c++11

##  C Make Flags
QMAKE_CFLAGS    += -std=c99

##  C++ Make Flags
QMAKE_CXXFLAGS  += -Wno-sign-conversion



##
## !!!!! Specify Editor or Engine Only !!!!!
##
CONFIG  += drop_editor

CONFIG(drop_editor){
    QT              +=  core gui

    greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

    # The following define makes your compiler show warnings if you use
    # any feature of Qt which has been marked as deprecated (the exact warnings
    # depend on your compiler). Please consult the documentation of the
    # deprecated API in order to know how to port your code away from it.
    DEFINES         +=  QT_DEPRECATED_WARNINGS

    # You can also make your code fail to compile if you use deprecated APIs.
    # In order to do so, uncomment the following line.
    # You can also select to disable deprecated APIs only up to a certain version of Qt.
    #DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
}




# Default rules for deployment.
qnx:  target.path = /tmp/$${TARGET}/bin
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



##### Additional PATHs
INCLUDEPATH += $$PWD/3rd_party


##### Sources
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
    editor/colors/colors.cpp \
    editor/colors/palette_blank.cpp \
    editor/colors/palette_material.cpp \
    editor/colors/palette_rocky_rover.cpp \
    editor/colors/palette_window_themes.cpp \
    editor/constants_editor.cpp \
    editor/debug.cpp \
    editor/docks/docks_build.cpp \
    editor/docks/docks_handle.cpp \
    editor/docks/docks_initialize.cpp \
    editor/enums_editor.cpp \
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
    editor/globals_editor.cpp \
    editor/helper_library.cpp \
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
    engine/enums_engine.cpp \
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
    library/dr_containers.cpp \
    library/dr_debug.cpp \
    library/dr_math.cpp \
    library/dr_random.cpp \
    library/dr_string.cpp \
    library/dr_time.cpp \
    library/types/dr_point.cpp \
    library/types/dr_pointf.cpp \
    library/types/dr_variant.cpp \
    main.cpp \
    model/enums_model_types.cpp \
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
    model/settings/settings.cpp \
    model/settings/settings_component.cpp \
    model/settings/settings_component_property.cpp


HEADERS += \
    3rd_party/boost/any.hpp \
    3rd_party/boost/assert.hpp \
    3rd_party/boost/config.hpp \
    3rd_party/boost/config/abi/borland_prefix.hpp \
    3rd_party/boost/config/abi/borland_suffix.hpp \
    3rd_party/boost/config/abi/msvc_prefix.hpp \
    3rd_party/boost/config/abi/msvc_suffix.hpp \
    3rd_party/boost/config/abi_prefix.hpp \
    3rd_party/boost/config/abi_suffix.hpp \
    3rd_party/boost/config/auto_link.hpp \
    3rd_party/boost/config/compiler/borland.hpp \
    3rd_party/boost/config/compiler/clang.hpp \
    3rd_party/boost/config/compiler/codegear.hpp \
    3rd_party/boost/config/compiler/comeau.hpp \
    3rd_party/boost/config/compiler/common_edg.hpp \
    3rd_party/boost/config/compiler/compaq_cxx.hpp \
    3rd_party/boost/config/compiler/cray.hpp \
    3rd_party/boost/config/compiler/diab.hpp \
    3rd_party/boost/config/compiler/digitalmars.hpp \
    3rd_party/boost/config/compiler/gcc.hpp \
    3rd_party/boost/config/compiler/gcc_xml.hpp \
    3rd_party/boost/config/compiler/greenhills.hpp \
    3rd_party/boost/config/compiler/hp_acc.hpp \
    3rd_party/boost/config/compiler/intel.hpp \
    3rd_party/boost/config/compiler/kai.hpp \
    3rd_party/boost/config/compiler/metrowerks.hpp \
    3rd_party/boost/config/compiler/mpw.hpp \
    3rd_party/boost/config/compiler/nvcc.hpp \
    3rd_party/boost/config/compiler/pathscale.hpp \
    3rd_party/boost/config/compiler/pgi.hpp \
    3rd_party/boost/config/compiler/sgi_mipspro.hpp \
    3rd_party/boost/config/compiler/sunpro_cc.hpp \
    3rd_party/boost/config/compiler/vacpp.hpp \
    3rd_party/boost/config/compiler/visualc.hpp \
    3rd_party/boost/config/compiler/xlcpp.hpp \
    3rd_party/boost/config/compiler/xlcpp_zos.hpp \
    3rd_party/boost/config/detail/posix_features.hpp \
    3rd_party/boost/config/detail/select_compiler_config.hpp \
    3rd_party/boost/config/detail/select_platform_config.hpp \
    3rd_party/boost/config/detail/select_stdlib_config.hpp \
    3rd_party/boost/config/detail/suffix.hpp \
    3rd_party/boost/config/header_deprecated.hpp \
    3rd_party/boost/config/helper_macros.hpp \
    3rd_party/boost/config/no_tr1/cmath.hpp \
    3rd_party/boost/config/no_tr1/complex.hpp \
    3rd_party/boost/config/no_tr1/functional.hpp \
    3rd_party/boost/config/no_tr1/memory.hpp \
    3rd_party/boost/config/no_tr1/utility.hpp \
    3rd_party/boost/config/platform/aix.hpp \
    3rd_party/boost/config/platform/amigaos.hpp \
    3rd_party/boost/config/platform/beos.hpp \
    3rd_party/boost/config/platform/bsd.hpp \
    3rd_party/boost/config/platform/cloudabi.hpp \
    3rd_party/boost/config/platform/cray.hpp \
    3rd_party/boost/config/platform/cygwin.hpp \
    3rd_party/boost/config/platform/haiku.hpp \
    3rd_party/boost/config/platform/hpux.hpp \
    3rd_party/boost/config/platform/irix.hpp \
    3rd_party/boost/config/platform/linux.hpp \
    3rd_party/boost/config/platform/macos.hpp \
    3rd_party/boost/config/platform/qnxnto.hpp \
    3rd_party/boost/config/platform/solaris.hpp \
    3rd_party/boost/config/platform/symbian.hpp \
    3rd_party/boost/config/platform/vms.hpp \
    3rd_party/boost/config/platform/vxworks.hpp \
    3rd_party/boost/config/platform/win32.hpp \
    3rd_party/boost/config/platform/zos.hpp \
    3rd_party/boost/config/pragma_message.hpp \
    3rd_party/boost/config/requires_threads.hpp \
    3rd_party/boost/config/stdlib/dinkumware.hpp \
    3rd_party/boost/config/stdlib/libcomo.hpp \
    3rd_party/boost/config/stdlib/libcpp.hpp \
    3rd_party/boost/config/stdlib/libstdcpp3.hpp \
    3rd_party/boost/config/stdlib/modena.hpp \
    3rd_party/boost/config/stdlib/msl.hpp \
    3rd_party/boost/config/stdlib/roguewave.hpp \
    3rd_party/boost/config/stdlib/sgi.hpp \
    3rd_party/boost/config/stdlib/stlport.hpp \
    3rd_party/boost/config/stdlib/vacpp.hpp \
    3rd_party/boost/config/stdlib/xlcpp_zos.hpp \
    3rd_party/boost/config/user.hpp \
    3rd_party/boost/config/warning_disable.hpp \
    3rd_party/boost/config/workaround.hpp \
    3rd_party/boost/container_hash/detail/float_functions.hpp \
    3rd_party/boost/container_hash/detail/hash_float.hpp \
    3rd_party/boost/container_hash/detail/limits.hpp \
    3rd_party/boost/container_hash/extensions.hpp \
    3rd_party/boost/container_hash/hash.hpp \
    3rd_party/boost/container_hash/hash_fwd.hpp \
    3rd_party/boost/core/addressof.hpp \
    3rd_party/boost/core/demangle.hpp \
    3rd_party/boost/core/enable_if.hpp \
    3rd_party/boost/core/no_exceptions_support.hpp \
    3rd_party/boost/cstdint.hpp \
    3rd_party/boost/current_function.hpp \
    3rd_party/boost/detail/container_fwd.hpp \
    3rd_party/boost/detail/workaround.hpp \
    3rd_party/boost/exception/exception.hpp \
    3rd_party/boost/integer/static_log2.hpp \
    3rd_party/boost/integer_fwd.hpp \
    3rd_party/boost/limits.hpp \
    3rd_party/boost/move/algo/move.hpp \
    3rd_party/boost/move/algorithm.hpp \
    3rd_party/boost/move/core.hpp \
    3rd_party/boost/move/detail/config_begin.hpp \
    3rd_party/boost/move/detail/config_end.hpp \
    3rd_party/boost/move/detail/iterator_to_raw_pointer.hpp \
    3rd_party/boost/move/detail/iterator_traits.hpp \
    3rd_party/boost/move/detail/meta_utils.hpp \
    3rd_party/boost/move/detail/meta_utils_core.hpp \
    3rd_party/boost/move/detail/pointer_element.hpp \
    3rd_party/boost/move/detail/std_ns_begin.hpp \
    3rd_party/boost/move/detail/std_ns_end.hpp \
    3rd_party/boost/move/detail/to_raw_pointer.hpp \
    3rd_party/boost/move/detail/type_traits.hpp \
    3rd_party/boost/move/detail/workaround.hpp \
    3rd_party/boost/move/iterator.hpp \
    3rd_party/boost/move/move.hpp \
    3rd_party/boost/move/traits.hpp \
    3rd_party/boost/move/utility.hpp \
    3rd_party/boost/move/utility_core.hpp \
    3rd_party/boost/preprocessor/config/config.hpp \
    3rd_party/boost/preprocessor/facilities/expand.hpp \
    3rd_party/boost/static_assert.hpp \
    3rd_party/boost/throw_exception.hpp \
    3rd_party/boost/type_index.hpp \
    3rd_party/boost/type_index/ctti_type_index.hpp \
    3rd_party/boost/type_index/detail/compile_time_type_info.hpp \
    3rd_party/boost/type_index/detail/ctti_register_class.hpp \
    3rd_party/boost/type_index/detail/stl_register_class.hpp \
    3rd_party/boost/type_index/stl_type_index.hpp \
    3rd_party/boost/type_index/type_index_facade.hpp \
    3rd_party/boost/type_traits/add_const.hpp \
    3rd_party/boost/type_traits/add_lvalue_reference.hpp \
    3rd_party/boost/type_traits/add_pointer.hpp \
    3rd_party/boost/type_traits/add_reference.hpp \
    3rd_party/boost/type_traits/add_rvalue_reference.hpp \
    3rd_party/boost/type_traits/add_volatile.hpp \
    3rd_party/boost/type_traits/conditional.hpp \
    3rd_party/boost/type_traits/decay.hpp \
    3rd_party/boost/type_traits/declval.hpp \
    3rd_party/boost/type_traits/detail/config.hpp \
    3rd_party/boost/type_traits/detail/is_function_cxx_03.hpp \
    3rd_party/boost/type_traits/detail/is_function_cxx_11.hpp \
    3rd_party/boost/type_traits/detail/is_function_msvc10_fix.hpp \
    3rd_party/boost/type_traits/detail/is_function_ptr_helper.hpp \
    3rd_party/boost/type_traits/detail/is_function_ptr_tester.hpp \
    3rd_party/boost/type_traits/detail/is_mem_fun_pointer_impl.hpp \
    3rd_party/boost/type_traits/detail/is_mem_fun_pointer_tester.hpp \
    3rd_party/boost/type_traits/detail/is_member_function_pointer_cxx_03.hpp \
    3rd_party/boost/type_traits/detail/is_member_function_pointer_cxx_11.hpp \
    3rd_party/boost/type_traits/detail/is_rvalue_reference_msvc10_fix.hpp \
    3rd_party/boost/type_traits/detail/yes_no_type.hpp \
    3rd_party/boost/type_traits/integral_constant.hpp \
    3rd_party/boost/type_traits/intrinsics.hpp \
    3rd_party/boost/type_traits/is_abstract.hpp \
    3rd_party/boost/type_traits/is_arithmetic.hpp \
    3rd_party/boost/type_traits/is_array.hpp \
    3rd_party/boost/type_traits/is_class.hpp \
    3rd_party/boost/type_traits/is_complete.hpp \
    3rd_party/boost/type_traits/is_const.hpp \
    3rd_party/boost/type_traits/is_convertible.hpp \
    3rd_party/boost/type_traits/is_enum.hpp \
    3rd_party/boost/type_traits/is_floating_point.hpp \
    3rd_party/boost/type_traits/is_function.hpp \
    3rd_party/boost/type_traits/is_integral.hpp \
    3rd_party/boost/type_traits/is_lvalue_reference.hpp \
    3rd_party/boost/type_traits/is_member_function_pointer.hpp \
    3rd_party/boost/type_traits/is_member_pointer.hpp \
    3rd_party/boost/type_traits/is_pointer.hpp \
    3rd_party/boost/type_traits/is_polymorphic.hpp \
    3rd_party/boost/type_traits/is_reference.hpp \
    3rd_party/boost/type_traits/is_rvalue_reference.hpp \
    3rd_party/boost/type_traits/is_same.hpp \
    3rd_party/boost/type_traits/is_scalar.hpp \
    3rd_party/boost/type_traits/is_signed.hpp \
    3rd_party/boost/type_traits/is_union.hpp \
    3rd_party/boost/type_traits/is_unsigned.hpp \
    3rd_party/boost/type_traits/is_void.hpp \
    3rd_party/boost/type_traits/is_volatile.hpp \
    3rd_party/boost/type_traits/make_signed.hpp \
    3rd_party/boost/type_traits/remove_bounds.hpp \
    3rd_party/boost/type_traits/remove_cv.hpp \
    3rd_party/boost/type_traits/remove_extent.hpp \
    3rd_party/boost/type_traits/remove_reference.hpp \
    3rd_party/boost/type_traits/type_identity.hpp \
    3rd_party/boost/utility/enable_if.hpp \
    3rd_party/boost/version.hpp \
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
    editor/colors/colors.h \
    editor/constants_editor.h \
    editor/debug.h \
    editor/docks/docks.h \
    editor/enums_editor.h \
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
    editor/globals_editor.h \
    editor/helper_library.h \
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
    engine/enums_engine.h \
    engine/form_engine.h \
    engine/globals_engine.h \
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
    library/dr_containers.h \
    library/dr_debug.h \
    library/dr_math.h \
    library/dr_random.h \
    library/dr_string.h \
    library/dr_time.h \
    library/types/dr_point.h \
    library/types/dr_pointf.h \
    library/types/dr_variant.h \
    model/constants_keys.h \
    model/enums_model_properties.h \
    model/enums_model_types.h \
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
    model/settings/settings.h \
    model/settings/settings_component.h \
    model/settings/settings_component_property.h


