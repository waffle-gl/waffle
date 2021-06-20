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

    if(gbm_FOUND AND libdrm_FOUND AND egl_FOUND)
        set(gbm_default ON)
    else()
        set(gbm_default OFF)
    endif()

    set(surfaceless_egl_default ${egl_FOUND})

    # On Linux, you must enable at least one of the below options.
    option(waffle_has_glx "Build support for GLX" ${glx_default})
    option(waffle_has_wayland "Build support for Wayland" ${wayland_default})
    option(waffle_has_x11_egl "Build support for X11/EGL" ${x11_egl_default})
    option(waffle_has_gbm "Build support for GBM" ${gbm_default})
    option(waffle_has_surfaceless_egl "Build support for EGL_MESA_platform_surfaceless" ${surfaceless_egl_default})
    option(waffle_has_nacl "Build support for NaCl" OFF)

    # NaCl specific settings.
    set(nacl_sdk_path "" CACHE STRING "Set nacl_sdk path here")
    set(nacl_version "pepper_39" CACHE STRING "Set NaCl bundle here")
endif()

option(waffle_build_tests "Build tests" ON)
option(waffle_build_manpages "Build manpages" OFF)
option(waffle_build_htmldocs "Build html documentation" OFF)
option(waffle_build_examples "Build example programs" ON)

set(waffle_xsltproc "xsltproc"
    CACHE STRING "Program for processing XSLT stylesheets. Used for building docs.")
