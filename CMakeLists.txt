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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
project(waffle1 C)
cmake_minimum_required(VERSION 2.8)


# ------------------------------------------------------------------------------
# Includes
# ------------------------------------------------------------------------------

include(Options.cmake)
include(WaffleDefineInternalOptions)
include(WaffleValidateOptions)
include(WaffleDefineVersion)
include(WaffleDefineCompilerFlags)
include(GNUInstallDirs)

find_package(PkgConfig)

# ------------------------------------------------------------------------------
# Find Libraries
# ------------------------------------------------------------------------------

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    find_package(OpenGL REQUIRED)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
endif()

if(waffle_has_egl)
    pkg_check_modules(egl REQUIRED egl)
endif()

if(waffle_has_glx)
    pkg_check_modules(gl REQUIRED gl)
endif()

if(waffle_has_wayland)
    pkg_check_modules(wayland-client REQUIRED wayland-client>=1)
    pkg_check_modules(wayland-egl REQUIRED wayland-egl>=9.1)
endif()

if(waffle_has_x11)
    pkg_check_modules(x11-xcb REQUIRED x11-xcb)
endif()

if(waffle_has_gbm)
    pkg_check_modules(gbm REQUIRED gbm)
    pkg_check_modules(libudev REQUIRED libudev)
endif()

# ------------------------------------------------------------------------------
# Add subdirectories
# ------------------------------------------------------------------------------

include_directories(
    include/waffle
    src
    )

add_subdirectory(cmake)
add_subdirectory(doc)
add_subdirectory(pkg)
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
# Target: check
# ------------------------------------------------------------------------------

# Run only unit tests.

if(waffle_build_tests)
    add_custom_target(check
        DEPENDS waffle-unittest
        COMMAND ${CMAKE_BINARY_DIR}/tests/unittests/waffle-unittest
        )
endif()

# ------------------------------------------------------------------------------
# Target: check-func
# ------------------------------------------------------------------------------

# Run functional and unit tests.
#
# The unit tests are ran first (due to the depenency on 'check'). If they
# fail, then no subsequent tests are ran.

if(waffle_build_tests)
    add_custom_target(check-func
        DEPENDS check gl_basic_test
        COMMAND ${CMAKE_BINARY_DIR}/tests/functional/gl_basic_test
        )
endif()

# ------------------------------------------------------------------------------
# Install: waffle.pc
# ------------------------------------------------------------------------------

configure_file(waffle.pc.in ${waffle_libname}.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/${waffle_libname}.pc
        DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)

# ------------------------------------------------------------------------------

include(WafflePrintConfigurationSummary)
