# Copyright © 2018 Intel Corporation
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

files_man_common = files(
  'common/author-chad.versace.xml',
  'common/copyright.xml',
  'common/issues.xml',
  'common/error-codes.xml',
  'common/legalnotice.xml',
  'common/return-value.xml',
)

# Each entry is a 4 part tuple consisting of
# <man section>, <name>, <extra pages that are <name>_<page>>, <extra_pages>
man_targets = [
  ['1', 'wflinfo', [], []],
  ['3', 'waffle_attrib_list', ['get', 'get_with_default', 'length', 'update'], []],
  ['3', 'waffle_config', ['choose', 'destroy', 'get_native'], []],
  ['3', 'waffle_context', ['create', 'destroy', 'get_native'], []],
  ['3', 'waffle_display', ['connect', 'disconnect', 'get_native', 'supports_context_api'], []],
  ['3', 'waffle_dl', ['can_open', 'sym'], []],
  ['3', 'waffle_enum', ['to_string'], []],
  ['3', 'waffle_error', ['get_code', 'get_info', 'to_string'], []],
  ['3', 'waffle_gbm', ['config', 'context', 'display', 'window'], []],
  ['3', 'waffle_get_proc_address', [], []],
  ['3', 'waffle_glx', ['config', 'context', 'display', 'window'], []],
  ['3', 'waffle_init', [], []],
  ['3', 'waffle_is_extension_in_string', [], []],
  ['3', 'waffle_make_current', [], ['waffle_get_current_display', 'waffle_get_current_window', 'waffle_get_current_context']],
  ['3', 'waffle_native', ['config', 'context', 'display', 'window'], []],
  ['3', 'waffle_teardown', [], []],
  ['3', 'waffle_wayland', ['config', 'context', 'display', 'window'], []],
  ['3', 'waffle_window', ['create', 'destroy', 'get_native', 'show', 'swap_buffers'], []],
  ['3', 'waffle_x11_egl', ['config', 'context', 'display', 'window'], []],
  ['7', 'waffle', [], []],
  ['7', 'waffle_feature_test_macros', [], []],
]


if get_option('build-manpages')
  prog_xslt = find_program('xsltproc')
  mans = []

  foreach s : man_targets
    extra_out = []
    foreach e : s[2]
      extra_out += '@0@_@1@.@2@'.format(s[1], e, s[0])
    endforeach
    foreach e : s[3]
      extra_out += '@0@.@1@'.format(e, s[0])
    endforeach

    name = '@0@.@1@'.format(s[1], s[0])

    mans += custom_target(
      name,
      input : ['manpage.xsl', name + '.xml'],
      output : [name, extra_out],
      command : [prog_xslt, '-nonet', '--xinclude', '-o', '@OUTPUT0@', '@INPUT@'],
      depend_files : files_man_common,
    )
  endforeach
  
  install_man(mans)
endif

if get_option('build-htmldocs')
  prog_xslt = find_program('xsltproc')

  foreach s : man_targets
    extra_out = []
    foreach e : s[2]
      extra_out += '@0@_@1@.@2@'.format(s[1], e, s[0])
    endforeach
    foreach e : s[3]
      extra_out += '@0@.@1@'.format(e, s[0])
    endforeach

    name = '@0@.@1@'.format(s[1], s[0])

    custom_target(
      name + '.html',
      input : ['html.xsl', name + '.xml'],
      output : [name + '.html'],
      command : [prog_xslt, '-nonet', '--xinclude', '-o', '@OUTPUT0@', '@INPUT@'],
      install : true,
      install_dir : join_paths(docdir, 'html', 'man'),
      depend_files : files_man_common,
    )
  endforeach
endif
