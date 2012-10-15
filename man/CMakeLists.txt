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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man3)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man7)

set(man_outputs
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_attrib_list.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_config.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_context.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_display.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_dl.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_enum.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_error.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_gbm.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_gl_misc.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_glx.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_init.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_native.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_wayland.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_window.3
    ${CMAKE_CURRENT_BINARY_DIR}/man3/waffle_x11_egl.3
    ${CMAKE_CURRENT_BINARY_DIR}/man7/waffle.7
    )

set(man_common_sources
    common/author-chad.versace.xml
    common/copyright.xml
    common/error-codes.xml
    common/legalnotice.xml
    common/return-value.xml
    )

function(waffle_add_manpage vol title)
    add_custom_command(
        OUTPUT man${vol}/${title}.${vol}
        DEPENDS ${title}.${vol}.xml ${man_common_sources}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${waffle_xsltproc} --xinclude -o ${CMAKE_CURRENT_BINARY_DIR}/man${vol}/ ${waffle_manpage_xsl} ${title}.${vol}.xml
        )
endfunction()

waffle_add_manpage(3 waffle_attrib_list)
waffle_add_manpage(3 waffle_config)
waffle_add_manpage(3 waffle_context)
waffle_add_manpage(3 waffle_display)
waffle_add_manpage(3 waffle_dl)
waffle_add_manpage(3 waffle_enum)
waffle_add_manpage(3 waffle_error)
waffle_add_manpage(3 waffle_gbm)
waffle_add_manpage(3 waffle_gl_misc)
waffle_add_manpage(3 waffle_glx)
waffle_add_manpage(3 waffle_init)
waffle_add_manpage(3 waffle_native)
waffle_add_manpage(3 waffle_wayland)
waffle_add_manpage(3 waffle_window)
waffle_add_manpage(3 waffle_x11_egl)
waffle_add_manpage(7 waffle)

add_custom_target(man
    ALL
    DEPENDS ${man_outputs}
    )

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man3
        DESTINATION ${CMAKE_INSTALL_FULL_MANDIR})
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man7
        DESTINATION ${CMAKE_INSTALL_FULL_MANDIR})
