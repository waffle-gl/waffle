# Copyright © 2017 Dylan Baker
# Copyright © 2018-2019 Intel Corporation
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

project(
  'waffle',
  ['c'],
  version : '1.7.90',
  license : 'BSD2', 
  meson_version : '>= 0.49',
  # ubproject cmocka throws a Wclobbered ... make this a level=2 once it's fixed
  default_options : ['c_std=c99', 'warning_level=1', 'buildtype=debugoptimized'],
)

waffle_name = meson.project_name() + '-1'
docdir = join_paths(get_option('datadir'), 'doc', meson.project_name() + '1')

build_x11_egl = false
build_wayland = false
build_glx = false
build_gbm = false
build_surfaceless = false
build_wgl = ['windows', 'cygwin'].contains(host_machine.system())
build_cgl = host_machine.system() == 'darwin'

if build_cgl
  add_languages('objc')
endif

cc = meson.get_compiler('c')

_dep_null = dependency('', required : false)
dep_threads = dependency('threads')
dep_dl = cc.find_library('dl', required : false)
dep_gl = _dep_null
dep_drm = _dep_null
dep_egl = _dep_null
dep_wayland_client = _dep_null
dep_wayland_egl = _dep_null
dep_wayland_scanner = _dep_null
dep_wayland_proto = _dep_null
dep_x11_xcb = _dep_null
dep_gbm = _dep_null
dep_cocoa = _dep_null
dep_core_foundation = _dep_null
dep_gl_headers = _dep_null

# Get dependencies
if build_wgl
  if not cc.has_header('GL/wglext.h')
    _sub = subproject('opengl_headers')
    dep_gl_headers = _sub.get_variable('ext_headers')
  endif
  dep_gl = cc.find_library('opengl32')
elif build_cgl
  dep_cocoa = dependency('cocoa')
  dep_core_foundation = dependency('corefoundation')
  dep_gl = dependency('gl')
else
  dep_gl = dependency('gl', required : get_option('glx'))
  dep_x11_xcb = dependency('x11-xcb', required : get_option('glx'))
  build_glx = dep_gl.found() and dep_x11_xcb.found()

  dep_egl = dependency('egl', required : get_option('x11_egl'))
  dep_x11_xcb = dependency('x11-xcb', required : get_option('x11_egl'))
  build_x11_egl = dep_egl.found() and dep_x11_xcb.found()

  dep_drm = dependency('libdrm', required : get_option('gbm'))
  dep_egl = dependency('egl', required : get_option('gbm'))
  dep_gbm = dependency('gbm', required : get_option('gbm'))
  build_gbm = dep_drm.found() and dep_egl.found() and dep_gbm.found()

  dep_egl = dependency('egl', required : get_option('surfaceless_egl'))
  build_surfaceless = dep_egl.found()

  dep_egl = dependency('egl', required : get_option('wayland'))
  dep_wayland_client = dependency(
    'wayland-client', version : '>= 1.10', required : get_option('wayland'),
  )
  if dep_wayland_client.found()
    wayland_core_xml = join_paths(dep_wayland_client.get_pkgconfig_variable('pkgdatadir'),
    'wayland.xml')
  endif
  dep_wayland_egl = dependency(
    'wayland-egl', version : '>= 9.1', required : get_option('wayland'),
  )
  dep_wayland_scanner = dependency(
    'wayland-scanner', version : '>= 1.15', required : get_option('wayland'),
  )
  if dep_wayland_scanner.found()
    prog_wayland_scanner = find_program(dep_wayland_scanner.get_pkgconfig_variable('wayland_scanner'))
  endif
  dep_wayland_proto = dependency(
    'wayland-protocols', version  : '>= 1.12', required : get_option('wayland'),
  )
  if dep_wayland_proto.found()
    wayland_xdg_shell_xml = join_paths(dep_wayland_proto.get_pkgconfig_variable('pkgdatadir'),
    'stable/xdg-shell/xdg-shell.xml')
  endif
  build_wayland = dep_egl.found() and dep_wayland_client.found() and dep_wayland_egl.found() and dep_wayland_scanner.found() and dep_wayland_proto.found()

  # We're interested only in the headers provided
  # FINISHME: make x11_xcb compile-only dependency
  _include_deps = [ dep_gl, dep_drm, dep_egl, dep_gbm, dep_wayland_client, dep_wayland_egl ]
  foreach d : _include_deps
    if d.found()
      d = d.partial_dependency(compile_args : true)
    endif
  endforeach
