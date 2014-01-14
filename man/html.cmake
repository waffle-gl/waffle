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

set(html_out_dir ${CMAKE_BINARY_DIR}/doc/html/man)

file(MAKE_DIRECTORY ${html_out_dir})

set(html_outputs
    ${html_out_dir}/wflinfo.1.html
    ${html_out_dir}/waffle_attrib_list.3.html
    ${html_out_dir}/waffle_config.3.html
    ${html_out_dir}/waffle_context.3.html
    ${html_out_dir}/waffle_display.3.html
    ${html_out_dir}/waffle_dl.3.html
    ${html_out_dir}/waffle_enum.3.html
    ${html_out_dir}/waffle_error.3.html
    ${html_out_dir}/waffle_gbm.3.html
    ${html_out_dir}/waffle_get_proc_address.3.html
    ${html_out_dir}/waffle_glx.3.html
    ${html_out_dir}/waffle_init.3.html
    ${html_out_dir}/waffle_is_extension_in_string.3.html
    ${html_out_dir}/waffle_make_current.3.html
    ${html_out_dir}/waffle_native.3.html
    ${html_out_dir}/waffle_wayland.3.html
    ${html_out_dir}/waffle_window.3.html
    ${html_out_dir}/waffle_x11_egl.3.html
    ${html_out_dir}/waffle.7.html
    ${html_out_dir}/waffle_feature_test_macros.7.html
    )

set(html_common_sources
    ${common_sources}
    html.xsl
    )

function(waffle_add_html vol title)
    set(output ${html_out_dir}/${title}.${vol}.html)
    set(main_input ${title}.${vol}.xml)
    add_custom_command(
        OUTPUT ${output}
        DEPENDS ${main_input} ${html_common_sources}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${waffle_xsltproc} --xinclude -o ${output} html.xsl ${main_input}
        )
endfunction()

waffle_add_html(1 wflinfo)
waffle_add_html(3 waffle_attrib_list)
waffle_add_html(3 waffle_config)
waffle_add_html(3 waffle_context)
waffle_add_html(3 waffle_display)
waffle_add_html(3 waffle_dl)
waffle_add_html(3 waffle_enum)
waffle_add_html(3 waffle_error)
waffle_add_html(3 waffle_gbm)
waffle_add_html(3 waffle_get_proc_address)
waffle_add_html(3 waffle_glx)
waffle_add_html(3 waffle_init)
waffle_add_html(3 waffle_is_extension_in_string)
waffle_add_html(3 waffle_make_current)
waffle_add_html(3 waffle_native)
waffle_add_html(3 waffle_wayland)
waffle_add_html(3 waffle_window)
waffle_add_html(3 waffle_x11_egl)
waffle_add_html(7 waffle)
waffle_add_html(7 waffle_feature_test_macros)

add_custom_target(html
    ALL
    DEPENDS ${html_outputs}
    )

install(DIRECTORY ${html_out_dir}
        DESTINATION ${CMAKE_INSTALL_FULL_DOCDIR}/html)
