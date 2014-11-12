// Copyright 2012 Intel Corporation
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "waffle_version.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_display;
struct waffle_config;
struct waffle_context;
struct waffle_window;

union waffle_native_display;
union waffle_native_config;
union waffle_native_context;
union waffle_native_window;

// ---------------------------------------------------------------------------
// waffle_error
// ---------------------------------------------------------------------------

enum waffle_error {
    WAFFLE_NO_ERROR                                     = 0x00,
    WAFFLE_ERROR_FATAL                                  = 0x01,
    WAFFLE_ERROR_UNKNOWN                                = 0x02,
    WAFFLE_ERROR_INTERNAL                               = 0x03,
    WAFFLE_ERROR_BAD_ALLOC                              = 0x04,
    WAFFLE_ERROR_NOT_INITIALIZED                        = 0x05,
    WAFFLE_ERROR_ALREADY_INITIALIZED                    = 0x06,
    WAFFLE_ERROR_BAD_ATTRIBUTE                          = 0x08,
    WAFFLE_ERROR_BAD_PARAMETER                          = 0x10,
    WAFFLE_ERROR_BAD_DISPLAY_MATCH                      = 0x11,
    WAFFLE_ERROR_UNSUPPORTED_ON_PLATFORM                = 0x12,
    WAFFLE_ERROR_BUILT_WITHOUT_SUPPORT                  = 0x13,
};

struct waffle_error_info {
    enum waffle_error code;
    const char *message;
    size_t message_length;
};

enum waffle_error
waffle_error_get_code(void);

const struct waffle_error_info*
waffle_error_get_info(void);

const char*
waffle_error_to_string(enum waffle_error e);

// ---------------------------------------------------------------------------
// waffle_enum
// ---------------------------------------------------------------------------

enum waffle_enum {
    // ------------------------------------------------------------------
    // Generic
    // ------------------------------------------------------------------

    WAFFLE_DONT_CARE                                            = -1,
    WAFFLE_NONE                                                 =  0,

    // ------------------------------------------------------------------
    // For waffle_init()
    // ------------------------------------------------------------------

    WAFFLE_PLATFORM                                             = 0x0010,
        WAFFLE_PLATFORM_ANDROID                                 = 0x0011,
        WAFFLE_PLATFORM_CGL                                     = 0x0012,
        WAFFLE_PLATFORM_GLX                                     = 0x0013,
        WAFFLE_PLATFORM_WAYLAND                                 = 0x0014,
        WAFFLE_PLATFORM_X11_EGL                                 = 0x0015,
        WAFFLE_PLATFORM_GBM                                     = 0x0016,
        WAFFLE_PLATFORM_WGL                                     = 0x0017,

    // ------------------------------------------------------------------
    // For waffle_config_choose()
    // ------------------------------------------------------------------

    WAFFLE_CONTEXT_API                                          = 0x020a,
        WAFFLE_CONTEXT_OPENGL                                   = 0x020b,
        WAFFLE_CONTEXT_OPENGL_ES1                               = 0x020c,
        WAFFLE_CONTEXT_OPENGL_ES2                               = 0x020d,
        WAFFLE_CONTEXT_OPENGL_ES3                               = 0x0214,

    WAFFLE_CONTEXT_MAJOR_VERSION                                = 0x020e,
    WAFFLE_CONTEXT_MINOR_VERSION                                = 0x020f,

    WAFFLE_CONTEXT_PROFILE                                      = 0x0210,
        WAFFLE_CONTEXT_CORE_PROFILE                             = 0x0211,
        WAFFLE_CONTEXT_COMPATIBILITY_PROFILE                    = 0x0212,

#if WAFFLE_API_VERSION >= 0x0103
    WAFFLE_CONTEXT_FORWARD_COMPATIBLE                           = 0x0215,
    WAFFLE_CONTEXT_DEBUG                                        = 0x0216,
#endif

    WAFFLE_RED_SIZE                                             = 0x0201,
    WAFFLE_GREEN_SIZE                                           = 0x0202,
    WAFFLE_BLUE_SIZE                                            = 0x0203,
    WAFFLE_ALPHA_SIZE                                           = 0x0204,

    WAFFLE_DEPTH_SIZE                                           = 0x0205,
    WAFFLE_STENCIL_SIZE                                         = 0x0206,

    WAFFLE_SAMPLE_BUFFERS                                       = 0x0207,
    WAFFLE_SAMPLES                                              = 0x0208,

    WAFFLE_DOUBLE_BUFFERED                                      = 0x0209,

    WAFFLE_ACCUM_BUFFER                                         = 0x0213,

    // ------------------------------------------------------------------
    // For waffle_dl_sym()
    // ------------------------------------------------------------------

