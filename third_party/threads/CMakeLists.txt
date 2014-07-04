include_directories (${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    set(threads_sources threads_win32.c)
else()
    set(threads_sources threads_posix.c)
endif()

add_library(threads_bundled STATIC EXCLUDE_FROM_ALL ${threads_sources})

set_target_properties(threads_bundled
    PROPERTIES
    # This static library is linked into the shared libwaffle and the latter
    # is PIC enabled thus we'll need to enable it here as well.
    POSITION_INDEPENDENT_CODE ON
    )

install(
    FILES LICENSE
    DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    RENAME LICENSE-threads.txt
    COMPONENT coredocs
)
