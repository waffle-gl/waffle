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

find_package(PkgConfig)

#
# Macro waffle_pkg_config is a smart wrapper around CMake's standard
# pkg_check_modules macro.
#
# waffle_pkg_config fixes a fatal flaw of pkg_check_modules.  Unlike
# pkg_check_modules, waffle_pkg_config calls pkg-config and checks for the
# requested package *each* time the macro is called.
#
# Flaws in pkg_check_modules
# --------------------------
# For each package prefix (that is, the first argument to pkg_check_modules),
# pkg_check_modules calls pkg-config only if no previous invocation succeeded
# in finding the package.  Therefore, if someone increments the required
# version parameter of some call to pkg_check_modules, then pkg_check_modules
# does not check for the new version of the package if a previous invocation of
# CMake succeeded in finding the older version.
#
# For example, consider the following patch to some CMakeLists.txt.
#     - pkg_check_modules(FOO REQUIRED foo>=1.0)
#     + pkg_check_modules(FOO REQUIRED foo>=2.0)
# Suppose that the system has foo-1.0 installed but not foo-2.0; and that
# a previous invocation of CMake suceeded in finding foo-1.0. Future
# invocations of CMake will continue to succeed even though the system lacks
# foo-2.0.
#
macro(waffle_pkg_config ...)
    unset(${ARGV0}_FOUND CACHE)
    pkg_check_modules(${ARGV})
endmacro()

if(waffle_on_mac)
    find_package(OpenGL REQUIRED)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
endif()


if(waffle_on_linux)
    # waffle_has_egl
    waffle_pkg_config(egl egl)

    # waffle_has_glx
    waffle_pkg_config(gl gl)

    # waffle_has_wayland
    waffle_pkg_config(wayland-client wayland-client>=1)
    waffle_pkg_config(wayland-egl wayland-egl>=9.1)

    # waffle_has_x11
    waffle_pkg_config(x11-xcb x11-xcb)

    # waffle_has_gbm
    waffle_pkg_config(gbm gbm)
    waffle_pkg_config(libudev libudev)
endif()
