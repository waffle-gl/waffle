if(waffle_on_linux)
    if(gl_FOUND AND x11-xcb_FOUND)
        set(glx_default ON)
    else()
        set(glx_default OFF)
    endif()

    if(wayland-client_FOUND AND wayland-egl_FOUND AND egl_FOUND)
        set(wayland_default ON)
    else()
        set(wayland_default OFF)
    endif()

    if(x11-xcb_FOUND AND egl_FOUND)
        set(x11_egl_default ON)
    else()
        set(x11_egl_default OFF)
    endif()

    if(gbm_FOUND AND libudev_FOUND AND egl_FOUND)
        set(gbm_default ON)
    else()
        set(gbm_default OFF)
    endif()

    # On Linux, you must enable at least one of the below options.
    option(waffle_has_glx "Build support for GLX" ${glx_default})
    option(waffle_has_wayland "Build support for Wayland" ${wayland_default})
    option(waffle_has_x11_egl "Build support for X11/EGL" ${x11_egl_default})
    option(waffle_has_gbm "Build support for GBM" ${gbm_default})
endif()

option(waffle_build_tests "Build tests" ON)
option(waffle_build_manpages "Build manpages" OFF)
option(waffle_build_htmldocs "Build html documentation" OFF)
option(waffle_build_examples "Build example programs" ON)

set(waffle_xsltproc "xsltproc"
    CACHE STRING "Program for processing XSLT stylesheets. Used for building docs.")