endif

dep_bash = dependency('bash-completion', required : false)
if dep_bash.found()
  bashcompletiondir = dep_bash.get_pkgconfig_variable('completionsdir')
else
  bashcompletiondir = join_paths(get_option('datadir'), 'bash-completion/completions')
endif
install_data('shell-completion/bash/wflinfo', install_dir : bashcompletiondir)

zshcompletiondir = join_paths(get_option('datadir'), 'zsh/site-functions')
install_data('shell-completion/zsh/_wflinfo', install_dir : zshcompletiondir)

# Set pre-processor flags
if build_wgl
    add_project_arguments('-DWAFFLE_HAS_WGL', language : ['c', 'cpp'])
elif build_cgl
    add_project_arguments('-DWAFFLE_HAS_CGL -DGL_SILENCE_DEPRECATION', language : ['c', 'obj'])
else
  if build_x11_egl
    add_project_arguments('-DWAFFLE_HAS_X11_EGL', language : ['c', 'cpp'])
  endif
  if build_glx
    add_project_arguments('-DWAFFLE_HAS_GLX', language : ['c', 'cpp'])
  endif
  if build_wayland
    add_project_arguments('-DWAFFLE_HAS_WAYLAND', language : ['c', 'cpp'])
  endif
  if build_gbm
    add_project_arguments('-DWAFFLE_HAS_GBM', language : ['c', 'cpp'])
  endif
  if build_surfaceless
    add_project_arguments('-DWAFFLE_HAS_SURFACELESS_EGL', language : ['c', 'cpp'])
  endif
endif

if cc.get_argument_syntax() == 'gcc'
  if cc.compiles('''
      static __thread int x;
      int main() {
        x = 42;
        return x;
      }''',
      args : '-Werror',
      name : 'Thread Local Storage')
    add_project_arguments('-DWAFFLE_HAS_TLS', language : ['c', 'cpp'])

    if cc.compiles('''
        static __thread int x __attribute__((tls_model("initial-exec")));
        int main() {
          x = 42;
          return x;
        }''',
        args : '-Werror',
        name : 'TLS model "Initial Exec"')
      add_project_arguments('-DWAFFLE_HAS_TLS_MODEL_INITIAL_EXEC', language : ['c', 'cpp'])
    endif
  endif

  add_project_arguments('-D_XOPEN_SOURCE=700', language : ['c', 'cpp'])

  add_project_arguments(
    cc.get_supported_arguments([
      '-Wno-unused-parameter',
      '-Werror=format',
      '-Werror=format-security',
      '-Werror=incompatible-pointer-types',
      '-Werror=init-self',
      '-Werror=int-conversion',
      '-Werror=missing-declarations',
      '-Werror=missing-prototypes',  # TODO: breaks on MacOS?
      '-Werror=pointer-arith',
      '-Werror=undef',
      '-Werror=vla',
      '-Wsuggest-attribute=format',
      '-Wwrite-strings',
    ]),
    language : ['c', 'cpp'],
  )

  if host_machine.system() == 'windows'
    if cc.has_argument('-static-libgcc')
      add_project_link_arguments('-static-libgcc', language : ['c', 'cpp'])
    endif
  endif
  api_c_args = [
    '-DWAFFLE_API_VERSION=@0@'.format('0x0108'),
    '-DWAFFLE_API_EXPERIMENTAL',
  ]
elif cc.get_argument_syntax() == 'msvc'
  add_project_arguments(
    '/D_win32_WINNT=0x0601',
    '/DWINVER=0x0601',
    '/D_CRT_NONSTDC_NO_WARNINGS',
    '/D_CRT_SECURE_NO_WARNINGS',
    language : ['c'],
  )
  api_c_args = [
    '/DWAFFLE_API_VERSION=@0@'.format('0x0108'),
    '/DWAFFLE_API_EXPERIMENTAL',
  ]
endif

install_data('README.md', 'LICENSE.txt', 'HACKING.txt', install_dir : docdir)
install_subdir('doc/release-notes', install_dir : docdir)


if get_option('build-tests')
  dep_cmocka = dependency('cmocka', fallback : ['cmocka', 'dep_cmocka'])
else
  dep_cmocka = _dep_null
endif

subdir('include')
subdir('third_party')
subdir('src')
subdir('doc')
subdir('man')
if get_option('build-tests')
  subdir('tests')
endif
if get_option('build-examples')
  subdir('examples')
endif