    WAFFLE_DL_OPENGL                                            = 0x0301,
    WAFFLE_DL_OPENGL_ES1                                        = 0x0302,
    WAFFLE_DL_OPENGL_ES2                                        = 0x0303,
    WAFFLE_DL_OPENGL_ES3                                        = 0x0304,
};

const char*
waffle_enum_to_string(int32_t e);

// ---------------------------------------------------------------------------

bool
waffle_init(const int32_t *attrib_list);

bool
waffle_make_current(struct waffle_display *dpy,
                    struct waffle_window *window,
                    struct waffle_context *ctx);

void*
waffle_get_proc_address(const char *name);

bool
waffle_is_extension_in_string(const char *extension_string,
                              const char *extension_name);

// ---------------------------------------------------------------------------
// waffle_display
// ---------------------------------------------------------------------------

struct waffle_display*
waffle_display_connect(const char *name);

bool
waffle_display_disconnect(struct waffle_display *self);

bool
waffle_display_supports_context_api(struct waffle_display *self,
                                    int32_t context_api);

union waffle_native_display*
waffle_display_get_native(struct waffle_display *self);

// ---------------------------------------------------------------------------
// waffle_config
// ---------------------------------------------------------------------------

struct waffle_config*
waffle_config_choose(struct waffle_display *dpy,
                     const int32_t attrib_list[]);

bool
waffle_config_destroy(struct waffle_config *self);

union waffle_native_config*
waffle_config_get_native(struct waffle_config *self);

// ---------------------------------------------------------------------------
// waffle_context
// ---------------------------------------------------------------------------

struct waffle_context*
waffle_context_create(struct waffle_config *config,
                      struct waffle_context *shared_ctx);

bool
waffle_context_destroy(struct waffle_context *self);

union waffle_native_context*
waffle_context_get_native(struct waffle_context *self);

// ---------------------------------------------------------------------------
// waffle_window
// ---------------------------------------------------------------------------

struct waffle_window*
waffle_window_create(
        struct waffle_config *config,
        int32_t width,
        int32_t height);

bool
waffle_window_destroy(struct waffle_window *self);

bool
waffle_window_show(struct waffle_window *self);

bool
waffle_window_swap_buffers(struct waffle_window *self);

union waffle_native_window*
waffle_window_get_native(struct waffle_window *self);

#if defined(WAFFLE_API_EXPERIMENTAL) && WAFFLE_API_VERSION >= 0x0103
bool
waffle_window_resize(
        struct waffle_window *self,
        int32_t width,
        int32_t height);
#endif

// ---------------------------------------------------------------------------
// waffle_dl
// ---------------------------------------------------------------------------

bool
waffle_dl_can_open(int32_t dl);

void*
waffle_dl_sym(int32_t dl, const char *name);

// ---------------------------------------------------------------------------
// waffle_native
// ---------------------------------------------------------------------------

struct waffle_gbm_config;
struct waffle_gbm_context;
struct waffle_gbm_display;
struct waffle_gbm_window;
struct waffle_glx_config;
struct waffle_glx_context;
struct waffle_glx_display;
struct waffle_glx_window;
struct waffle_wayland_config;
struct waffle_wayland_context;
struct waffle_wayland_display;
struct waffle_wayland_window;
struct waffle_x11_egl_config;
struct waffle_x11_egl_context;
struct waffle_x11_egl_display;
struct waffle_x11_egl_window;

union waffle_native_display {
    struct waffle_gbm_display *gbm;
    struct waffle_glx_display *glx;
    struct waffle_x11_egl_display *x11_egl;
    struct waffle_wayland_display *wayland;
};

union waffle_native_config {
    struct waffle_gbm_config *gbm;
    struct waffle_glx_config *glx;
    struct waffle_x11_egl_config *x11_egl;
    struct waffle_wayland_config *wayland;
};

union waffle_native_context {
    struct waffle_gbm_context *gbm;
    struct waffle_glx_context *glx;
    struct waffle_x11_egl_context *x11_egl;
    struct waffle_wayland_context *wayland;
};

union waffle_native_window {
    struct waffle_gbm_window *gbm;
    struct waffle_glx_window *glx;
    struct waffle_x11_egl_window *x11_egl;
    struct waffle_wayland_window *wayland;
};

// ---------------------------------------------------------------------------
// waffle_attrib_list
// ---------------------------------------------------------------------------

int32_t
waffle_attrib_list_length(const int32_t attrib_list[]);

bool
waffle_attrib_list_get(
        const int32_t attrib_list[],
        int32_t key,
        int32_t *value);

bool
waffle_attrib_list_get_with_default(
        const int32_t attrib_list[],
        int32_t key,
        int32_t *value,
        int32_t default_value);

bool
waffle_attrib_list_update(
        int32_t *attrib_list,
        int32_t key,
        int32_t value);

#ifdef __cplusplus
} // end extern "C"
#endif
