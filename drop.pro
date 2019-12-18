####################################################################################
##
##                   DROP
##           by Scidian Software
##
############################
TARGET   =  Drop
TEMPLATE =  app

CONFIG  +=  c++11

##  C Compiler Flags
QMAKE_CFLAGS    += -std=c99

##  C++ Compiler Flags
QMAKE_CXXFLAGS  += -Wno-sign-conversion



##
## !!!!! Specify Editor or Engine Only !!!!!
##
CONFIG  += drop_editor

CONFIG(drop_editor){
    QT  +=  core gui
    QT  += widgets

    # Show warnings if using any feature of Qt which has been marked as deprecated
    DEFINES         +=  QT_DEPRECATED_WARNINGS

    # Fail to compile if using deprecated APIs.
    # Also possible to disable deprecated APIs only up to a certain version of Qt.
    #DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
}


# Default rules for deployment.
qnx:  target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


##### Resource Files
RESOURCES += \
    images.qrc \
    shaders.qrc


##### Other Included Files
DISTFILES += \
    drop_icon.ico \
    drop_icons.icns


##### Icon Files for Project
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
    3rd_party/glm/detail/glm.cpp \
    3rd_party/hull_finder.cpp \
    3rd_party/poly_partition.cpp \
    3rd_party/polyline_simplification.cpp \
    editor/colors/colors.cpp \
    editor/colors/palette_blank.cpp \
    editor/colors/palette_material.cpp \
    editor/colors/palette_rocky_rover.cpp \
    editor/colors/palette_window_themes.cpp \
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
    editor/project/project_add_built_in_images.cpp \
    editor/project/project_initialize_new.cpp \
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
    library/types/dr_color.cpp \
    library/types/dr_point.cpp \
    library/types/dr_pointf.cpp \
    library/types/dr_rect.cpp \
    library/types/dr_rectf.cpp \
    library/types/dr_variant.cpp \
    library/types/dr_vec2.cpp \
    library/types/dr_vec3.cpp \
    main.cpp \
    model/enums_model_types.cpp \
    model/project/project.cpp \
    model/project/project__file_open.cpp \
    model/project/project__file_save.cpp \
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
    3rd_party/glm/common.hpp \
    3rd_party/glm/detail/_features.hpp \
    3rd_party/glm/detail/_fixes.hpp \
    3rd_party/glm/detail/_noise.hpp \
    3rd_party/glm/detail/_swizzle.hpp \
    3rd_party/glm/detail/_swizzle_func.hpp \
    3rd_party/glm/detail/_vectorize.hpp \
    3rd_party/glm/detail/compute_common.hpp \
    3rd_party/glm/detail/compute_vector_relational.hpp \
    3rd_party/glm/detail/func_common.inl \
    3rd_party/glm/detail/func_common_simd.inl \
    3rd_party/glm/detail/func_exponential.inl \
    3rd_party/glm/detail/func_exponential_simd.inl \
    3rd_party/glm/detail/func_geometric.inl \
    3rd_party/glm/detail/func_geometric_simd.inl \
    3rd_party/glm/detail/func_integer.inl \
    3rd_party/glm/detail/func_integer_simd.inl \
    3rd_party/glm/detail/func_matrix.inl \
    3rd_party/glm/detail/func_matrix_simd.inl \
    3rd_party/glm/detail/func_packing.inl \
    3rd_party/glm/detail/func_packing_simd.inl \
    3rd_party/glm/detail/func_trigonometric.inl \
    3rd_party/glm/detail/func_trigonometric_simd.inl \
    3rd_party/glm/detail/func_vector_relational.inl \
    3rd_party/glm/detail/func_vector_relational_simd.inl \
    3rd_party/glm/detail/qualifier.hpp \
    3rd_party/glm/detail/setup.hpp \
    3rd_party/glm/detail/type_float.hpp \
    3rd_party/glm/detail/type_half.hpp \
    3rd_party/glm/detail/type_half.inl \
    3rd_party/glm/detail/type_mat2x2.hpp \
    3rd_party/glm/detail/type_mat2x2.inl \
    3rd_party/glm/detail/type_mat2x3.hpp \
    3rd_party/glm/detail/type_mat2x3.inl \
    3rd_party/glm/detail/type_mat2x4.hpp \
    3rd_party/glm/detail/type_mat2x4.inl \
    3rd_party/glm/detail/type_mat3x2.hpp \
    3rd_party/glm/detail/type_mat3x2.inl \
    3rd_party/glm/detail/type_mat3x3.hpp \
    3rd_party/glm/detail/type_mat3x3.inl \
    3rd_party/glm/detail/type_mat3x4.hpp \
    3rd_party/glm/detail/type_mat3x4.inl \
    3rd_party/glm/detail/type_mat4x2.hpp \
    3rd_party/glm/detail/type_mat4x2.inl \
    3rd_party/glm/detail/type_mat4x3.hpp \
    3rd_party/glm/detail/type_mat4x3.inl \
    3rd_party/glm/detail/type_mat4x4.hpp \
    3rd_party/glm/detail/type_mat4x4.inl \
    3rd_party/glm/detail/type_mat4x4_simd.inl \
    3rd_party/glm/detail/type_quat.hpp \
    3rd_party/glm/detail/type_quat.inl \
    3rd_party/glm/detail/type_quat_simd.inl \
    3rd_party/glm/detail/type_vec1.hpp \
    3rd_party/glm/detail/type_vec1.inl \
    3rd_party/glm/detail/type_vec2.hpp \
    3rd_party/glm/detail/type_vec2.inl \
    3rd_party/glm/detail/type_vec3.hpp \
    3rd_party/glm/detail/type_vec3.inl \
    3rd_party/glm/detail/type_vec4.hpp \
    3rd_party/glm/detail/type_vec4.inl \
    3rd_party/glm/detail/type_vec4_simd.inl \
    3rd_party/glm/exponential.hpp \
    3rd_party/glm/ext.hpp \
    3rd_party/glm/ext/matrix_clip_space.hpp \
    3rd_party/glm/ext/matrix_clip_space.inl \
    3rd_party/glm/ext/matrix_common.hpp \
    3rd_party/glm/ext/matrix_common.inl \
    3rd_party/glm/ext/matrix_double2x2.hpp \
    3rd_party/glm/ext/matrix_double2x2_precision.hpp \
    3rd_party/glm/ext/matrix_double2x3.hpp \
    3rd_party/glm/ext/matrix_double2x3_precision.hpp \
    3rd_party/glm/ext/matrix_double2x4.hpp \
    3rd_party/glm/ext/matrix_double2x4_precision.hpp \
    3rd_party/glm/ext/matrix_double3x2.hpp \
    3rd_party/glm/ext/matrix_double3x2_precision.hpp \
    3rd_party/glm/ext/matrix_double3x3.hpp \
    3rd_party/glm/ext/matrix_double3x3_precision.hpp \
    3rd_party/glm/ext/matrix_double3x4.hpp \
    3rd_party/glm/ext/matrix_double3x4_precision.hpp \
    3rd_party/glm/ext/matrix_double4x2.hpp \
    3rd_party/glm/ext/matrix_double4x2_precision.hpp \
    3rd_party/glm/ext/matrix_double4x3.hpp \
    3rd_party/glm/ext/matrix_double4x3_precision.hpp \
    3rd_party/glm/ext/matrix_double4x4.hpp \
    3rd_party/glm/ext/matrix_double4x4_precision.hpp \
    3rd_party/glm/ext/matrix_float2x2.hpp \
    3rd_party/glm/ext/matrix_float2x2_precision.hpp \
    3rd_party/glm/ext/matrix_float2x3.hpp \
    3rd_party/glm/ext/matrix_float2x3_precision.hpp \
    3rd_party/glm/ext/matrix_float2x4.hpp \
    3rd_party/glm/ext/matrix_float2x4_precision.hpp \
    3rd_party/glm/ext/matrix_float3x2.hpp \
    3rd_party/glm/ext/matrix_float3x2_precision.hpp \
    3rd_party/glm/ext/matrix_float3x3.hpp \
    3rd_party/glm/ext/matrix_float3x3_precision.hpp \
    3rd_party/glm/ext/matrix_float3x4.hpp \
    3rd_party/glm/ext/matrix_float3x4_precision.hpp \
    3rd_party/glm/ext/matrix_float4x2.hpp \
    3rd_party/glm/ext/matrix_float4x2_precision.hpp \
    3rd_party/glm/ext/matrix_float4x3.hpp \
    3rd_party/glm/ext/matrix_float4x3_precision.hpp \
    3rd_party/glm/ext/matrix_float4x4.hpp \
    3rd_party/glm/ext/matrix_float4x4_precision.hpp \
    3rd_party/glm/ext/matrix_projection.hpp \
    3rd_party/glm/ext/matrix_projection.inl \
    3rd_party/glm/ext/matrix_relational.hpp \
    3rd_party/glm/ext/matrix_relational.inl \
    3rd_party/glm/ext/matrix_transform.hpp \
    3rd_party/glm/ext/matrix_transform.inl \
    3rd_party/glm/ext/quaternion_common.hpp \
    3rd_party/glm/ext/quaternion_common.inl \
    3rd_party/glm/ext/quaternion_common_simd.inl \
    3rd_party/glm/ext/quaternion_double.hpp \
    3rd_party/glm/ext/quaternion_double_precision.hpp \
    3rd_party/glm/ext/quaternion_exponential.hpp \
    3rd_party/glm/ext/quaternion_exponential.inl \
    3rd_party/glm/ext/quaternion_float.hpp \
    3rd_party/glm/ext/quaternion_float_precision.hpp \
    3rd_party/glm/ext/quaternion_geometric.hpp \
    3rd_party/glm/ext/quaternion_geometric.inl \
    3rd_party/glm/ext/quaternion_relational.hpp \
    3rd_party/glm/ext/quaternion_relational.inl \
    3rd_party/glm/ext/quaternion_transform.hpp \
    3rd_party/glm/ext/quaternion_transform.inl \
    3rd_party/glm/ext/quaternion_trigonometric.hpp \
    3rd_party/glm/ext/quaternion_trigonometric.inl \
    3rd_party/glm/ext/scalar_common.hpp \
    3rd_party/glm/ext/scalar_common.inl \
    3rd_party/glm/ext/scalar_constants.hpp \
    3rd_party/glm/ext/scalar_constants.inl \
    3rd_party/glm/ext/scalar_int_sized.hpp \
    3rd_party/glm/ext/scalar_integer.hpp \
    3rd_party/glm/ext/scalar_integer.inl \
    3rd_party/glm/ext/scalar_relational.hpp \
    3rd_party/glm/ext/scalar_relational.inl \
    3rd_party/glm/ext/scalar_uint_sized.hpp \
    3rd_party/glm/ext/scalar_ulp.hpp \
    3rd_party/glm/ext/scalar_ulp.inl \
    3rd_party/glm/ext/vector_bool1.hpp \
    3rd_party/glm/ext/vector_bool1_precision.hpp \
    3rd_party/glm/ext/vector_bool2.hpp \
    3rd_party/glm/ext/vector_bool2_precision.hpp \
    3rd_party/glm/ext/vector_bool3.hpp \
    3rd_party/glm/ext/vector_bool3_precision.hpp \
    3rd_party/glm/ext/vector_bool4.hpp \
    3rd_party/glm/ext/vector_bool4_precision.hpp \
    3rd_party/glm/ext/vector_common.hpp \
    3rd_party/glm/ext/vector_common.inl \
    3rd_party/glm/ext/vector_double1.hpp \
    3rd_party/glm/ext/vector_double1_precision.hpp \
    3rd_party/glm/ext/vector_double2.hpp \
    3rd_party/glm/ext/vector_double2_precision.hpp \
    3rd_party/glm/ext/vector_double3.hpp \
    3rd_party/glm/ext/vector_double3_precision.hpp \
    3rd_party/glm/ext/vector_double4.hpp \
    3rd_party/glm/ext/vector_double4_precision.hpp \
    3rd_party/glm/ext/vector_float1.hpp \
    3rd_party/glm/ext/vector_float1_precision.hpp \
    3rd_party/glm/ext/vector_float2.hpp \
    3rd_party/glm/ext/vector_float2_precision.hpp \
    3rd_party/glm/ext/vector_float3.hpp \
    3rd_party/glm/ext/vector_float3_precision.hpp \
    3rd_party/glm/ext/vector_float4.hpp \
    3rd_party/glm/ext/vector_float4_precision.hpp \
    3rd_party/glm/ext/vector_int1.hpp \
    3rd_party/glm/ext/vector_int1_precision.hpp \
    3rd_party/glm/ext/vector_int2.hpp \
    3rd_party/glm/ext/vector_int2_precision.hpp \
    3rd_party/glm/ext/vector_int3.hpp \
    3rd_party/glm/ext/vector_int3_precision.hpp \
    3rd_party/glm/ext/vector_int4.hpp \
    3rd_party/glm/ext/vector_int4_precision.hpp \
    3rd_party/glm/ext/vector_integer.hpp \
    3rd_party/glm/ext/vector_integer.inl \
    3rd_party/glm/ext/vector_relational.hpp \
    3rd_party/glm/ext/vector_relational.inl \
    3rd_party/glm/ext/vector_uint1.hpp \
    3rd_party/glm/ext/vector_uint1_precision.hpp \
    3rd_party/glm/ext/vector_uint2.hpp \
    3rd_party/glm/ext/vector_uint2_precision.hpp \
    3rd_party/glm/ext/vector_uint3.hpp \
    3rd_party/glm/ext/vector_uint3_precision.hpp \
    3rd_party/glm/ext/vector_uint4.hpp \
    3rd_party/glm/ext/vector_uint4_precision.hpp \
    3rd_party/glm/ext/vector_ulp.hpp \
    3rd_party/glm/ext/vector_ulp.inl \
    3rd_party/glm/fwd.hpp \
    3rd_party/glm/geometric.hpp \
    3rd_party/glm/glm.hpp \
    3rd_party/glm/gtc/bitfield.hpp \
    3rd_party/glm/gtc/bitfield.inl \
    3rd_party/glm/gtc/color_space.hpp \
    3rd_party/glm/gtc/color_space.inl \
    3rd_party/glm/gtc/constants.hpp \
    3rd_party/glm/gtc/constants.inl \
    3rd_party/glm/gtc/epsilon.hpp \
    3rd_party/glm/gtc/epsilon.inl \
    3rd_party/glm/gtc/integer.hpp \
    3rd_party/glm/gtc/integer.inl \
    3rd_party/glm/gtc/matrix_access.hpp \
    3rd_party/glm/gtc/matrix_access.inl \
    3rd_party/glm/gtc/matrix_integer.hpp \
    3rd_party/glm/gtc/matrix_inverse.hpp \
    3rd_party/glm/gtc/matrix_inverse.inl \
    3rd_party/glm/gtc/matrix_transform.hpp \
    3rd_party/glm/gtc/matrix_transform.inl \
    3rd_party/glm/gtc/noise.hpp \
    3rd_party/glm/gtc/noise.inl \
    3rd_party/glm/gtc/packing.hpp \
    3rd_party/glm/gtc/packing.inl \
    3rd_party/glm/gtc/quaternion.hpp \
    3rd_party/glm/gtc/quaternion.inl \
    3rd_party/glm/gtc/quaternion_simd.inl \
    3rd_party/glm/gtc/random.hpp \
    3rd_party/glm/gtc/random.inl \
    3rd_party/glm/gtc/reciprocal.hpp \
    3rd_party/glm/gtc/reciprocal.inl \
    3rd_party/glm/gtc/round.hpp \
    3rd_party/glm/gtc/round.inl \
    3rd_party/glm/gtc/type_aligned.hpp \
    3rd_party/glm/gtc/type_precision.hpp \
    3rd_party/glm/gtc/type_precision.inl \
    3rd_party/glm/gtc/type_ptr.hpp \
    3rd_party/glm/gtc/type_ptr.inl \
    3rd_party/glm/gtc/ulp.hpp \
    3rd_party/glm/gtc/ulp.inl \
    3rd_party/glm/gtc/vec1.hpp \
    3rd_party/glm/gtx/associated_min_max.hpp \
    3rd_party/glm/gtx/associated_min_max.inl \
    3rd_party/glm/gtx/bit.hpp \
    3rd_party/glm/gtx/bit.inl \
    3rd_party/glm/gtx/closest_point.hpp \
    3rd_party/glm/gtx/closest_point.inl \
    3rd_party/glm/gtx/color_encoding.hpp \
    3rd_party/glm/gtx/color_encoding.inl \
    3rd_party/glm/gtx/color_space.hpp \
    3rd_party/glm/gtx/color_space.inl \
    3rd_party/glm/gtx/color_space_YCoCg.hpp \
    3rd_party/glm/gtx/color_space_YCoCg.inl \
    3rd_party/glm/gtx/common.hpp \
    3rd_party/glm/gtx/common.inl \
    3rd_party/glm/gtx/compatibility.hpp \
    3rd_party/glm/gtx/compatibility.inl \
    3rd_party/glm/gtx/component_wise.hpp \
    3rd_party/glm/gtx/component_wise.inl \
    3rd_party/glm/gtx/dual_quaternion.hpp \
    3rd_party/glm/gtx/dual_quaternion.inl \
    3rd_party/glm/gtx/easing.hpp \
    3rd_party/glm/gtx/easing.inl \
    3rd_party/glm/gtx/euler_angles.hpp \
    3rd_party/glm/gtx/euler_angles.inl \
    3rd_party/glm/gtx/extend.hpp \
    3rd_party/glm/gtx/extend.inl \
    3rd_party/glm/gtx/extended_min_max.hpp \
    3rd_party/glm/gtx/extended_min_max.inl \
    3rd_party/glm/gtx/exterior_product.hpp \
    3rd_party/glm/gtx/exterior_product.inl \
    3rd_party/glm/gtx/fast_exponential.hpp \
    3rd_party/glm/gtx/fast_exponential.inl \
    3rd_party/glm/gtx/fast_square_root.hpp \
    3rd_party/glm/gtx/fast_square_root.inl \
    3rd_party/glm/gtx/fast_trigonometry.hpp \
    3rd_party/glm/gtx/fast_trigonometry.inl \
    3rd_party/glm/gtx/float_notmalize.inl \
    3rd_party/glm/gtx/functions.hpp \
    3rd_party/glm/gtx/functions.inl \
    3rd_party/glm/gtx/gradient_paint.hpp \
    3rd_party/glm/gtx/gradient_paint.inl \
    3rd_party/glm/gtx/handed_coordinate_space.hpp \
    3rd_party/glm/gtx/handed_coordinate_space.inl \
    3rd_party/glm/gtx/hash.hpp \
    3rd_party/glm/gtx/hash.inl \
    3rd_party/glm/gtx/integer.hpp \
    3rd_party/glm/gtx/integer.inl \
    3rd_party/glm/gtx/intersect.hpp \
    3rd_party/glm/gtx/intersect.inl \
    3rd_party/glm/gtx/io.hpp \
    3rd_party/glm/gtx/io.inl \
    3rd_party/glm/gtx/log_base.hpp \
    3rd_party/glm/gtx/log_base.inl \
    3rd_party/glm/gtx/matrix_cross_product.hpp \
    3rd_party/glm/gtx/matrix_cross_product.inl \
    3rd_party/glm/gtx/matrix_decompose.hpp \
    3rd_party/glm/gtx/matrix_decompose.inl \
    3rd_party/glm/gtx/matrix_factorisation.hpp \
    3rd_party/glm/gtx/matrix_factorisation.inl \
    3rd_party/glm/gtx/matrix_interpolation.hpp \
    3rd_party/glm/gtx/matrix_interpolation.inl \
    3rd_party/glm/gtx/matrix_major_storage.hpp \
    3rd_party/glm/gtx/matrix_major_storage.inl \
    3rd_party/glm/gtx/matrix_operation.hpp \
    3rd_party/glm/gtx/matrix_operation.inl \
    3rd_party/glm/gtx/matrix_query.hpp \
    3rd_party/glm/gtx/matrix_query.inl \
    3rd_party/glm/gtx/matrix_transform_2d.hpp \
    3rd_party/glm/gtx/matrix_transform_2d.inl \
    3rd_party/glm/gtx/mixed_product.hpp \
    3rd_party/glm/gtx/mixed_product.inl \
    3rd_party/glm/gtx/norm.hpp \
    3rd_party/glm/gtx/norm.inl \
    3rd_party/glm/gtx/normal.hpp \
    3rd_party/glm/gtx/normal.inl \
    3rd_party/glm/gtx/normalize_dot.hpp \
    3rd_party/glm/gtx/normalize_dot.inl \
    3rd_party/glm/gtx/number_precision.hpp \
    3rd_party/glm/gtx/number_precision.inl \
    3rd_party/glm/gtx/optimum_pow.hpp \
    3rd_party/glm/gtx/optimum_pow.inl \
    3rd_party/glm/gtx/orthonormalize.hpp \
    3rd_party/glm/gtx/orthonormalize.inl \
    3rd_party/glm/gtx/perpendicular.hpp \
    3rd_party/glm/gtx/perpendicular.inl \
    3rd_party/glm/gtx/polar_coordinates.hpp \
    3rd_party/glm/gtx/polar_coordinates.inl \
    3rd_party/glm/gtx/projection.hpp \
    3rd_party/glm/gtx/projection.inl \
    3rd_party/glm/gtx/quaternion.hpp \
    3rd_party/glm/gtx/quaternion.inl \
    3rd_party/glm/gtx/range.hpp \
    3rd_party/glm/gtx/raw_data.hpp \
    3rd_party/glm/gtx/raw_data.inl \
    3rd_party/glm/gtx/rotate_normalized_axis.hpp \
    3rd_party/glm/gtx/rotate_normalized_axis.inl \
    3rd_party/glm/gtx/rotate_vector.hpp \
    3rd_party/glm/gtx/rotate_vector.inl \
    3rd_party/glm/gtx/scalar_multiplication.hpp \
    3rd_party/glm/gtx/scalar_relational.hpp \
    3rd_party/glm/gtx/scalar_relational.inl \
    3rd_party/glm/gtx/spline.hpp \
    3rd_party/glm/gtx/spline.inl \
    3rd_party/glm/gtx/std_based_type.hpp \
    3rd_party/glm/gtx/std_based_type.inl \
    3rd_party/glm/gtx/string_cast.hpp \
    3rd_party/glm/gtx/string_cast.inl \
    3rd_party/glm/gtx/texture.hpp \
    3rd_party/glm/gtx/texture.inl \
    3rd_party/glm/gtx/transform.hpp \
    3rd_party/glm/gtx/transform.inl \
    3rd_party/glm/gtx/transform2.hpp \
    3rd_party/glm/gtx/transform2.inl \
    3rd_party/glm/gtx/type_aligned.hpp \
    3rd_party/glm/gtx/type_aligned.inl \
    3rd_party/glm/gtx/type_trait.hpp \
    3rd_party/glm/gtx/type_trait.inl \
    3rd_party/glm/gtx/vec_swizzle.hpp \
    3rd_party/glm/gtx/vector_angle.hpp \
    3rd_party/glm/gtx/vector_angle.inl \
    3rd_party/glm/gtx/vector_query.hpp \
    3rd_party/glm/gtx/vector_query.inl \
    3rd_party/glm/gtx/wrap.hpp \
    3rd_party/glm/gtx/wrap.inl \
    3rd_party/glm/integer.hpp \
    3rd_party/glm/mat2x2.hpp \
    3rd_party/glm/mat2x3.hpp \
    3rd_party/glm/mat2x4.hpp \
    3rd_party/glm/mat3x2.hpp \
    3rd_party/glm/mat3x3.hpp \
    3rd_party/glm/mat3x4.hpp \
    3rd_party/glm/mat4x2.hpp \
    3rd_party/glm/mat4x3.hpp \
    3rd_party/glm/mat4x4.hpp \
    3rd_party/glm/matrix.hpp \
    3rd_party/glm/packing.hpp \
    3rd_party/glm/simd/common.h \
    3rd_party/glm/simd/exponential.h \
    3rd_party/glm/simd/geometric.h \
    3rd_party/glm/simd/integer.h \
    3rd_party/glm/simd/matrix.h \
    3rd_party/glm/simd/packing.h \
    3rd_party/glm/simd/platform.h \
    3rd_party/glm/simd/trigonometric.h \
    3rd_party/glm/simd/vector_relational.h \
    3rd_party/glm/trigonometric.hpp \
    3rd_party/glm/vec2.hpp \
    3rd_party/glm/vec3.hpp \
    3rd_party/glm/vec4.hpp \
    3rd_party/glm/vector_relational.hpp \
    3rd_party/hull_finder.h \
    3rd_party/poly_partition.h \
    3rd_party/polyline_simplification.h \
    3rd_party/stb/stb_image.h \
    3rd_party/stb/stb_image_resize.h \
    3rd_party/stb/stb_image_write.h \
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
    editor/project/project.h \
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
    library/types/dr_color.h \
    library/types/dr_point.h \
    library/types/dr_pointf.h \
    library/types/dr_rect.h \
    library/types/dr_rectf.h \
    library/types/dr_variant.h \
    library/types/dr_vec2.h \
    library/types/dr_vec3.h \
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


