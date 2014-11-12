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

# CMake is annoyingly silent compared to autoconf. The user wants to know what
# was configured how.

message("")
message("-----------------------------------------------")
message("Configuration summary")
message("-----------------------------------------------")
message("")
message("Supported platforms: ")
if(waffle_on_mac)
    message("    cgl")
endif()
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
if(waffle_on_windows)
    message("    wgl")
endif()
message("")
message("Dependencies:")
if(waffle_has_egl)
    message("    egl_INCLUDE_DIRS: ${egl_INCLUDE_DIRS}")
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
message("    CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message("    CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message("    CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}")
message("    CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")
if(waffle_build_manpages OR waffle_build_htmldocs)
message("    xsltproc: ${waffle_xsltproc}")
endif()
message("")
message("Install paths:")
message("    CMAKE_INSTALL_PREFIX:          ${CMAKE_INSTALL_PREFIX}")
message("    CMAKE_INSTALL_INCLUDEDIR:      ${CMAKE_INSTALL_INCLUDEDIR}")
message("    CMAKE_INSTALL_LIBDIR:          ${CMAKE_INSTALL_LIBDIR}")
message("    CMAKE_INSTALL_DOCDIR:          ${CMAKE_INSTALL_DOCDIR}")
message("")
message("-----------------------------------------------")
