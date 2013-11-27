# ----------------------------------------------------------------------------
# Target: waffle-unittest (executable)
# ----------------------------------------------------------------------------

add_executable(waffle-unittest
    waffle-unittest.c
    )

target_link_libraries(waffle-unittest
    pthread
    waffle_static
    waffle_test
    )

add_custom_target(waffle_unittest_run
    DEPENDS waffle-unittest
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/waffle-unittest
    )

add_dependencies(check waffle_unittest_run)

