# ----------------------------------------------------------------------------
# Target: simple-x11-egl (executable)
# ----------------------------------------------------------------------------

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    add_executable(simple-x11-egl simple-x11-egl.c)
    target_link_libraries(simple-x11-egl ${waffle_libname})
endif()

# ----------------------------------------------------------------------------
# Target: gl_basic (executable)
# ----------------------------------------------------------------------------

add_executable(gl_basic gl_basic.c)
target_link_libraries(gl_basic ${waffle_libname})

if("${CMAKE_SYSTEM_NAME}" STREQUAL Darwin)
    set_target_properties(gl_basic
        PROPERTIES
        COMPILE_FLAGS "-ObjC"
        )
endif()
