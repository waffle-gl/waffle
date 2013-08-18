if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    # On Linux, you must enable at least one of the below options.
    option(waffle_has_glx "Build support for GLX" OFF)
    option(waffle_has_wayland "Build support for Wayland" OFF)
    option(waffle_has_x11_egl "Build support for X11/EGL" OFF)
    option(waffle_has_gbm "Build support for GBM" OFF)
endif()

option(waffle_build_tests "Build tests" ON)
option(waffle_build_manpages "Build manpages" OFF)
option(waffle_build_htmldoc "Build html documentation" OFF)
option(waffle_build_examples "Build example programs" ON)

set(waffle_xsltproc "xsltproc"
    CACHE STRING "Program for processing XSLT stylesheets. Used for building docs.")
