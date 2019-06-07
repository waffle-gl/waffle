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

# because of the way meson works and the use of preprocessor defines to guard
# code puting all of the depends in one giant array is fine. If they weren't
# found meson is fine with that, otherwise they'll be unnecissary and cause no
# harm.
deps_for_waffle = [
  dep_gl,
  dep_egl,
  dep_gbm,
  dep_udev,
  dep_nacl,
  dep_wayland_client,
  dep_wayland_egl,
  dep_x11_xcb,
  dep_dl,
  dep_cocoa,
  dep_core_foundation,
  dep_gl_headers,
  idep_threads,
]

files_libwaffle = files(
  'api/api_priv.c',
  'api/waffle_attrib_list.c',
  'api/waffle_config.c',
  'api/waffle_context.c',
  'api/waffle_display.c',
  'api/waffle_dl.c',
  'api/waffle_enum.c',
  'api/waffle_error.c',
  'api/waffle_gl_misc.c',
  'api/waffle_init.c',
  'api/waffle_window.c',
  'core/wcore_attrib_list.c',
  'core/wcore_config_attrs.c',
  'core/wcore_display.c',
  'core/wcore_error.c',
  'core/wcore_tinfo.c',
  'core/wcore_util.c',
)

if not ['darwin', 'windows'].contains(host_machine.system())
  files_libwaffle += files('linux/linux_dl.c', 'linux/linux_platform.c')
endif

if build_x11_egl or build_wayland or build_gbm or build_surfaceless
  files_libwaffle += files(
    'egl/wegl_config.c',
    'egl/wegl_context.c',
    'egl/wegl_display.c',
    'egl/wegl_platform.c',
    'egl/wegl_util.c',
    'egl/wegl_surface.c',
  )
endif

if build_surfaceless
  files_libwaffle += files(
    'surfaceless_egl/sl_display.c',
    'surfaceless_egl/sl_platform.c',
    'surfaceless_egl/sl_window.c',
  )
endif

if build_wayland
  files_libwaffle += files(
    'wayland/wayland_display.c',
    'wayland/wayland_platform.c',
    'wayland/wayland_window.c',
    'wayland/wayland_wrapper.c',
  )
endif

if build_gbm
  files_libwaffle += files(
    'gbm/wgbm_config.c',
    'gbm/wgbm_display.c',
    'gbm/wgbm_platform.c',
    'gbm/wgbm_window.c',
  )
endif

if build_x11_egl
  files_libwaffle += files(
    'xegl/xegl_display.c',
    'xegl/xegl_platform.c',
    'xegl/xegl_window.c',
  )
endif

if build_glx or build_x11_egl
  files_libwaffle += files(
    'x11/x11_display.c',
    'x11/x11_window.c',
  )
endif

if build_glx
  files_libwaffle += files(
    'glx/glx_config.c',
    'glx/glx_context.c',
    'glx/glx_display.c',
    'glx/glx_platform.c',
    'glx/glx_window.c',
  )
endif

if build_nacl
  files_libwaffle += files(
    'nacl/nacl_config.c',
    'nacl/nacl_context.c',
    'nacl/nacl_display.c',
    'nacl/nacl_dl.c',
    'nacl/nacl_platform.c',
    'nacl/nacl_window.c',
    'nacl/nacl_container.cpp',
  )
endif

if build_wgl
  files_libwaffle += files(
    'wgl/wgl_config.c',
    'wgl/wgl_context.c',
    'wgl/wgl_display.c',
    'wgl/wgl_dl.c',
    'wgl/wgl_error.c',
    'wgl/wgl_platform.c',
    'wgl/wgl_window.c',
  )
endif

if build_cgl
  files_libwaffle += files(
    'cgl/cgl_config.m',
    'cgl/cgl_context.m',
    'cgl/cgl_display.m',
    'cgl/cgl_dl.m',
    'cgl/cgl_error.m',
    'cgl/cgl_platform.m',
    'cgl/cgl_window.m',
    'cgl/WaffleGLView.m',
  )
endif

include_libwaffle = include_directories(
  'android', 'api', 'cgl', 'core', 'egl', 'glx', 'linux', 'nacl',
  'surfaceless_egl', 'wayland', 'wgl', 'x11', 'xegl'
)

waffle_data = configuration_data()
waffle_data.set('waffle_libname', waffle_name)

waffle_defs = configure_file(
  configuration : waffle_data,
  input : 'waffle.def.in',
  output : '@BASENAME@',
)

_ver = meson.project_version().split('.')
_ver = '0.@0@.@1@'.format(_ver[1], _ver[2])

libwaffle = library(
  waffle_name,
  [files_libwaffle, waffle_config_h],
  include_directories : [include_libwaffle, inc_waffle, inc_include],
  c_args : api_c_args,
  cpp_args : api_c_args,
  dependencies : deps_for_waffle,
  soversion : host_machine.system() != 'windows' ? '0' : '',
  version : _ver,
  install : true,
  vs_module_defs : waffle_defs,
  gnu_symbol_visibility : 'hidden',
)

ext_waffle = declare_dependency(
  link_with : libwaffle,
  include_directories : [inc_waffle, inc_include],
)

pkg = import('pkgconfig')
pkg.generate(
  libwaffle,
  version : meson.project_version(),
  subdirs : waffle_name,
  description : 'A library for selecting an OpenGL API and windows system at runtime.',
)

if meson.version().version_compare('>= 0.50')
  cmake = import('cmake')
  cmake.write_basic_package_version_file(
    name : 'Waffle',
    version : meson.project_version(),
    compatibility : 'SameMajorVersion',
  )

  cmake_conf = configuration_data()
  cmake_conf.set('waffle_libname', waffle_name)
  cmake_conf.set('PACKAGE_CMAKE_INSTALL_INCLUDEDIR', join_paths('${PACKAGE_PREFIX_DIR}', get_option('includedir')))
  cmake_conf.set('PACKAGE_CMAKE_INSTALL_LIBDIR', join_paths('${PACKAGE_PREFIX_DIR}', get_option('libdir')))

  cmake.configure_package_config_file(
    name : 'Waffle',
    input : '../../cmake/Modules/WaffleConfig.cmake.in',
    configuration : cmake_conf,
  )
endif

if get_option('build-tests')
  if get_option('default_library') == 'shared'
    # The unit tests need to poke at internals of the library, but with a
    # shared library and visibility=hidden it can't. If we have a shared
    # library then we build a static library to link the unit tests against
    testwaffle = static_library(
      'testwaffle',
      [files_libwaffle, waffle_config_h],
      include_directories : [include_libwaffle, inc_waffle, inc_include],
      c_args : api_c_args,
      cpp_args : api_c_args,
      dependencies : deps_for_waffle,
    )
  else
    testwaffle = libwaffle
  endif

  foreach t : ['wcore_attrib_list', 'wcore_config_attrs', 'wcore_error']
    test(
      t,
      executable(
        '@0@_unittest'.format(t),
        'core/@0@_unittest.c'.format(t),
        dependencies : [dep_cmocka, idep_threads],
        include_directories : [inc_waffle, inc_include],
        link_with : testwaffle,
      ),
    )
  endforeach
endif
