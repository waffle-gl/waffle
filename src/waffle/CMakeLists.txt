# ----------------------------------------------------------------------------
# Applies to all targets in this CMakeLists.
# ----------------------------------------------------------------------------

include_directories(
    android
    api
    cgl
    core
    egl
    glx
    linux
    wayland
    x11
    xegl

    ${egl_INCLUDE_DIRS}
    ${gbm_INCLUDE_DIRS}
    ${gl_INCLUDE_DIRS}
    ${udev_INCLUDE_DIRS}
    ${wayland-client_INCLUDE_DIRS}
    ${wayland-egl_INCLUDE_DIRS}
    ${x11-xcb_INCLUDE_DIRS}
    )

# ----------------------------------------------------------------------------
# Target: waffle (shared library)
# ----------------------------------------------------------------------------

set(waffle_libdeps
    ${egl_LDFLAGS}
    ${gbm_LDFLAGS}
    ${gl_LDFLAGS}
    ${udev_LDFLAGS}
    ${wayland-client_LDFLAGS}
    ${wayland-egl_LDFLAGS}
    ${x11_LDFLAGS}
    ${x11-xcb_LDFLAGS}
    ${xcb_LDFLAGS}
    )

set(waffle_sources
    api/api_priv.c
    api/waffle_attrib_list.c
    api/waffle_config.c
    api/waffle_context.c
    api/waffle_display.c
    api/waffle_dl.c
    api/waffle_enum.c
    api/waffle_error.c
    api/waffle_gl_misc.c
    api/waffle_init.c
    api/waffle_window.c
    core/wcore_attrib_list.c
    core/wcore_config_attrs.c
    core/wcore_display.c
    core/wcore_error.c
    core/wcore_tinfo.c
    core/wcore_util.c
    )

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    list(APPEND waffle_sources
        cgl/cgl_config.m
        cgl/cgl_context.m
        cgl/cgl_display.m
        cgl/cgl_dl.m
        cgl/cgl_error.m
        cgl/cgl_platform.m
        cgl/cgl_window.m

        cgl/WaffleGLView.m
        )
    list(APPEND waffle_libdeps
        ${OPENGL_gl_LIBRARY}
        ${COCOA_FRAMEWORK}
        ${CORE_FOUNDATION_FRAMEWORK}
        )
endif()

if(waffle_has_egl)
    list(APPEND waffle_sources
        egl/wegl_config.c
        egl/wegl_context.c
        egl/wegl_display.c
        egl/wegl_util.c
        egl/wegl_window.c
        )
endif()

if(waffle_has_glx)
    list(APPEND waffle_sources
        glx/glx_config.c
        glx/glx_context.c
        glx/glx_display.c
        glx/glx_platform.c
        glx/glx_window.c
        )
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    list(APPEND waffle_sources
        linux/linux_dl.c
        linux/linux_platform.c
        )
    list(APPEND waffle_libdeps
        dl
        pthread
        )
endif()

if(waffle_has_wayland)
    list(APPEND waffle_sources
        wayland/wayland_display.c
        wayland/wayland_platform.c
        wayland/wayland_window.c
    )
endif()

if(waffle_has_x11)
    list(APPEND waffle_sources
        x11/x11_display.c
        x11/x11_window.c
        )
endif()

if(waffle_has_x11_egl)
    list(APPEND waffle_sources
        xegl/xegl_display.c
        xegl/xegl_platform.c
        xegl/xegl_window.c
    )
endif()

if(waffle_has_gbm)
    list(APPEND waffle_sources
        gbm/wgbm_config.c
        gbm/wgbm_display.c
        gbm/wgbm_platform.c
        gbm/wgbm_window.c
    )
endif()

# CMake will pass to the C compiler only C sources. CMake does not recognize the
# .m extension and ignores any such files in the source lists. To coerce CMake
# to pass .m files to the compiler, we must lie and claim that they are
# C sources.
set_source_files_properties(
    ${waffle_sources}
    PROPERTIES
    LANGUAGE C
    COMPILE_FLAGS "-fvisibility=hidden"
    )

include_directories(
    ${GLX_INLCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${X11-XCB_INCLUDE_DIRS}
    ${XCB_INCLUDE_DIRS}
    )

add_library(${waffle_libname} SHARED ${waffle_sources})
target_link_libraries(${waffle_libname} ${waffle_libdeps})

set_target_properties(${waffle_libname}
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    SOVERSION ${waffle_soversion}
    VERSION ${waffle_soversion}.${waffle_minor_version}.${waffle_patch_version})

install(TARGETS ${waffle_libname}
        LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

# ----------------------------------------------------------------------------
# Target: waffle_static
# ----------------------------------------------------------------------------

# The unittest executable needs access to waffle's internal symbols. Yet, the
# sources in libwaffle.so are compiled with -fvisibility=hidden.
# One solution is to additionally build a static libwaffle and let the
# unittest executable link to that. Visibility options do not apply to static
# libraries; that is, all symbols are public.

add_library(waffle_static
    STATIC
    EXCLUDE_FROM_ALL
    ${waffle_sources}
    )

target_link_libraries(waffle_static)

set_target_properties(waffle_static
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
