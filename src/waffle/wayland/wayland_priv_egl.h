// Copyright 2012 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

// WL_EGL_PLATFORM configures Mesa's <EGL/egl.h> to define native types (such
// as EGLNativeDisplay) as Wayland types rather than Xlib types.
#define WL_EGL_PLATFORM 1

#define NATIVE_EGL(basename) wayland_egl_##basename
#include <waffle/egl/egl.h>
