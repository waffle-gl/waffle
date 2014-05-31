# ----------------------------------------------------------------------------
# Target: wflinfo (executable)
# ----------------------------------------------------------------------------

add_executable(wflinfo wflinfo.c)
target_link_libraries(wflinfo ${waffle_libname})
set_target_properties(wflinfo
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )

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
