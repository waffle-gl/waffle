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
include(WaffleDefineVersion)
include(WaffleCheckThreadLocalStorage)
include(GNUInstallDirs)

find_package(PkgConfig)

# ----------------------------------------------
# Emit errors for removed options
# ----------------------------------------------

if(DEFINED waffle_install_includedir)
    message(FATAL_ERROR "Option waffle_install_includedir has been "
            "replaced by CMAKE_INSTALL_INCLUDEDIR. You may need to edit the "
            "CMakeCache to delete the option. See /doc/building.txt for "
            "details.")
endif()

if(DEFINED waffle_install_libdir)
    message(FATAL_ERROR "Option waffle_install_libdir has been "
            "replaced by CMAKE_INSTALL_LIBDIR. You may need to edit the "
            "CMakeCache to delete the option. See /doc/building.txt for "
            "details.")
endif()

if(DEFINED waffle_install_docdir)
    message(FATAL_ERROR "Option waffle_install_docdir has been "
            "replaced by CMAKE_INSTALL_DOCDIR. You may need to edit the "
            "CMakeCache to delete the option. See /doc/building.txt for "
            "details.")
endif()

#
# Set internal options.
#

# Set the following:
#     waffle_has_tls
#     waffle_tls_model_initial_exec
waffle_check_thread_local_storage()

if(waffle_has_wayland OR waffle_has_x11_egl OR waffle_has_gbm)
    set(waffle_has_egl TRUE)
else(waffle_has_wayland OR waffle_has_x11_egl)
    set(waffle_has_egl FALSE)
endif()

if(waffle_has_glx OR waffle_has_x11_egl)
    set(waffle_has_x11 TRUE)
else()
    set(waffle_has_x11 FALSE)
endif()

# ----------------------------------------------
# Validate waffle options
# ----------------------------------------------

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    if(NOT waffle_has_glx AND NOT waffle_has_wayland AND
       NOT waffle_has_x11_egl AND NOT waffle_has_gbm)
        message(FATAL_ERROR
                "Must enable at least one of: "
                "waffle_has_glx, waffle_has_wayland, "
                "waffle_has_x11_egl, waffle_has_gbm.")
    endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    if(waffle_has_glx)
        message(FATAL_ERROR "Option is not supported on Darwin: waffle_has_glx.")
    endif()
    if(waffle_has_wayland)
        message(FATAL_ERROR "Option is not supported on Darwin: waffle_has_wayland.")
    endif()
    if(waffle_has_x11_egl)
        message(FATAL_ERROR "Option is not supported on Darwin: waffle_has_x11_egl.")
    endif()
endif()


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
# Compiler Flags
# ------------------------------------------------------------------------------

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c99 -Wall -Werror=implicit-function-declaration")
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    # On MacOS, the SSE2 headers trigger this error.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=missing-prototypes")
endif()

set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -DDEBUG")

# Produce enough debug info for generating backtraces, but not
# single-stepping.
set(CMAKE_C_FLAGS_RELEASE "-g1 -O2 -DNDEBUG")

if(waffle_has_tls)
    add_definitions(-DWAFFLE_HAS_TLS)
endif()

if(waffle_has_tls_model_initial_exec)
    add_definitions(-DWAFFLE_HAS_TLS_MODEL_INITIAL_EXEC)
endif()

#
# Define macros WAFFLE_HAS_{PLATFORM}.
#
# Android is not in this list because it uses a separate build system.
#

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    add_definitions(-DWAFFLE_HAS_CGL)
endif()

if(waffle_has_glx)
    add_definitions(-DWAFFLE_HAS_GLX)
endif()

if(waffle_has_wayland)
    add_definitions(-DWAFFLE_HAS_WAYLAND)
endif()

if(waffle_has_x11_egl)
    add_definitions(-DWAFFLE_HAS_X11_EGL)
endif()

if(waffle_has_gbm)
    add_definitions(-DWAFFLE_HAS_GBM)
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
