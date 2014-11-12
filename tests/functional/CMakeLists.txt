set(gl_basic_test_sources
    gl_basic_test.c
    )

if(waffle_on_mac)
    list(APPEND gl_basic_test_sources
        gl_basic_cocoa.m
        )
endif()

if(NOT MSVC)
    set_source_files_properties(
        ${gl_basic_test_sources}
        PROPERTIES
            COMPILE_FLAGS "-Wno-initializer-overrides"
    )
endif()

# CMake will pass to the C compiler only C sources. CMake does not recognize the
# .m extension and ignores any such files in the source lists. To coerce CMake
# to pass .m files to the compiler, we must lie and claim that they are
# C sources.
set_source_files_properties(
    ${gl_basic_test_sources}
    PROPERTIES LANGUAGE C
    )

add_executable(gl_basic_test
    ${gl_basic_test_sources}
    )

target_link_libraries(gl_basic_test
    ${waffle_libname}
    waffle_test
    )

add_custom_target(gl_basic_test_run
    COMMAND gl_basic_test
    )

add_dependencies(check-func gl_basic_test_run)

if(VALGRIND_EXECUTABLE)
    add_custom_target(valgrind_gl_basic_test_run
        DEPENDS gl_basic_test
        COMMAND ${VALGRIND_EXECUTABLE} $<TARGET_FILE:gl_basic_test>
        )

    add_dependencies(valgrind-check-func valgrind_gl_basic_test_run)
endif()
