# Copyright 2013 Intel Corporation
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

#
# Waffle's version scheme follows the rules [1] set by the Apache Portable
# Runtime Project.  The scheme allows multiple versions of a library and its
# headers to be installed in parallel.
#
# [1] http://apr.apache.org/versioning.html
#

#
# Release versions.
#
# !!! Must be updated in tandem with the Android.mk !!!
#
# Bump this to x.y.90 immediately after each waffle-x.y.0 release.
#
set(waffle_major_version "1")
set(waffle_minor_version "4")
set(waffle_patch_version "90")

set(waffle_version "${waffle_major_version}.${waffle_minor_version}.${waffle_patch_version}")


#
# Library versions.
#
# On Linux, libwaffle's filename is:
#   libwaffle-MAJOR.so.0.MINOR.PATCH
#
set(waffle_libname "waffle-${waffle_major_version}")
set(waffle_soversion "0")


#
# API version.
#
# This gets bumped to x.(y+1) immediately after each waffle-x.y.0 release.
#
math(EXPR waffle_api_version "(${waffle_major_version} << 8) | ${waffle_minor_version}")
if (waffle_patch_version GREATER 89)
    math(EXPR waffle_api_version "${waffle_api_version} + 1")
endif()
