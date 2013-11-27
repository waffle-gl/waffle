set(unittest_out_dir "${CMAKE_BINARY_DIR}/tests")

# ----------------------------------------------------------------------------
# Target: wcore_attrib_list_unittest
# ----------------------------------------------------------------------------

add_executable(wcore_attrib_list_unittest
    core/wcore_attrib_list_unittest.c
)
set_target_properties(wcore_attrib_list_unittest
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${unittest_out_dir}"
)
target_link_libraries(wcore_attrib_list_unittest
    waffle_static
    waffle_test
)
add_custom_target(wcore_attrib_list_unittest_run
    DEPENDS wcore_attrib_list_unittest
    COMMAND "${unittest_out_dir}/wcore_attrib_list_unittest"
)
add_dependencies(check wcore_attrib_list_unittest_run)

# ----------------------------------------------------------------------------
# Target: wcore_config_attrs_unittest
# ----------------------------------------------------------------------------

add_executable(wcore_config_attrs_unittest
    core/wcore_config_attrs_unittest.c
)
set_target_properties(wcore_attrib_list_unittest
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${unittest_out_dir}"
)
target_link_libraries(wcore_config_attrs_unittest
    waffle_static
    waffle_test
)
add_custom_target(wcore_config_attrs_unittest_run
    DEPENDS wcore_config_attrs_unittest
    COMMAND "${unittest_out_dir}/wcore_config_attrs_unittest"
)
add_dependencies(check wcore_config_attrs_unittest_run)

# ----------------------------------------------------------------------------
# Target: wcore_error_unittest
# ----------------------------------------------------------------------------

add_executable(wcore_error_unittest
    core/wcore_error_unittest.c
)
set_target_properties(wcore_attrib_list_unittest
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${unittest_out_dir}"
)
target_link_libraries(wcore_error_unittest
    waffle_static
    waffle_test
)
add_custom_target(wcore_error_unittest_run
    DEPENDS wcore_error_unittest
    COMMAND "${unittest_out_dir}/wcore_error_unittest"
)
add_dependencies(check wcore_error_unittest_run)
