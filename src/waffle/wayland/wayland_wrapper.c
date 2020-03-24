// Copyright 2015 Emil Velikov
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

/// @file
/// @brief Wrappers for Wayland-client functions
///
/// Many of the wayland functions are defined as static inline within the
/// public headers. In order to avoid the static(link-time) dependency, we
/// provide the required symbols with this file.
///
/// This is achieved by declaring wrapper functions, around which we define the
/// needed (base) wayland ones. After that we include the public header, at
/// which point the pre-processor/compiler will use our defines.
///
/// Each wrapper is initialised via dlsym to retrieve the relevant symbol from
/// the library libwayland-client.so.0


#include <stdbool.h>
#include <dlfcn.h>

#include "wcore_error.h"

#include "wayland_wrapper.h"

// dlopen handle for libwayland-client.so.0
static void *dl_wl_client;

static const char *libwl_client_filename = "libwayland-client.so.0";

#define WAFFLE_WAYLAND_INTERFACE(iface) \
    const struct wl_interface *wfl_##iface = NULL;
#define WAFFLE_WAYLAND_SYM(rc, fn, params) \
    pfn_##fn wfl_##fn = NULL;
#include "wayland_sym.h"
#undef WAFFLE_WAYLAND_SYM
#undef WAFFLE_WAYLAND_INTERFACE

bool
wayland_wrapper_teardown(void)
{
    bool ok = true;
    int error;

    if (dl_wl_client) {
        error = dlclose(dl_wl_client);
        if (error) {
            ok &= false;
            wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                         "dlclose(\"%s\") failed: %s",
                         libwl_client_filename, dlerror());
        }
    }

    return ok;
}

// On failure the caller of wayland_wrapper_init will trigger it's own
// destruction which will execute wayland_wrapper_teardown.
bool
wayland_wrapper_init(void)
{
    dl_wl_client = dlopen(libwl_client_filename, RTLD_LAZY | RTLD_LOCAL);
    if (!dl_wl_client) {
        wcore_errorf(WAFFLE_ERROR_FATAL,
                     "dlopen(\"%s\") failed: %s",
                     libwl_client_filename, dlerror());
        return false;
    }

#define WAFFLE_WAYLAND_INTERFACE(iface)                               \
    wfl_##iface = (struct wl_interface *)dlsym(dl_wl_client, #iface); \
    if (!wfl_##iface) {                                               \
        wcore_errorf(WAFFLE_ERROR_FATAL,                              \
                     "dlsym(\"%s\", \"" #iface "\") failed: %s",      \
                     libwl_client_filename, dlerror());               \
        return false;                                                 \
    }
#define WAFFLE_WAYLAND_SYM(rc, fn, params)                      \
    wfl_##fn = (pfn_##fn)dlsym(dl_wl_client, #fn);              \
    if (!wfl_##fn) {                                            \
        wcore_errorf(WAFFLE_ERROR_FATAL,                        \
                     "dlsym(\"%s\", \"" #fn "\") failed: %s",   \
                     libwl_client_filename, dlerror());         \
        return false;                                           \
    }
#include "wayland_sym.h"
#undef WAFFLE_WAYLAND_SYM
#undef WAFFLE_WAYLAND_INTERFACE

    return true;
}
