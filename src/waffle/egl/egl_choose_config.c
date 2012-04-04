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

/// @file
/// @brief Implementation of egl_choose_config().
///
/// The function is declared in egl_no_native.h, but requires so much code
/// that it is placed in its own source file.

#include "egl_no_native.h"

#include <stdlib.h>
#include <string.h>

#include <waffle/waffle_enum.h>
#include <waffle/waffle_attrib_list.h>
#include <waffle/core/wcore_error.h>

static int32_t egl_config_attrib_map[] = {
    WAFFLE_RED_SIZE,            EGL_RED_SIZE,
    WAFFLE_BLUE_SIZE,           EGL_BLUE_SIZE,
    WAFFLE_GREEN_SIZE,          EGL_GREEN_SIZE,
    WAFFLE_ALPHA_SIZE,          EGL_ALPHA_SIZE,
    WAFFLE_DEPTH_SIZE,          EGL_DEPTH_SIZE,
    WAFFLE_STENCIL_SIZE,        EGL_STENCIL_SIZE,
    WAFFLE_SAMPLE_BUFFERS,      EGL_SAMPLE_BUFFERS,
    WAFFLE_SAMPLES,             EGL_SAMPLES,
    0,
};

static EGLint egl_config_default_attrib_list[] = {
    EGL_BUFFER_SIZE,            0,
    EGL_RED_SIZE,               0,
    EGL_BLUE_SIZE,              0,
    EGL_GREEN_SIZE,             0,
    EGL_ALPHA_SIZE,             0,

    EGL_DEPTH_SIZE,             0,
    EGL_STENCIL_SIZE,           0,

    EGL_SAMPLE_BUFFERS,         0,
    EGL_SAMPLES,                0,

    // An oddly named attribute. Its value is a bitmask of OpenGL API's.
    EGL_RENDERABLE_TYPE,        0,

    // According to the EGL 1.4 spec Table 3.4, the default value of
    // EGL_SURFACE_BIT is EGL_WINDOW_BIT.  Explicitly set the default here for
    // the sake of self-documentation.
    EGL_SURFACE_TYPE,           EGL_WINDOW_BIT,
    EGL_NONE,
};

/// @brief Set EGL_BUFFER_SIZE to sum of rgba sizes.
/// @return true on success.
static bool
egl_config_set_buffer_size(EGLint egl_attrib_list[])
{
    const EGLint channels[] = {
        EGL_RED_SIZE,
        EGL_GREEN_SIZE,
        EGL_BLUE_SIZE,
        EGL_ALPHA_SIZE,
    };

    EGLint buffer_size = 0;
    bool ok = true;
    int i;

    for (i = 0; i < 4; ++i) {
        EGLint channel_size;
        ok &= waffle_attrib_list_get(egl_attrib_list,
                                     channels[i], &channel_size);
        if (!ok) {
            wcore_error_internal("failed to get channel size (i=%d)", i);
            return false;
        }
        buffer_size += channel_size;
    }

    ok &= waffle_attrib_list_update(egl_attrib_list,
                                    EGL_BUFFER_SIZE, buffer_size);
    if (!ok) {
        wcore_error_internal("%s", "failed to set EGL_BUFFER_SIZE");
        return false;
    }

    return true;
}

/// @brief Set EGL_RENDERABLE_TYPE in egl_attrib_list.
static bool
egl_config_set_renderable_type(EGLint egl_attrib_list[], int waffle_gl_api)
{
    EGLint value;
    bool ok = true;

    switch (waffle_gl_api) {
        case WAFFLE_GL:     value = EGL_OPENGL_BIT;       break;
        case WAFFLE_GLES1:  value = EGL_OPENGL_ES_BIT;    break;
        case WAFFLE_GLES2:  value = EGL_OPENGL_ES2_BIT;   break;

        default:
            wcore_error_internal("gl_api has bad value 0x%x", waffle_gl_api);
            return false;
    }

    ok &= waffle_attrib_list_update(egl_attrib_list,
                                   EGL_RENDERABLE_TYPE, value);
    if (!ok) {
        wcore_error_internal("%s", "failed to update EGL_RENDERABLE_TYPE");
        return false;
    }

    return true;

}

/// Translate an attribute list for a waffle_config into one for an EGLConfig.
///
/// @param[in] waffle_config_attrib_list is from waffle_config_choose().
/// @return null on failure.
static EGLint*
egl_config_translate_attrib_list(const int32_t waffle_attrib_list[])
{
    bool ok = true;

    EGLint *egl_config_attrib_list =
            malloc(sizeof(egl_config_default_attrib_list));
    if (!egl_config_attrib_list) {
        wcore_error(WAFFLE_OUT_OF_MEMORY);
        return NULL;
    }

    // Set defaults.
    memcpy(egl_config_attrib_list,
           egl_config_default_attrib_list,
           sizeof(egl_config_default_attrib_list));

    // Nothing to parse. Just use defaults.
    if (!waffle_attrib_list)
        return egl_config_attrib_list;

    for (const int32_t *i = waffle_attrib_list; *i != 0; i += 2) {
        // w_{attr,value} are in terms of a waffle attrib_list. Below they
        // are translated to e_{attr,value}, in terms of an EGL attrib_list.
        enum waffle_enum w_attr = i[0];
        int32_t w_value = i[1];

        if (w_attr == WAFFLE_DOUBLE_BUFFERED) {
            // Don't insert this attribute into egl_config_attrib_list. The
            // EGL_RENDER_BUFFER attribute belongs in the EGLSurface attribute
            // list.
            continue;
        }
        else {
            bool found;
            EGLint e_attr;
            EGLint e_value = w_value;

            found = waffle_attrib_list_get(egl_config_attrib_map,
                                           w_attr, &e_attr);
            if (!found) {
                wcore_errorf(WAFFLE_BAD_ATTRIBUTE,
                             "bad attribute 0x%x", w_attr);
                goto error;
            }

            waffle_attrib_list_update(egl_config_attrib_list, e_attr, e_value);
        }
    }

    ok &= egl_config_set_buffer_size(egl_config_attrib_list);
    if (!ok)
        goto error;

    return egl_config_attrib_list;

error:
    free(egl_config_attrib_list);
    return NULL;
}

static EGLint*
egl_config_make_attrib_list(
        const int32_t waffle_attrib_list[],
        int32_t waffle_gl_api)
{
    bool ok = true;
    EGLint *egl_attrib_list = NULL;

    egl_attrib_list = egl_config_translate_attrib_list(waffle_attrib_list);
    if (!egl_attrib_list)
        goto error;

    ok &= egl_config_set_renderable_type(egl_attrib_list, waffle_gl_api);
    if (!ok)
        goto error;

    return egl_attrib_list;

error:
    free(egl_attrib_list);
    return NULL;
}

EGLConfig
egl_choose_config(
        EGLDisplay dpy,
        const int32_t waffle_attrib_list[],
        int32_t waffle_gl_api)
{
    EGLConfig config = NULL;
    EGLint num_configs = 0;
    bool ok = true;

    EGLint *attrib_list = egl_config_make_attrib_list(waffle_attrib_list,
                                                      waffle_gl_api);
    if (!attrib_list)
        goto end;

    ok &= eglChooseConfig(dpy, attrib_list, &config, 1, &num_configs);
    if (!ok) {
        egl_get_error("eglChooseConfig");
        goto end;
    }
    else if (num_configs == 0) {
        ok = false;
        wcore_errorf(WAFFLE_UNKNOWN_ERROR,
                     "eglChooseConfig found no matching configs");
        goto end;
    }

end:
    free(attrib_list);
    return config;
}