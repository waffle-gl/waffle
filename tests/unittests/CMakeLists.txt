# ----------------------------------------------------------------------------
# Target: waffle-unittest (executable)
# ----------------------------------------------------------------------------

add_executable(waffle-unittest
    waffle-unittest.c

    ${CMAKE_SOURCE_DIR}/src/waffle/api/waffle_attrib_list_unittest.c
    ${CMAKE_SOURCE_DIR}/src/waffle/core/wcore_error_unittest.c
    ${CMAKE_SOURCE_DIR}/src/waffle/core/wcore_config_attrs_unittest.c
    )

target_link_libraries(waffle-unittest
    pthread
    ${waffle_libname}
    waffle_test
    )
