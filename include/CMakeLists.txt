configure_file(waffle/waffle_version.h.in
               ${CMAKE_CURRENT_SOURCE_DIR}/waffle/waffle_version.h
               @ONLY)

           #install(FILES waffle/waffle_version.h
           #        DESTINATION ${waffle_install_includedir}/${waffle_libname})

install(FILES waffle/waffle.h
              waffle/waffle_attrib_list.h
              waffle/waffle_config.h
              waffle/waffle_context.h
              waffle/waffle_display.h
              waffle/waffle_dl.h
              waffle/waffle_enum.h
              waffle/waffle_error.h
              waffle/waffle_gl_misc.h
              waffle/waffle_gbm.h
              waffle/waffle_glx.h
              waffle/waffle_init.h
              waffle/waffle_portability.h
              waffle/waffle_version.h
              waffle/waffle_wayland.h
              waffle/waffle_window.h
              waffle/waffle_x11_egl.h
        DESTINATION ${waffle_install_includedir}/${waffle_libname})
