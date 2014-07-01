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

cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

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
# Targets: check, check-func
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
# Install packaging files: waffle.pc, FindWaffle.cmake
# ------------------------------------------------------------------------------

configure_file(waffle.pc.in ${waffle_libname}.pc @ONLY)

install(
    FILES ${CMAKE_BINARY_DIR}/${waffle_libname}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    COMPONENT pkgconfig
    )

install(
    FILES cmake/Modules/FindWaffle.cmake
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/Modules"
    COMPONENT cmakefind
    )

# ------------------------------------------------------------------------------

include(WafflePrintConfigurationSummary)
