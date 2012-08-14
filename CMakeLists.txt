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

project(waffle C)
cmake_minimum_required(VERSION 2.8)

# Release version.
set(waffle_version 0.3.1)

# Library version.
#
# The library version scheme follows the libtool guidelines and is independent
# of the release version. It is appended to libwaffle.so as libwaffle.so.X.X.X.
# See http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html.
set(waffle_so_version "2.1.0")


# ------------------------------------------------------------------------------
# Includes
# ------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")
include(CheckThreadLocalStorage)


# ------------------------------------------------------------------------------
# Waffle Options
# ------------------------------------------------------------------------------

# All options here are prefixed with `waffle_`. Most options are cache
# variables set by the user.

# ----------------------------------------------
# Set waffle options
# ----------------------------------------------

option(waffle_build_tests "Build tests" ON)
set(waffle_build_examples "Build examples" ON)

option(waffle_has_glx "Build support for GLX" OFF)
option(waffle_has_wayland "Build support for Wayland" OFF)
option(waffle_has_x11_egl "Build support for X11/EGL" OFF)

set(waffle_install_includedir "${CMAKE_INSTALL_PREFIX}/include"
    CACHE PATH "Directory where header files will be installed")

set(waffle_install_libdir "${CMAKE_INSTALL_PREFIX}/lib"
    CACHE PATH "Directory where libraries will be installed")

set(waffle_install_docdir "${CMAKE_INSTALL_PREFIX}/share/doc/waffle"
    CACHE PATH "Directory where documentation will be installed")

#
# Set internal options.
#

# Set the following:
#     waffle_has_tls
#     waffle_tls_model_initial_exec
check_thread_local_storage()

if(waffle_has_wayland OR waffle_has_x11_egl)
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
    if(NOT waffle_has_glx AND NOT waffle_has_wayland AND NOT waffle_has_x11_egl)
        message(FATAL_ERROR "Must enable at least one of: waffle_has_glx, waffle_has_wayland, waffle_has_x11_egl.")
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

function(waffle_find_library var name)
    find_library("${var}" "${name}")

    if("${var}")
        message(STATUS "Library ${name} found: ${${var}}")
    else()
        message(STATUS "Library ${name} not found")
    endif()
endfunction()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    find_package(OpenGL REQUIRED)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
endif()

if(waffle_has_egl)
    waffle_find_library(waffle_EGL_library EGL)
endif()

if(waffle_has_glx)
    waffle_find_library(waffle_GL_library GL)
endif()

if(waffle_has_wayland)
    waffle_find_library(waffle_wayland-client_library wayland-client)
    waffle_find_library(waffle_wayland-egl_library wayland-egl)
endif()

if(waffle_has_x11)
    waffle_find_library(waffle_xcb_library xcb)
    waffle_find_library(waffle_X11_library X11)
    waffle_find_library(waffle_X11-xcb_library X11-xcb)
endif()


# ------------------------------------------------------------------------------
# Compiler Flags
# ------------------------------------------------------------------------------

# FIXME: Only enable c99 if compiler supports it.
set(CMAKE_C_FLAGS "--std=c99 -Wall -Werror")

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

# ------------------------------------------------------------------------------
# Add subdirectories
# ------------------------------------------------------------------------------

include_directories(include src)

add_subdirectory(src)
add_subdirectory(include)

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

configure_file(waffle.pc.in waffle.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/waffle.pc
        DESTINATION ${waffle_install_libdir}/pkgconfig)

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
message("")
message("Libraries:")
if(DEFINED waffle_EGL_library)
    message("    EGL: ${waffle_EGL_library}")
endif()
if(DEFINED waffle_GL_library)
    message("    GL: ${waffle_GL_library}")
endif()
if(waffle_has_wayland)
    message("    wayland-client: ${waffle_wayland-client_library}")
    message("    wayland-egl: ${waffle_wayland-egl_library}")
endif()
if(DEFINED waffle_X11-xcb_library)
    message("    xcb: ${waffle_xcb_library}")
    message("    X11: ${waffle_X11_library}")
    message("    X11-xcb: ${waffle_X11-xcb_library}")
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
message("")
message("Install paths:")
message("    includedir: ${waffle_install_includedir}")
message("    libdir: ${waffle_install_libdir}")
message("    docdir: ${waffle_install_docdir}")
message("-----------------------------------------------")
