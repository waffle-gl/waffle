install(FILES "${CMAKE_SOURCE_DIR}/README.txt"
              "${CMAKE_SOURCE_DIR}/LICENSE.txt"
        DESTINATION "${waffle_install_docdir}")

install(DIRECTORY "${CMAKE_SOURCE_DIR}/doc/release-notes"
        DESTINATION "${waffle_install_docdir}"
        FILES_MATCHING PATTERN "waffle-${waffle_major_version}.${waffle_minor_version}.*.txt")
