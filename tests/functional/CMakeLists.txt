link_libraries(
    ${waffle_libname}
    waffle_test
    )

set(gl_basic_test_sources
    gl_basic_test.c
    )

if(waffle_on_mac)
    list(APPEND gl_basic_test_sources
        gl_basic_cocoa.m
        )
endif()

set_source_files_properties(
    ${gl_basic_test_sources}
    PROPERTIES
        COMPILE_FLAGS "-Wno-initializer-overrides"
)

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
