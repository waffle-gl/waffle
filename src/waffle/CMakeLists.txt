# ----------------------------------------------------------------------------
# Target: waffle (shared library)
# ----------------------------------------------------------------------------

set(waffle_sources
    api/waffle_attrib_list.c
    core/wcore_error.c
    )

if(waffle_has_x11)
    list(APPEND waffle_sources
        x11/x11.c
        )
endif(waffle_has_x11)

add_library(waffle SHARED ${waffle_sources})

if(waffle_has_x11_egl)
    target_link_libraries(waffle
        ${waffle_EGL_library}
        ${waffle_X11-xcb_library}
        )
endif(waffle_has_x11_egl)

# The version scheme for libwaffle.so follows the libtool guidelines
# (http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html).
set_target_properties(waffle
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    SOVERSION 0.0.0)
