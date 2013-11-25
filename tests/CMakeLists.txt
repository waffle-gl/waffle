add_definitions(
    -DWAFFLE_API_VERSION=${waffle_api_version}
    -DWAFFLE_API_EXPERIMENTAL
    )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(waffle_test)
add_subdirectory(unittests)
add_subdirectory(functional)
