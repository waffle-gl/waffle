# ----------------------------------------------------------------------------
# Target: waffle (shared library)
# ----------------------------------------------------------------------------

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
        egl/egl_no_native.c
        )
    list(APPEND waffle_libdeps
        ${waffle_EGL_library}
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
    list(APPEND waffle_libdeps
        ${waffle_GL_library}
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
        wayland/wayland_config.c
        wayland/wayland_context.c
        wayland/wayland_display.c
        wayland/wayland_platform.c
        wayland/wayland_priv_egl.c
        wayland/wayland_window.c
    )
    list(APPEND waffle_libdeps
        ${waffle_wayland-client_library}
        ${waffle_wayland-egl_library}
        )
endif()

if(waffle_has_x11)
    list(APPEND waffle_sources
        x11/x11_display.c
        x11/x11_window.c
        )
    list(APPEND waffle_libdeps
        ${waffle_X11-xcb_library}
        ${waffle_X11_library}
        ${waffle_xcb_library}
        )
endif()

if(waffle_has_x11_egl)
    list(APPEND waffle_sources
        x11_egl/xegl_config.c
        x11_egl/xegl_context.c
        x11_egl/xegl_display.c
        x11_egl/xegl_platform.c
        x11_egl/xegl_priv_egl.c
        x11_egl/xegl_window.c
    )
endif()

# CMake will pass to the C compiler only C sources. CMake does not recognize the
# .m extension and ignores any such files in the source lists. To coerce CMake
# to pass .m files to the compiler, we must lie and claim that they are
# C sources.
set_source_files_properties(
    ${waffle_sources}
    PROPERTIES LANGUAGE C
    )

add_library(waffle SHARED ${waffle_sources})
target_link_libraries(waffle ${waffle_libdeps})

set_target_properties(waffle
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    SOVERSION ${waffle_so_version})

install(TARGETS waffle
    LIBRARY DESTINATION ${waffle_install_libdir})
