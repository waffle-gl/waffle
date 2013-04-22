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
project(waffle C)
cmake_minimum_required(VERSION 2.8)

# -----------------------------------------------------------------------------
# Version Numbers
# -----------------------------------------------------------------------------

# See /doc/versioning-rules.txt.

#
# Release versions.
#
# !!! Must be updated in tandem with the Android.mk !!!
#
set(waffle_major_version "1")
set(waffle_minor_version "2")
set(waffle_patch_version "75")

set(waffle_version "${waffle_major_version}.${waffle_minor_version}.${waffle_patch_version}")


#
# Library versions.
#
# On Linux, libwaffle's filename is:
#   libwaffle-MAJOR.so.0.MINOR.PATCH
#
set(waffle_libname "waffle-${waffle_major_version}")
set(waffle_soversion "0")

# ------------------------------------------------------------------------------
# Includes
# ------------------------------------------------------------------------------

include(CheckThreadLocalStorage)

#
# Include GNUInstallDirs.
#
# Hackishly rename PROJECT_NAME temporarily in order to workaround the default
# of CMAKE_INSTALL_DOCDIR. The default value is "DATAROOTDIR/doc/PROJECT_NAME",
# which evaluates to the undesirable "DATAROOTDIR/doc/waffle". Instead, we
# want to install to "DATAROOTDIR/doc/waffle-MAJOR_VERSION".
#
set(PROJECT_NAME "waffle-${waffle_major_version}")
include(GNUInstallDirs)
set(PROJECT_NAME "waffle")

find_package(PkgConfig)


# ------------------------------------------------------------------------------
# Waffle Options
# ------------------------------------------------------------------------------

# All options here are prefixed with `waffle_`. Most options are cache
# variables set by the user.

# ----------------------------------------------
# Set waffle options
# ----------------------------------------------

option(waffle_build_tests "Build tests" ON)

# Most users don't have the necessary tools installed to build from
# Docbook sources, so require users to explicitly enable building them.
option(waffle_build_manpages "Build manpages" OFF)
option(waffle_build_htmldoc "Build html documentation" OFF)

option(waffle_build_examples "Build examples" ON)

option(waffle_has_glx "Build support for GLX" OFF)
option(waffle_has_wayland "Build support for Wayland" OFF)
option(waffle_has_x11_egl "Build support for X11/EGL" OFF)
option(waffle_has_gbm "Build support for GBM" OFF)

set(waffle_xsltproc "xsltproc"
    CACHE STRING "Tool used to process XSLT stylesheets")

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
check_thread_local_storage()

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
    pkg_check_modules(udev REQUIRED udev)
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

if(NOT waffle_has_tls)
    message(FATAL "Compiler does not support thread local storage with keyword '__thread'")
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
    third_party/khronos
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
# Print summary
# ------------------------------------------------------------------------------

# CMake is annoyingly silent compared to autoconf. The user wants to know what
# was configured how.

message("-----------------------------------------------")
message("")
message("Waffle configuration summary")
message("")
message("Supported platforms: ")
if(waffle_has_glx)
    message("    glx")
endif()
if(waffle_has_wayland)
    message("    wayland")
endif()
if(waffle_has_x11_egl)
    message("    x11_egl")
endif()
if(waffle_has_gbm)
    message("    gbm")
endif()
message("")
message("Dependencies:")
if(waffle_has_egl)
    message("    egl_INCLUDE_DIRS: ${egl_INCLUDE_DIRS}")
    message("    egl_LDFLAGS:      ${egl_LDFLAGS}")
endif()
if(waffle_has_glx)
    message("    gl_INCLUDE_DIRS: ${gl_INCLUDE_DIRS}")
    message("    gl_LDFLAGS:      ${gl_LDFLAGS}")
endif()
if(waffle_has_wayland)
    message("    wayland-client_INCLUDE_DIRS: ${wayland-client_INCLUDE_DIRS}")
    message("    wayland-client_LDFLAGS:      ${wayland-client_LDFLAGS}")
    message("    wayland-egl_INCLUDE_DIRS:    ${wayland-egl_INCLUDE_DIRS}")
    message("    wayland-egl_LDFLAGS:         ${wayland-egl_LDFLAGS}")
endif()
if(waffle_has_x11)
    message("    x11-xcb_INCLUDE_DIRS: ${x11-xcb_INCLUDE_DIRS}")
    message("    x11-xcb_LDFLAGS:      ${x11-xcb_LDFLAGS}")
endif()
if(waffle_has_gbm)
    message("    gbm_INCLUDE_DIRS: ${gbm_INCLUDE_DIRS}")
    message("    gbm_LDFLAGS:      ${gbm_LDFLAGS}")
endif()
message("")
message("Build type:")
message("    ${CMAKE_BUILD_TYPE}")
message("")
message("Tools:")
message("    cc: ${CMAKE_C_COMPILER}")
message("    CFLAGS_base: ${CMAKE_C_FLAGS}")
message("    CFLAGS_debug: ${CMAKE_C_FLAGS_DEBUG}")
message("    CFLAGS_release: ${CMAKE_C_FLAGS_RELEASE}")
if(waffle_build_manpages OR waffle_build_htmldocs)
message("    xsltproc: ${waffle_xsltproc}")
endif()
message("")
message("Install paths:")
message("    CMAKE_INSTALL_PREFIX:          ${CMAKE_INSTALL_PREFIX}")
message("    CMAKE_INSTALL_INCLUDEDIR:      ${CMAKE_INSTALL_INCLUDEDIR}")
message("    CMAKE_INSTALL_LIBDIR:          ${CMAKE_INSTALL_LIBDIR}")
message("    CMAKE_INSTALL_DOCDIR:          ${CMAKE_INSTALL_DOCDIR}")
message("-----------------------------------------------")
