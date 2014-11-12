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

if(waffle_on_linux)
    if(NOT waffle_has_glx AND NOT waffle_has_wayland AND
       NOT waffle_has_x11_egl AND NOT waffle_has_gbm)
        message(FATAL_ERROR
                "Must enable at least one of: "
                "waffle_has_glx, waffle_has_wayland, "
                "waffle_has_x11_egl, waffle_has_gbm.")
    endif()
    if(waffle_has_gbm)
        if(NOT gbm_FOUND)
            set(gbm_missing_deps
                "${gbm_missing_deps} gbm"
                )
        endif()
        if(NOT libudev_FOUND)
            set(gbm_missing_deps
                "${gbm_missing_deps} libudev"
                )
        endif()
        if(NOT egl_FOUND)
            set(gbm_missing_deps
                "${gbm_missing_deps} egl"
                )
        endif()
        if(gbm_missing_deps)
            message(FATAL_ERROR "gbm dependency is missing: ${gbm_missing_deps}")
        endif()
    endif()
    if(waffle_has_glx)
        if(NOT gl_FOUND)
            set(glx_missing_deps
                "${glx_missing_deps} gl"
                )
        endif()
        if(NOT x11-xcb_FOUND)
            set(glx_missing_deps
                "${glx_missing_deps} x11-xcb"
                )
        endif()
        if(glx_missing_deps)
            message(FATAL_ERROR "glx dependency is missing: ${glx_missing_deps}")
        endif()
    endif()
    if(waffle_has_wayland)
        if(NOT wayland-client_FOUND)
            set(wayland_missing_deps
                "${wayland_missing_deps} wayland-client>=1"
                )
        endif()
        if(NOT wayland-egl_FOUND)
            set(wayland_missing_deps
                "${wayland_missing_deps} wayland-egl>=9.1"
                )
        endif()
        if(NOT egl_FOUND)
            set(wayland_missing_deps
                "${wayland_missing_deps} egl"
                )
        endif()
        if(wayland_missing_deps)
            message(FATAL_ERROR "wayland dependency is missing: ${wayland_missing_deps}")
        endif()
    endif()
    if(waffle_has_x11_egl)
        if(NOT x11-xcb_FOUND)
            set(x11_egl_missing_deps
                "${x11_egl_missing_deps} x11-xcb"
                )
        endif()
        if(NOT egl_FOUND)
            set(x11_egl_missing_deps
                "${x11_egl_missing_deps} egl"
                )
        endif()
        if(x11_egl_missing_deps)
            message(FATAL_ERROR "x11_egl dependency is missing: ${x11_egl_missing_deps}")
        endif()
    endif()
elseif(waffle_on_mac)
    if(waffle_has_gbm)
        message(FATAL_ERROR "Option is not supported on Darwin: waffle_has_gbm.")
    endif()
    if(waffle_has_glx)
        message(FATAL_ERROR "Option is not supported on Darwin: waffle_has_glx.")
    endif()
    if(waffle_has_wayland)
        message(FATAL_ERROR "Option is not supported on Darwin: waffle_has_wayland.")
    endif()
    if(waffle_has_x11_egl)
        message(FATAL_ERROR "Option is not supported on Darwin: waffle_has_x11_egl.")
    endif()
elseif(waffle_on_windows)
    if(waffle_has_gbm)
        message(FATAL_ERROR "Option is not supported on Windows: waffle_has_gbm.")
    endif()
    if(waffle_has_glx)
        message(FATAL_ERROR "Option is not supported on Windows: waffle_has_glx.")
    endif()
    if(waffle_has_wayland)
        message(FATAL_ERROR "Option is not supported on Windows: waffle_has_wayland.")
    endif()
    if(waffle_has_x11_egl)
        message(FATAL_ERROR "Option is not supported on Windows: waffle_has_x11_egl.")
    endif()
endif()
