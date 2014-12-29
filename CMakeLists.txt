# Copyright 2012 Intel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# - Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

project(waffle1 C)

cmake_minimum_required(VERSION 2.8.11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

# Set the default location for all build artifacts to traditionally named
# top-level directories.  CMake's default location for build artifacts varies
# per artifact and is hard-to-guess.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

include(WaffleDefineOS)
include(WaffleFindDependencies)
include(Options.cmake)
include(WaffleDefineInternalOptions)
include(WaffleValidateOptions)
include(WaffleDefineVersion)
include(WaffleDefineCompilerFlags)
include(GNUInstallDirs)

if(waffle_build_tests)
    include(WaffleCMocka)
endif()

find_package(PkgConfig)

# ------------------------------------------------------------------------------
# Targets: check, check-func, valgrind-check, valgrind-check-func
# ------------------------------------------------------------------------------

#
# Target 'check' runs unit tests, but no functional tests.
#
add_custom_target(check)

#
# Target 'check-func' runs functional tests as well as unit tests.
#
# The unit tests are ran first (due to the depenency on 'check'). If any unit
# test fails, then no functional tests run.
#
add_custom_target(check-func
    DEPENDS check
    )

find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE)
    # Runs the 'check' target under valgrind.
    add_custom_target(valgrind-check
        DEPENDS check
        )

    # Runs the 'check-func' target under valgrind.
    add_custom_target(valgrind-check-func
        DEPENDS valgrind-check check-func
        )
endif()

# ------------------------------------------------------------------------------
# Add subdirectories
# ------------------------------------------------------------------------------

if(waffle_on_mac)
    link_libraries(
        ${COCOA_FRAMEWORK}
        ${CORE_FOUNDATION_FRAMEWORK}
        )
endif()

include_directories(
    include
    include/waffle
    src
    )

add_subdirectory(third_party/threads)
include_directories(
    third_party/threads
    )
set(THREADS_LIBRARIES threads_bundled)

if(MSVC)
    add_subdirectory(third_party/getopt)
    include_directories(
        third_party/getopt
        )
    set(GETOPT_LIBRARIES getopt_bundled)
endif()

add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(man)

if(waffle_build_tests)
    add_subdirectory(tests)
endif()

if(waffle_build_examples)
    add_subdirectory(examples)
endif()

# ------------------------------------------------------------------------------
# Install packaging files: waffle.pc WaffleConfigVersion.cmake, and
# WaffleConfig.cmake
# ------------------------------------------------------------------------------

configure_file(waffle.pc.in ${waffle_libname}.pc @ONLY)

install(
    FILES ${CMAKE_BINARY_DIR}/${waffle_libname}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    COMPONENT pkgconfig
    )

set(ConfigPackageLocation "${CMAKE_INSTALL_LIBDIR}/cmake/Waffle")
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/cmake/Modules/WaffleConfigVersion.cmake"
    VERSION "${waffle_version}"
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    cmake/Modules/WaffleConfig.cmake.in
    cmake/Modules/WaffleConfig.cmake
    INSTALL_DESTINATION "${ConfigPackageLocation}"
    PATH_VARS CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_INCLUDEDIR
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(
    FILES
        "${CMAKE_BINARY_DIR}/cmake/Modules/WaffleConfigVersion.cmake"
        "${CMAKE_BINARY_DIR}/cmake/Modules/WaffleConfig.cmake"
    DESTINATION "${ConfigPackageLocation}"
    COMPONENT devel
)

# ------------------------------------------------------------------------------
# Install core documentation
# ------------------------------------------------------------------------------

install(
    FILES
        "README.txt"
        "LICENSE.txt"
        "HACKING.txt"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    COMPONENT coredocs
    )

# ------------------------------------------------------------------------------

include(WafflePrintConfigurationSummary)

set (CPACK_SET_DESTDIR ON)
set (CPACK_PACKAGE_VERSION_MAJOR ${waffle_major_version})
set (CPACK_PACKAGE_VERSION_MINOR ${waffle_minor_version})
set (CPACK_PACKAGE_VERSION_PATCH ${waffle_patch_version})

# cpack detects win64 vs win32 only when msvc is available
# reported upstream, fix (likely post cmake 3.0) pending
if (MINGW)
        if (CMAKE_SIZEOF_VOID_P EQUAL 8)
                set (CPACK_SYSTEM_NAME win64)
        endif ()
endif ()

# See http://www.vtk.org/Wiki/CMake:CPackPackageGenerators
if (WIN32)
        set (CPACK_GENERATOR "ZIP")
else ()
        set (CPACK_GENERATOR "TBZ2")
endif ()

include(CPack)
