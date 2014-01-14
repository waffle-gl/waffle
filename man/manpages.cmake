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

set(man_out_dir ${CMAKE_CURRENT_BINARY_DIR})

file(MAKE_DIRECTORY ${man_out_dir}/man1)
file(MAKE_DIRECTORY ${man_out_dir}/man3)
file(MAKE_DIRECTORY ${man_out_dir}/man7)

set(man_outputs
    ${man_out_dir}/man1/wflinfo.1
    ${man_out_dir}/man3/waffle_attrib_list.3
    ${man_out_dir}/man3/waffle_config.3
    ${man_out_dir}/man3/waffle_context.3
    ${man_out_dir}/man3/waffle_display.3
    ${man_out_dir}/man3/waffle_dl.3
    ${man_out_dir}/man3/waffle_enum.3
    ${man_out_dir}/man3/waffle_error.3
    ${man_out_dir}/man3/waffle_gbm.3
    ${man_out_dir}/man3/waffle_get_proc_address.3
    ${man_out_dir}/man3/waffle_glx.3
    ${man_out_dir}/man3/waffle_init.3
    ${man_out_dir}/man3/waffle_is_extension_in_string.3
    ${man_out_dir}/man3/waffle_make_current.3
    ${man_out_dir}/man3/waffle_native.3
    ${man_out_dir}/man3/waffle_wayland.3
    ${man_out_dir}/man3/waffle_window.3
    ${man_out_dir}/man3/waffle_x11_egl.3
    ${man_out_dir}/man7/waffle.7
    ${man_out_dir}/man7/waffle_feature_test_macros.7
    )

set(man_common_sources
    ${common_sources}
    manpage.xsl
    )

function(waffle_add_manpage vol title)
    add_custom_command(
        OUTPUT man${vol}/${title}.${vol}
        DEPENDS ${title}.${vol}.xml ${man_common_sources}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${waffle_xsltproc} --xinclude -o ${man_out_dir}/man${vol}/ manpage.xsl ${title}.${vol}.xml
        )
endfunction()

waffle_add_manpage(1 wflinfo)
waffle_add_manpage(3 waffle_attrib_list)
waffle_add_manpage(3 waffle_config)
waffle_add_manpage(3 waffle_context)
waffle_add_manpage(3 waffle_display)
waffle_add_manpage(3 waffle_dl)
waffle_add_manpage(3 waffle_enum)
waffle_add_manpage(3 waffle_error)
waffle_add_manpage(3 waffle_gbm)
waffle_add_manpage(3 waffle_get_proc_address)
waffle_add_manpage(3 waffle_glx)
waffle_add_manpage(3 waffle_init)
waffle_add_manpage(3 waffle_is_extension_in_string)
waffle_add_manpage(3 waffle_make_current)
waffle_add_manpage(3 waffle_native)
waffle_add_manpage(3 waffle_wayland)
waffle_add_manpage(3 waffle_window)
waffle_add_manpage(3 waffle_x11_egl)
waffle_add_manpage(7 waffle)
waffle_add_manpage(7 waffle_feature_test_macros)

add_custom_target(man
    ALL
    DEPENDS ${man_outputs}
    )

install(DIRECTORY ${man_out_dir}/man1
        DESTINATION ${CMAKE_INSTALL_FULL_MANDIR})
install(DIRECTORY ${man_out_dir}/man3
        DESTINATION ${CMAKE_INSTALL_FULL_MANDIR})
install(DIRECTORY ${man_out_dir}/man7
        DESTINATION ${CMAKE_INSTALL_FULL_MANDIR})
