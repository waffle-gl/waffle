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
    waffle
    waffle_test
    )

# ----------------------------------------------------------------------------
# Target: waffle-check
# ----------------------------------------------------------------------------

add_custom_target(waffle-check
    DEPENDS waffle-unittest
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/waffle-unittest
    )

add_dependencies(check waffle-check)
