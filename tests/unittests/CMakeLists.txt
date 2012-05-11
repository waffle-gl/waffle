# ----------------------------------------------------------------------------
# Target: waffle-unittest (executable)
# ----------------------------------------------------------------------------

add_executable(waffle-unittest
    waffle-unittest.c

    waffle/api/waffle_attrib_list_unittest.c
    waffle/core/wcore_error_unittest.c
    waffle/core/wcore_config_attrs_unittest.c
    )

target_link_libraries(waffle-unittest
    pthread
    waffle
    waffle_test
    )
