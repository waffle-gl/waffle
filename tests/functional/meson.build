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

# Recent versions of meson (like we require) support -Wno-foo correctly
no_override_args = cc.first_supported_argument([
  '-Wno-initializer-overrides',
  '-Wno-override-init',
])

files_gl_basic_test = files('gl_basic_test.c')
if build_cgl
  files_gl_basic_test += files('gl_basic_cocoa.m')
endif

gl_basic_test = executable(
  'gl_basic_test',
  [files_gl_basic_test, waffle_config_h],
  c_args : [api_c_args, no_override_args],
  dependencies : [dep_cmocka, dep_cocoa, idep_getopt, ext_waffle],
  include_directories : inc_include,
)

if build_cgl
  test('gl_basic (cgl)', gl_basic_test, args : ['--platform', 'cgl'])
elif ['dragonfly', 'openbsd', 'netbsd', 'freebsd', 'linux'].contains(host_machine.system())
  if build_glx
    test(
      'gl_basic (glx)',
      gl_basic_test,
      args : ['--platform', 'glx'],
      suite : ['functional']
    )
  endif
  if build_wayland
    test(
      'gl_basic (wayland)',
      gl_basic_test,
      args : ['--platform', 'wayland'],
      suite : ['functional'],
    )
  endif
  if build_x11_egl
    test(
      'gl_basic (x11_egl)',
      gl_basic_test,
      args : ['--platform', 'x11_egl'],
      suite : ['functional'],
    )
  endif
  if build_gbm
    test(
      'gl_basic (gbm)',
      gl_basic_test,
      args : ['--platform', 'gbm'],
      suite : ['functional'],
    )
  endif
elif host_machine.system() == 'windows' # cygwin?
  test(
    'gl_basic (wgl)',
    gl_basic_test,
    args : ['--platform', 'wgl'],
    suite : ['functional'],
  )
endif
