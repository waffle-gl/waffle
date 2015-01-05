# ----------------------------------------------------------------------------
# Target: wflinfo (executable)
# ----------------------------------------------------------------------------

if(waffle_has_nacl)
    return()
endif()

add_executable(wflinfo wflinfo.c)
target_link_libraries(wflinfo ${waffle_libname} ${GETOPT_LIBRARIES})

if(waffle_on_mac)
    set_target_properties(wflinfo
        PROPERTIES
        COMPILE_FLAGS "-ObjC"
        )
endif()

install(
    TARGETS wflinfo
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT utils
    )
