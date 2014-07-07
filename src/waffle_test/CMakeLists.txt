include_directories(
    "${CMAKE_SOURCE_DIR}/include/waffle_test"
    "${CMAKE_SOURCE_DIR}/include/waffle_test/priv"
    )

add_library(waffle_test STATIC
    wt_main.c
    wt_runner.c
    wt_test.c
    )
