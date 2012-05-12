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

/// @addtogroup linux_platform
/// @{

/// @file

#include "linux_platform.h"

#include <stdlib.h>

#include <waffle/waffle_enum.h>
#include <waffle/core/wcore_error.h>

#include "linux_dl.h"

struct linux_platform {
    struct linux_dl *libgl;
    struct linux_dl *libgles1;
    struct linux_dl *libgles2;
};

struct linux_platform*
linux_platform_create(void)
{
    struct linux_platform *self = calloc(1, sizeof(*self));
    if (!self) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    return self;
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

/// @}
