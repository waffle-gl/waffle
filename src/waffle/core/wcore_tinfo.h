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

struct wcore_error_tinfo;
struct wcore_context;
struct wcore_display;
struct wcore_window;

/// @brief Thread-local info for all of Waffle.
///
/// A note on the current display and context:
///
///   EGL allows a thread to have current one the below combinations of display,
///   context, and surface.
///
///     a. No display, no context, no surface.
///
///        This case occurs when the thread has not previously called
///        eglMakeCurrent(), or when the thread has calls
///        eglMakeCurrent(EGL_DISPLAY_NONE, EGL_NO_SURFACE, EGL_NO_SURFACE,
///        EGL_NO_CONTEXT). Note that the EGL 1.5 specification mandates that
///        calling eglMakeCurrent() with EGL_DISPLAY_NONE generate an error,
///        but some implementations allow it anyway according to the footnote
///        in the EGL 1.5 specification (2014.08.27), p59:
///
///          Some implementations have chosen to allow EGL_NO_DISPLAY as
///          a valid dpy parameter for eglMakeCurrent. This behavior is not
///          portable to all EGL implementations, and should be considered as
///          an undocumented vendor extension.
///
///     b. One display, no context, no surface.
///
///        This case occurs when the thread calls eglMakeCurrent() with
///        a valid display but no context.
///
///     c. One display, one context, no surface.
///
///        Supported by EGL 1.5, EGL_KHR_surfaceless_context, and
///        EGL_KHR_create_context.
///
///     d. One display; one context, and both a draw and read surface.
///
///        The classic case.
///
///    EGL permits a context to be current to at most one thread. Same for
///    surfaces.  However, a display may be current to multiple threads.
///
///    Therefore, to support waffle_get_current(), Waffle must track all three
///    objects (display, context, and waffle_window) in thread-specific storage.
///    It is insufficient to maintain a reference to the current context and
///    window in the current display, as there may exist multiple contexts and
///    surfaces, each current in different threads, but all children to the same
///    display.
///
struct wcore_tinfo {
    /// @brief Info for @ref wcore_error.
    struct wcore_error_tinfo *error;

    struct wcore_display *current_display;
    struct wcore_window *current_window;
    struct wcore_context *current_context;

    bool is_init;
};

/// @brief Get the thread-local info for the current thread.
struct wcore_tinfo* wcore_tinfo_get(void);
