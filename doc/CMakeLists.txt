install(FILES "${CMAKE_SOURCE_DIR}/README.txt"
              "${CMAKE_SOURCE_DIR}/LICENSE.txt"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}")

install(DIRECTORY "${CMAKE_SOURCE_DIR}/doc/release-notes"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
        FILES_MATCHING PATTERN "waffle-${waffle_major_version}.${waffle_minor_version}.*.txt")
