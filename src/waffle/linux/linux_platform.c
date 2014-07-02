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

#include <stdlib.h>

#include "wcore_error.h"
#include "wcore_util.h"

#include "linux_dl.h"
#include "linux_platform.h"

struct linux_platform {
    struct linux_dl *libgl;
    struct linux_dl *libgles1;
    struct linux_dl *libgles2;
};

struct linux_platform*
linux_platform_create(void)
{
    return wcore_calloc(sizeof(struct linux_platform));
}

bool
linux_platform_destroy(struct linux_platform *self)
{
    bool ok = true;

    if (!self)
        return true;

    // FIXME: Waffle is unable to emit a sequence of errors.
    ok &= linux_dl_close(self->libgl);
    ok &= linux_dl_close(self->libgles1);
    ok &= linux_dl_close(self->libgles2);

    free(self);
    return ok;
}

static struct linux_dl*
linux_platform_get_dl(
        struct linux_platform *self,
        int32_t waffle_dl)
{
    struct linux_dl **dl;

    switch (waffle_dl) {
        case WAFFLE_DL_OPENGL:     dl = &self->libgl;    break;
        case WAFFLE_DL_OPENGL_ES1: dl = &self->libgles1; break;
        case WAFFLE_DL_OPENGL_ES2: dl = &self->libgles2; break;
        case WAFFLE_DL_OPENGL_ES3: dl = &self->libgles2; break;
        default:
            wcore_error_internal("waffle_dl has bad value %#x", waffle_dl);
            return NULL;
    }

    if (*dl == NULL)
        *dl = linux_dl_open(waffle_dl);

    return *dl;
}

bool
linux_platform_dl_can_open(
        struct linux_platform *self,
        int32_t waffle_dl)
{
    struct linux_dl *dl = NULL;
    WCORE_ERROR_DISABLED({
        dl = linux_platform_get_dl(self, waffle_dl);
    });
    return dl != NULL;
}

void*
linux_platform_dl_sym(
        struct linux_platform *self,
        int32_t waffle_dl,
        const char *name)
{
    struct linux_dl *dl = linux_platform_get_dl(self, waffle_dl);
    if (!dl)
        return NULL;

    return linux_dl_sym(dl, name);
}
