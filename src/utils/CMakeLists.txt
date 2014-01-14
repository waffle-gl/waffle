# ----------------------------------------------------------------------------
# Target: wflinfo (executable)
# ----------------------------------------------------------------------------

if(waffle_on_mac)
    # TODO(chadv,due:waffle-1.4): Enable wflinfo on Mac.
    #
    # No one has yet verified that wflinfo successfully builds on Mac. Since it
    # may break the build, we simply disable it for now.
    return()
endif()

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

install(TARGETS wflinfo DESTINATION bin)
