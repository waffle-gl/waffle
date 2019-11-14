configure_file(waffle-1/waffle_version.h.in
               ${CMAKE_CURRENT_SOURCE_DIR}/waffle-1/waffle_version.h
               @ONLY)

install(
    FILES
        waffle-1/waffle.h
        waffle-1/waffle_gbm.h
        waffle-1/waffle_glx.h
        waffle-1/waffle_version.h
        waffle-1/waffle_wayland.h
        waffle-1/waffle_x11_egl.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${waffle_libname}
    COMPONENT headers
    )
