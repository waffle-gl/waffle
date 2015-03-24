// Copyright 2014 Intel Corporation
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

#include <dlfcn.h>

#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/c/pp_errors.h"
#include "nacl_container.h"
#include "nacl_swap_thread.h"

namespace waffle {

struct nacl_container {
    pp::Graphics3D *ctx;
    NaclSwapThread *swapper;

    void *glapi;
    bool (*glInitializePPAPI) (PPB_GetInterface);
    void (*glSetCurrentContextPPAPI) (PP_Resource);
    bool (*glTerminatePPAPI) (void);
};

static void
nacl_container_dtor(waffle::nacl_container *nc)
{
    if (!nc)
        return;

    if (nc->glapi)
        dlclose(nc->glapi);

    delete nc->swapper;
    delete nc;
}

static nacl_container*
nacl_container_ctor()
{
    nacl_container *nc = new nacl_container;

    if (!nc)
        return NULL;

    return nc;
}

static bool
nacl_container_context_init(waffle::nacl_container *nc, struct nacl_config *cfg)
{
    // There is no way currently to pass a pp::Instance for Waffle, so
    // we fetch a map of all instances and if there's only one we select
    // that one, otherwise we fail.
    const pp::Module::InstanceMap instances =
        pp::Module::Get()->current_instances();

    if (instances.size() != 1) {
        wcore_errorf(WAFFLE_ERROR_FATAL,
                    "Could not find a pp::Instance for Waffle to use.");
        return false;
    }

    pp::Instance *pp_instance = instances.begin()->second;
    nc->ctx = new pp::Graphics3D(pp_instance, pp::Graphics3D(), cfg->attribs);

    if (nc->ctx->is_null()) {
        wcore_errorf(WAFFLE_ERROR_FATAL, "Unable to create NaCl 3D context.");
        return false;
    }

    // We need to fetch NaCl specific init, makecurrent and terminate
    // functions that communicate with the browser interface. As nacl_config
    // currently supports only ES2, this is hardcoded for ES2.
    nc->glapi = dlopen(NACL_GLES2_LIBRARY, RTLD_LAZY);
    if (!nc->glapi) {
        wcore_errorf(WAFFLE_ERROR_FATAL, "dlopen failed: %s", dlerror());
        return false;
    }

#define RESOLVE(func) \
    nc->func = (typeof(nc->func)) dlsym(nc->glapi, (#func)); \
    if (!nc->func) { \
        wcore_errorf(WAFFLE_ERROR_FATAL, "failed to resolve %s", #func); \
        return false; \
    }

    RESOLVE(glInitializePPAPI);
    RESOLVE(glSetCurrentContextPPAPI);
    RESOLVE(glTerminatePPAPI);

#undef RESOLVE

    if (!nc->glInitializePPAPI(pp::Module::Get()->get_browser_interface())) {
        wcore_errorf(WAFFLE_ERROR_FATAL,
                    "Unable to initialize GL PPAPI!");
        return false;
    }

    if (!pp_instance->BindGraphics(*nc->ctx)) {
        wcore_errorf(WAFFLE_ERROR_FATAL, "Unable to bind NaCl 3D context.");
        return false;
    }

    nc->swapper = new NaclSwapThread(pp_instance, nc->ctx);
    return true;
}

}; // namespace waffle ends

extern "C" struct nacl_container*
nacl_container_init()
{
    return reinterpret_cast<nacl_container*>(waffle::nacl_container_ctor());
}

extern "C" void
nacl_container_teardown(nacl_container *nc)
{
    waffle::nacl_container_dtor(reinterpret_cast<waffle::nacl_container*>(nc));
}

extern "C" bool
nacl_container_context_init(struct nacl_container *nc, struct nacl_config *cfg)
{
    return waffle::nacl_container_context_init(
                   reinterpret_cast<waffle::nacl_container*>(nc), cfg);
}

extern "C" void
nacl_container_context_fini(struct nacl_container *nc)
{
    waffle::nacl_container *cpp_nc =
        reinterpret_cast<waffle::nacl_container*>(nc);

    delete cpp_nc->ctx;
    cpp_nc->ctx = NULL;

    cpp_nc->glSetCurrentContextPPAPI(0);
    cpp_nc->glTerminatePPAPI();
}

extern "C" bool
nacl_container_window_resize(struct nacl_container *nc, int32_t width, int32_t height)
{
    waffle::nacl_container *cpp_nc =
        reinterpret_cast<waffle::nacl_container*>(nc);

    int32_t error = cpp_nc->ctx->ResizeBuffers(width, height);

    if (error == PP_OK)
       return true;

    switch (error) {
        case PP_ERROR_BADRESOURCE:
            wcore_errorf(WAFFLE_ERROR_FATAL, "Invalid NaCl 3D context.");
            break;
        case PP_ERROR_BADARGUMENT:
            wcore_errorf(WAFFLE_ERROR_BAD_PARAMETER,
                         "Invalid values given for resize (w %d h %d)",
                         width, height);
            break;
        default:
            wcore_errorf(WAFFLE_ERROR_UNKNOWN, "NaCl resize failed.");
    }
    return false;
}

extern "C" bool
nacl_container_context_makecurrent(nacl_container *nc, bool release)
{
    waffle::nacl_container *cpp_nc =
        reinterpret_cast<waffle::nacl_container*>(nc);

    PP_Resource ctx = release ? 0 : cpp_nc->ctx->pp_resource();

    cpp_nc->glSetCurrentContextPPAPI(ctx);

    return true;
}

extern "C" bool
nacl_container_swapbuffere(nacl_container *nc)
{
    waffle::nacl_container *cpp_nc =
        reinterpret_cast<waffle::nacl_container*>(nc);
    return cpp_nc->swapper->swap();
}
