install(
    FILES
        Makefile.example
        gl_basic.c
        simple-x11-egl.c
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/examples"
    COMPONENT examples
    )

# ----------------------------------------------------------------------------
# Target: simple-x11-egl (executable)
# ----------------------------------------------------------------------------

if(waffle_on_linux AND waffle_has_x11_egl)
    add_executable(simple-x11-egl simple-x11-egl.c)
    target_link_libraries(simple-x11-egl ${waffle_libname})
endif()

# ----------------------------------------------------------------------------
# Target: gl_basic (executable)
# ----------------------------------------------------------------------------

if(waffle_has_nacl)
    return()
endif()

add_executable(gl_basic gl_basic.c)
target_link_libraries(gl_basic ${waffle_libname} ${GETOPT_LIBRARIES})

if(waffle_on_mac)
    set_target_properties(gl_basic
        PROPERTIES
        COMPILE_FLAGS "-ObjC"
        )
endif()
