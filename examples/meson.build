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

install_data(
  'Makefile.example', 'gl_basic.c', 'simple-x11-egl.c',
  install_dir : join_paths(docdir, 'examples'),
)

if build_x11_egl
  executable(
    'simple-x11-egl',
    'simple-x11-egl.c',
    dependencies : ext_waffle,
  )
endif

if build_cgl
  # We need to force wflinfo.c to be compiled as if it were objc, to do that we
  # copy it into the build directory as a .m file
  files_gl_basic = configure_file(
    input : 'gl_basic.c',
    output : 'gl_basic.m',
    copy : true,
  )
else
  files_gl_basic = files('gl_basic.c')
endif

executable(
  'gl_basic',
  files_gl_basic,
  dependencies : [ext_waffle, dep_core_foundation, dep_cocoa, idep_getopt],
)
