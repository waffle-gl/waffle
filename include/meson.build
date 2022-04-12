# Copyright (c) 2017 Dylan Baker
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

inc_include = include_directories('.')
inc_waffle = include_directories('waffle-1')

ver = meson.project_version().split('.')
data = configuration_data()
data.set('waffle_major_version', ver[0])
data.set('waffle_minor_version', ver[1])
data.set('waffle_patch_version', ver[2])

waffle_config_h = configure_file(
  configuration : data,
  input : 'waffle-1/waffle_version.h.in',
  output : 'waffle_version.h',
)

install_headers(
  'waffle-1/waffle.h',
  'waffle-1/waffle_gbm.h',
  'waffle-1/waffle_glx.h',
  'waffle-1/waffle_surfaceless_egl.h',
  'waffle-1/waffle_wayland.h',
  'waffle-1/waffle_x11_egl.h',
  waffle_config_h,
  subdir : waffle_name,
)
