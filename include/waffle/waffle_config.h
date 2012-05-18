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

/// @defgroup waffle_config waffle_config
/// @ingroup waffle_api
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "waffle_portable.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_config;
struct waffle_display;

/// @brief Choose a config satisfying some attributes.
///
///
/// ### Example Attribute Lists ###
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
/// const int32_t empty_list[] = {0};
///
/// const int32_t opengl32_rgb565[] = {
///     WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL,
///     WAFFLE_CONTEXT_MAJOR_VERSION,   3,
///     WAFFLE_CONTEXT_MINOR_VERSION,   2,
///     WAFFLE_CONTEXT_PROFILE,         WAFFLE_CONTEXT_CORE_PROFILE,
///
///     WAFFLE_RED_SIZE,    5,
///     WAFFLE_GREEN_SIZE,  6,
///     WAFFLE_BLUE_SIZE,   5,
///     0,
/// };
///
/// const int32_t opengles2_z24s8[] = {
///     WAFFLE_CONTEXT_API,      WAFFLE_CONTEXT_OPENGL_ES2,
///
///     WAFFLE_DEPTH_SIZE,      24,
///     WAFFLE_STENCIL_SIZE,     8,
///     0,
/// };
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///
///
/// ### Attributes ###
///
/// Argument @a attrib_list specifies a list of attributes, described in the
/// table below, that the returned config must satisfy. The list consists of
/// a zero-terminated sequence of name/value pairs. If an attribute is absent
/// from the list, then the attribute assumes its default value. If @a
/// attrib_list is null or empty, then all attributes assume their default
/// values.
///
/// Individual attributes are discussed below.
///
/// The context attributes (`WAFFLE_CONTEXT_*`) have quirks that are specific
/// to the native platform. Waffle attempts to accomdate those quirks in
/// a platform-neutral way as much as possible, but not all quirks can be
/// eliminated through a platform abstraction layer. The quirks are documented
/// below in detail.  For additional documentation on the behavior of the
/// contxt attributes on each platform, refer to the following: for GLX, refer
/// to the [GLX_ARB_create_context_profile] and
/// [GLX_EXT_create_context_es2_profile] extensions; for EGL, refer to the
/// [EGL 1.4 specification]; for CGL, refer to the documentation of
/// `kCGLPFAOpenGLProfile` in Apple's [CGL Reference].
///
/// For example, one quirk that Waffle is able to accomodate is that some
/// platforms require specification of context attributes at different times.
/// GLX requires that the context attributes be specified at time of context
/// creation (`glXCreateContextAttribsARB`), but MacOS requires the attributes
/// to be specified when choosing a config (`CGLChoosePixelFormat`).
/// Therefore, Waffle is constrained by MacOS to require the attributes at
/// `waffle_config_choose`.
///
/// [GLX_ARB_create_context_profile]: http://www.opengl.org/registry/specs/ARB/glx_create_context.txt
/// [GLX_EXT_create_context_es2_profile]: http://www.opengl.org/registry/specs/EXT/glx_create_context_es2_profile.txt
/// [EGL 1.4 specification]: http://www.khronos.org/registry/egl/specs/eglspec.1.4.20110406.pdf
/// [CGL Reference]: https://developer.apple.com/library/mac/#documentation/graphicsimaging/reference/CGL_OpenGL/Reference/reference.html
///
///
/// ##### `WAFFLE_CONTEXT_API` #####
///
/// The set of supported values for the `WAFFLE_CONTEXT_API` attribute depends
/// on the native system.  To check if the system supports a given API, call
/// waffle_display_supports_context_api().  Invariants and expectations for
/// each platform are discussed below.
///
/// On Android, `WAFFLE_CONTEXT_OPENGL_ES1` is always supported.  If the
/// Android version is at least 4.0, then `WAFFLE_CONTEXT_OPENGL_ES2` is
/// supported.  Systems with earlier versions of Android may support
/// `WAFFLE_CONTEXT_OPENGL_ES2`. No other API is supported.
///
/// On GLX, `WAFFLE_CONTEXT_OPENGL` is always supported. The system may
/// support other APIs.
///
/// On X11/EGL and Wayland, no API is guaranteed to be supported. One must
/// call waffle_display_supports_context_api() to check for supported APIs.
///
/// On MacOS, only `WAFFLE_CONTEXT_OPENGL` is supported. This may change in
/// the future if Apple adds support for additional APIs.
///
/// ##### `WAFFLE_CONTEXT_MAJOR_VERSION` and `WAFFLE_CONTEXT_MINOR_VERSION` #####
///
/// The default and supported values of the requested context version depend
/// on the native system and the value of `WAFFLE_CONTEXT_API`.
///
/// If the requested API is `WAFFLE_CONTEXT_OPENGL_ES1` then the default and
/// only supported version is 1.0.  The system may promote the version to 1.1.
///
/// If the requested API is `WAFFLE_CONTEXT_OPENGL_ES2`, then the default and
/// only supported version is 2.0.
///
/// For all platforms, if the requested API is `WAFFLE_CONTEXT_OPENGL` then
/// the default version is 1.0.  If the requested version is less than 3.0
/// then the native system may promote the requested version to any greater
/// version that is legacy-compatible.
///
/// If the requested API is `WAFFLE_CONTEXT_OPENGL` and the native platform is
/// MacOS, then the requested version must be 1.0 or 3.2.
///
/// If the requested API is `WAFFLE_CONTEXT_OPENGL` and the native platform is
/// GLX, then the version is unchecked by Waffle and passed directly to GLX at
/// time of context creation.
///
/// If the requested API is `WAFFLE_CONTEXT_OPENGL` and the native platform is
/// an EGL platform (that is, one of Android, X11/EGL, or Wayland), then the
/// version must be 1.0. (This restriction may be removed if, in the future,
/// EGL allows selection of the context version).
///
/// ##### `WAFFLE_CONTEXT_PROFILE` #####
///
/// The default and supported values for the `WAFFLE_CONTEXT_PROFILE`
/// attribute depend on the native system and the requested value of
/// `WAFFLE_CONTEXT_API`.
///
/// If the requested API is `WAFFLE_CONTEXT_OPENGL_ES1` or
/// `WAFFLE_CONTEXT_OPENGL_ES2`, then the default and only supported value is
/// `WAFFLE_NONE`.
///
/// If the requested API is `WAFFLE_CONTEXT_OPENGL`. the the default value is
/// `WAFFLE_CONTEXT_CORE_PROFILE`. If the requested version is less than 3.2,
/// then `WAFFLE_CONTEXT_PROFILE` is ignored.
///
///
/// | Name                                    | Type | Default          | Choices                                       |
/// |:----------------------------------------|-----:|-----------------:|:----------------------------------------------|
/// | WAFFLE_CONTEXT_API                      | enum |         required | (see discussion)                              |
/// | .                                       |    . |                . | WAFFLE_CONTEXT_OPENGL                         |
/// | .                                       |    . |                . | WAFFLE_CONTEXT_OPENGL_ES1                     |
/// | .                                       |    . |                . | WAFFLE_CONTEXT_OPENGL_ES2                     |
/// | .                                       |    . |                . | .                                             |
/// | .                                       |    . |                . | .                                             |
/// | WAFFLE_CONTEXT_MAJOR_VERSION            |  int | (see discussion) | .                                             |
/// | WAFFLE_CONTEXT_MINOR_VERSION            |  int | (see discussion) | .                                             |
/// | .                                       |    . |                . | .                                             |
/// | WAFFLE_CONTEXT_PROFILE                  | enum | (see discussion) | .                                             |
/// | .                                       |    . |                . | WAFFLE_NONE                                   |
/// | .                                       |    . |                . | WAFFLE_CONTEXT_CORE_PROFILE                   |
/// | .                                       |    . |                . | WAFFLE_CONTEXT_COMPATIBILITY_PROFILE          |
/// | .                                       |    . |                . | .                                             |
/// | WAFFLE_RED_SIZE                         |  int | WAFFLE_DONT_CARE | .                                             |
/// | WAFFLE_GREEN_SIZE                       |  int | WAFFLE_DONT_CARE | .                                             |
/// | WAFFLE_BLUE_SIZE                        |  int | WAFFLE_DONT_CARE | .                                             |
/// | WAFFLE_ALPHA_SIZE                       |  int | WAFFLE_DONT_CARE | .                                             |
/// | .                                       |    . |                . | .                                             |
/// | WAFFLE_DEPTH_SIZE                       |  int | WAFFLE_DONT_CARE | .                                             |
/// | WAFFLE_STENCIL_SIZE                     |  int | WAFFLE_DONT_CARE | .                                             |
/// | .                                       |    . |                . | .                                             |
/// | WAFFLE_SAMPLE_BUFFERS                   | bool |            false | .                                             |
/// | WAFFLE_SAMPLES                          |  int |                0 | .                                             |
/// | .                                       |    . |                . | .                                             |
/// | WAFFLE_DOUBLE_BUFFERED                  | bool |             true | .                                             |
///
WAFFLE_API struct waffle_config*
waffle_config_choose(struct waffle_display *dpy, const int32_t attrib_list[]);

WAFFLE_API bool
waffle_config_destroy(struct waffle_config *self);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
