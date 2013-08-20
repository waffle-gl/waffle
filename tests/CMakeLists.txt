include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(waffle_test)
add_subdirectory(unittests)
add_subdirectory(functional)

#
# Run only pure unit tests.
#
add_custom_target(check
    DEPENDS waffle-unittest
    COMMAND ${CMAKE_BINARY_DIR}/tests/unittests/waffle-unittest
)

#
# Run functional and unit tests.
#
# The unit tests are ran first (due to the depenency on 'check'). If they
# fail, then no subsequent tests are ran.
#
add_custom_target(check-func
    DEPENDS check gl_basic_test
    COMMAND ${CMAKE_BINARY_DIR}/tests/functional/gl_basic_test
)
