add_definitions(
    -DWAFFLE_API_VERSION=${waffle_api_version}
    -DWAFFLE_API_EXPERIMENTAL
    )

add_subdirectory(functional)
