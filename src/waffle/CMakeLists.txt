# ----------------------------------------------------------------------------
# Target: waffle (shared library)
# ----------------------------------------------------------------------------

set(waffle_sources
    api/api_priv.c
    api/waffle_attrib_list.c
    api/waffle_display.c
    api/waffle_init.c
    core/wcore_error.c
    core/wcore_platform.c
    )

if(waffle_has_egl)
    list(APPEND waffle_sources
        egl/egl_choose_config.c
        egl/egl_no_native.c
        )
endif(waffle_has_egl)

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

if(waffle_has_x11_egl)
  target_link_libraries(waffle ${waffle_X11-xcb_library})
endif(waffle_has_x11_egl)

# The version scheme for libwaffle.so follows the libtool guidelines
# (http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html).
set_target_properties(waffle
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    SOVERSION 0.0.0)
