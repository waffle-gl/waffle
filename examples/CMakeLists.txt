install(
    FILES
        Makefile.example
        gl_basic.c
        simple-x11-egl.c
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/examples"
    COMPONENT examples
    )

# ----------------------------------------------------------------------------
# Target: simple-x11-egl (executable)
# ----------------------------------------------------------------------------

if(waffle_on_linux AND waffle_has_x11_egl)
    add_executable(simple-x11-egl simple-x11-egl.c)
    target_link_libraries(simple-x11-egl ${waffle_libname})
endif()

# ----------------------------------------------------------------------------
# Target: gl_basic_nacl (executable + JSON manifest file)
# ----------------------------------------------------------------------------
if (waffle_has_nacl)
    add_executable(gl_basic_nacl.nexe gl_basic.c)
    include_directories(${nacl_INCLUDE_DIRS})

    # Set path where to create and copy required files.
    set(nacl_example_path ${PROJECT_SOURCE_DIR}/html/gl_basic_nacl)

    file(MAKE_DIRECTORY ${nacl_example_path})

    set_target_properties(gl_basic_nacl.nexe
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${nacl_example_path}
    )

    target_link_libraries(gl_basic_nacl.nexe
        ${waffle_libname}
        ${nacl_LDFLAGS}
        -lppapi_simple
        -lnacl_io
        )

    # Create .nmf file that contains JSON manifest for the .nexe required by NaCl
    # NOTE, this serves as example only and user might need to recreate nmf file
    # suitable for the used environment. Extra entry for libppapi_gles2.so is added
    # because we are not linking with it but require its presence during execution.
    # --no-arch-prefix is used because '-x' has no option to set architecture for
    # the extra entry.
    add_custom_command(
        OUTPUT gl_basic_nacl.nmf
        COMMAND ${nacl_root}/tools/create_nmf.py --no-arch-prefix -x libppapi_gles2.so:libppapi_gles2.so -L${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${nacl_example_path}/gl_basic_nacl.nexe -o ${nacl_example_path}/gl_basic_nacl.nmf
        DEPENDS gl_basic_nacl.nexe
        COMMENT "Create JSON manifest"
        VERBATIM
    )

    add_custom_target(gl_basic_create_json_manifest ALL
                      DEPENDS gl_basic_nacl.nmf)

    # install index.html that loads gl_basic_nacl.nmf
    file(INSTALL index.html DESTINATION ${nacl_example_path})

endif()

# ----------------------------------------------------------------------------
# Target: gl_basic (executable)
# ----------------------------------------------------------------------------

if(waffle_has_nacl)
    return()
endif()

add_executable(gl_basic gl_basic.c)
target_link_libraries(gl_basic ${waffle_libname} ${GETOPT_LIBRARIES})

if(waffle_on_mac)
    set_target_properties(gl_basic
        PROPERTIES
        COMPILE_FLAGS "-ObjC"
        )
endif()
