configure_file(waffle/waffle_version.h.in
               ${CMAKE_CURRENT_SOURCE_DIR}/waffle/waffle_version.h
               @ONLY)

install(
    FILES
        waffle/waffle.h
        waffle/waffle_gbm.h
        waffle/waffle_glx.h
        waffle/waffle_version.h
        waffle/waffle_wayland.h
        waffle/waffle_x11_egl.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${waffle_libname}
    COMPONENT headers
    )
