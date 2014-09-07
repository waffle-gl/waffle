install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/doc/release-notes"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    COMPONENT releasenotes
    FILES_MATCHING PATTERN "waffle-${waffle_major_version}.${waffle_minor_version}.*.txt"
    )
