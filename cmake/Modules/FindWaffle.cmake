# - Locate the Waffle library and headers.
# This module defines the following variables:
#     WAFFLE_INCLUDE_DIRS    Full path to directory of the main Waffle header, waffle.h.
#     WAFFLE_LIBRARIES       Full path to the Waffle library.
#     WAFFLE_FOUND           True if Waffle was found.
#     WAFFLE_VERSION_STRING  Waffle's version in form "Major.Minor.Patch".

#=============================================================================
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
#=============================================================================

# See /usr/share/cmake/Modules/readme.txt for CMake official guidelines for
# FindXXX.cmake files.

if(NOT DEFINED WAFFLE_FIND_VERSION_MAJOR)
    set(WAFFLE_FIND_VERSION_MAJOR 1)
endif()

find_path(WAFFLE_INCLUDE_DIRS
    NAMES waffle.h
    PATH_SUFFIXES "waffle-${WAFFLE_FIND_VERSION_MAJOR}"
 )

find_library(WAFFLE_LIBRARIES
    NAMES "waffle-${WAFFLE_FIND_VERSION_MAJOR}"
)

if(WAFFLE_INCLUDE_DIR AND EXISTS "${WAFFLE_INCLUDE_DIR}/waffle_version.h")
    file(STRINGS "${WAFFLE_INCLUDE_DIR}/waffle_version.h" WAFFLE_VERSION_H)
    string(REGEX REPLACE "^.*WAFFLE_MAJOR_VERSION ([0-9]+).*$" "\\1" WAFFLE_MAJOR_VERSION "${WAFFLE_VERSION_H}")
    string(REGEX REPLACE "^.*WAFFLE_MINOR_VERSION ([0-9]+).*$" "\\1" WAFFLE_MINOR_VERSION "${WAFFLE_VERSION_H}")
    string(REGEX REPLACE "^.*WAFFLE_PATCH_VERSION ([0-9]+).*$" "\\1" WAFFLE_PATCH_VERSION "${WAFFLE_VERSION_H}")
    set(WAFFLE_VERSION_STRING "${WAFFLE_MAJOR_VERSION}.${WAFFLE_MINOR_VERSION}.${WAFFLE_PATCH_VERSION}")
endif()

# handle the 'QUIETLY' and 'REQUIRED' arguments and
# set 'WAFFLE_FOUND' to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Waffle
    REQUIRED_VARS WAFFLE_LIBRARIES WAFFLE_INCLUDE_DIRS
    VERSION_VAR WAFFLE_VERSION_STRING
    )

mark_as_advanced(WAFFLE_LIBRARIES WAFFLE_INCLUDE_DIRS)

# Don't expose these variables.
#
# Some were set in the cache and some were not. To be safe, remove the
# variables from the cache *and* local scope.
#
unset(WAFFLE_FIND_VERSION_MAJOR CACHE)
unset(WAFFLE_INCLUDE_DIR CACHE)
unset(WAFFLE_VERSION_H CACHE)

unset(WAFFLE_FIND_VERSION_MAJOR)
unset(WAFFLE_INCLUDE_DIR)
unset(WAFFLE_VERSION_H)
