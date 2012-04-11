# ----------------------------------------------------------------------------
# Target: waffle (shared library)
# ----------------------------------------------------------------------------

set(waffle_sources
    api/api_priv.c
    api/waffle_attrib_list.c
    api/waffle_config.c
    api/waffle_context.c
    api/waffle_display.c
    api/waffle_enum.c
    api/waffle_error.c
    api/waffle_gl_misc.c
    api/waffle_init.c
    api/waffle_window.c
    core/wcore_config_attrs.c
    core/wcore_error.c
    core/wcore_platform.c
    )

if(waffle_has_egl)
    list(APPEND waffle_sources
        egl/egl_no_native.c
        )
endif(waffle_has_egl)

if(waffle_has_glx)
    list(APPEND waffle_sources
        glx/glx_config.c
        glx/glx_context.c
        glx/glx_display.c
        glx/glx_gl_misc.c
        glx/glx_platform.c
        glx/glx_window.c
        )
endif(waffle_has_glx)

if(waffle_has_wayland)
    list(APPEND waffle_sources
        wayland/wayland_config.c
        wayland/wayland_context.c
        wayland/wayland_display.c
        wayland/wayland_gl_misc.c
        wayland/wayland_platform.c
        wayland/wayland_priv_egl.c
        wayland/wayland_window.c
    )
endif(waffle_has_wayland)

if(waffle_has_x11)
    list(APPEND waffle_sources
        x11/x11.c
        )
endif(waffle_has_x11)

if(waffle_has_x11_egl)
    list(APPEND waffle_sources
        x11_egl/xegl_config.c
        x11_egl/xegl_context.c
        x11_egl/xegl_display.c
        x11_egl/xegl_gl_misc.c
        x11_egl/xegl_platform.c
        x11_egl/xegl_priv_egl.c
        x11_egl/xegl_window.c
    )
endif(waffle_has_x11_egl)

add_library(waffle SHARED ${waffle_sources})

if(waffle_has_egl)
  target_link_libraries(waffle ${waffle_EGL_library})
endif(waffle_has_egl)

if(waffle_has_glx)
    target_link_libraries(waffle ${waffle_GL_library})
endif(waffle_has_glx)

if(waffle_has_wayland)
    target_link_libraries(waffle
        ${waffle_wayland-client_library}
        ${waffle_wayland-egl_library}
        )
endif(waffle_has_wayland)

if(waffle_has_x11_egl)
  target_link_libraries(waffle ${waffle_X11-xcb_library})
endif(waffle_has_x11_egl)

set_target_properties(waffle
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    SOVERSION ${waffle_so_version})

install(TARGETS waffle
    LIBRARY DESTINATION ${waffle_install_libdir})
