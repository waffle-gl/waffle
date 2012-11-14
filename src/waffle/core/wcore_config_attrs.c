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

/// @addtogroup wcore_config_attrs
/// @{

/// @file

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "waffle_attrib_list.h"
#include "waffle_enum.h"

#include "wcore_config_attrs.h"
#include "wcore_error.h"

static bool
check_keys(const int32_t attrib_list[])
{
    if (attrib_list == NULL)
        return true;

    for (int32_t i = 0; attrib_list[i]; i += 2) {
        int32_t key = attrib_list[i];

        switch (key) {
            case WAFFLE_CONTEXT_API:
            case WAFFLE_CONTEXT_MAJOR_VERSION:
            case WAFFLE_CONTEXT_MINOR_VERSION:
            case WAFFLE_CONTEXT_PROFILE:
            case WAFFLE_RED_SIZE:
            case WAFFLE_GREEN_SIZE:
            case WAFFLE_BLUE_SIZE:
            case WAFFLE_ALPHA_SIZE:
            case WAFFLE_DEPTH_SIZE:
            case WAFFLE_STENCIL_SIZE:
            case WAFFLE_SAMPLES:
            case WAFFLE_SAMPLE_BUFFERS:
            case WAFFLE_DOUBLE_BUFFERED:
            case WAFFLE_ACCUM_BUFFER:
                break;
            default:
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "unrecognized attribute 0x%x at attrib_list[%d]",
                             key, i);
                return false;
        }
    }

    return true;
}

static bool
parse_context_api(struct wcore_config_attrs *attrs,
                  const int32_t attrib_list[])
{
    bool found;

    found = waffle_attrib_list_get(attrib_list,
                                   WAFFLE_CONTEXT_API, &attrs->context_api);
    if (!found) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "required attribute WAFFLE_CONTEXT_API is missing");
        return false;
    }

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
        case WAFFLE_CONTEXT_OPENGL_ES3:
            break;
        default:
            wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                         "WAFFLE_CONTEXT_API has bad value %#x",
                         attrs->context_api);
            return false;
    }

    return true;
}

static bool
set_context_version_default(struct wcore_config_attrs *attrs)
{
    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            attrs->context_major_version = 1;
            attrs->context_minor_version = 0;
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES1:
            attrs->context_major_version = 1;
            attrs->context_minor_version = 0;
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES2:
            attrs->context_major_version = 2;
            attrs->context_minor_version = 0;
            return true;
        case WAFFLE_CONTEXT_OPENGL_ES3:
            attrs->context_major_version = 3;
            attrs->context_minor_version = 0;
            return true;
        default:
            wcore_error_internal("attrs->context_api has bad value 0x%x",
                                 attrs->context_api);
            return false;
    }
}

static bool
parse_context_version(struct wcore_config_attrs *attrs,
                      const int32_t attrib_list[])
{
    waffle_attrib_list_get_with_default(attrib_list,
                                        WAFFLE_CONTEXT_MAJOR_VERSION,
                                        &attrs->context_major_version,
                                        attrs->context_major_version);
    waffle_attrib_list_get_with_default(attrib_list,
                                        WAFFLE_CONTEXT_MINOR_VERSION,
                                        &attrs->context_minor_version,
                                        attrs->context_minor_version);

    if (attrs->context_major_version < 1) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "WAFFLE_CONTEXT_MAJOR_VERSION must be >= 1");
        return false;
    }

    if (attrs->context_minor_version < 0) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "WAFFLE_CONTEXT_MINOR_VERSION must be >= 0");
        return false;
    }

    attrs->context_full_version =
            10 * attrs->context_major_version
            + attrs->context_minor_version;

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (attrs->context_full_version < 10) {
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "for OpenGL, the requested context version "
                             "must be >= 1.0");
                return false;
            }
            break;

        case WAFFLE_CONTEXT_OPENGL_ES1:
            if (attrs->context_full_version != 10 &&
                attrs->context_full_version != 11) {
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "for OpenGL ES1, the requested context version "
                             "must be 1.0 or 1.1");
                return false;
            }
            break;

        case WAFFLE_CONTEXT_OPENGL_ES2:
            if (attrs->context_major_version != 2) {
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "for OpenGL ES2, the requested major context "
                             "version must be 2");
                return false;
            }
            break;

        case WAFFLE_CONTEXT_OPENGL_ES3:
            if (attrs->context_major_version != 3) {
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "for OpenGL ES3, the requested major context "
                             "version must be 3");
                return false;
            }
            break;

        default:
            wcore_error_internal("attrs->context_api has bad value 0x%x",
                                 attrs->context_api);
            return false;
    }

    return true;
}

static bool
set_context_profile_default(struct wcore_config_attrs *attrs)
{
    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            attrs->context_profile = WAFFLE_CONTEXT_CORE_PROFILE;
            break;
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
        case WAFFLE_CONTEXT_OPENGL_ES3:
            attrs->context_profile = WAFFLE_NONE;
            break;
        default:
            assert(false);
            return false;
    }

    return true;
}

static bool
parse_context_profile(struct wcore_config_attrs *attrs,
                      const int32_t attrib_list[])
{
    waffle_attrib_list_get_with_default(attrib_list,
                                        WAFFLE_CONTEXT_PROFILE,
                                        &attrs->context_profile,
                                        attrs->context_profile);

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (attrs->context_full_version >= 32
                && attrs->context_profile != WAFFLE_CONTEXT_CORE_PROFILE
                && attrs->context_profile != WAFFLE_CONTEXT_COMPATIBILITY_PROFILE) {
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "for OpenGL >= 3.2,"
                             "WAFFLE_CONTEXT_PROFILE must be either "
                             "WAFFLE_CONTEXT_CORE_PROFILE or "
                             "WAFFLE_CONTEXT_COMPATIBILITY_PROFILE");
                return false;
            }
            else {
                // Ignore profile.
            }
            break;
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
        case WAFFLE_CONTEXT_OPENGL_ES3:
            if (attrs->context_profile != WAFFLE_NONE) {
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "for OpenGL ES, WAFFLE_CONTEXT_PROFILE must be "
                             "WAFFLE_NONE");
                return false;
            }
            break;
        default:
            assert(false);
            return false;
    }

    return true;
}

static bool
set_misc_defaults(struct wcore_config_attrs *attrs)
{
    attrs->rgba_size            = 0;
    attrs->red_size             = WAFFLE_DONT_CARE;
    attrs->green_size           = WAFFLE_DONT_CARE;
    attrs->blue_size            = WAFFLE_DONT_CARE;
    attrs->alpha_size           = WAFFLE_DONT_CARE;
    attrs->depth_size           = WAFFLE_DONT_CARE;
    attrs->stencil_size         = WAFFLE_DONT_CARE;
    attrs->sample_buffers       = 0;
    attrs->samples              = 0;
    attrs->double_buffered      = true;
    attrs->accum_buffer         = false;

    return true;
}

static bool
parse_misc(struct wcore_config_attrs *attrs,
           const int32_t attrib_list[])
{
    for (int32_t i = 0; attrib_list[i]; i += 2) {
        int32_t key = attrib_list[i + 0];
        int32_t value = attrib_list[i + 1];

        switch (key) {

            #define CASE_INT(enum_name, struct_memb)                           \
                case enum_name:                                                \
                    if (value < -1) {                                          \
                        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,               \
                                     #enum_name " has bad value %d", value);   \
                                     return false;                             \
                    }                                                          \
                    attrs->struct_memb = value;                                \
                    break;

            #define CASE_BOOL(enum_name, struct_memb)                              \
                case enum_name:                                                    \
                    if (value != true && value != false) {                         \
                        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,                   \
                                     #enum_name " has bad value 0x%x, must be "    \
                                     "true(1) or false(0)", value);                \
                                     return false;                                 \
                    }                                                              \
                    attrs->struct_memb = value;                                    \
                    break;

            case WAFFLE_CONTEXT_API:
            case WAFFLE_CONTEXT_MAJOR_VERSION:
            case WAFFLE_CONTEXT_MINOR_VERSION:
            case WAFFLE_CONTEXT_PROFILE:
                // These keys have already been parsed.
                break;

            CASE_INT(WAFFLE_RED_SIZE, red_size)
            CASE_INT(WAFFLE_GREEN_SIZE, green_size)
            CASE_INT(WAFFLE_BLUE_SIZE, blue_size)
            CASE_INT(WAFFLE_ALPHA_SIZE, alpha_size)
            CASE_INT(WAFFLE_DEPTH_SIZE, depth_size)
            CASE_INT(WAFFLE_STENCIL_SIZE, stencil_size)
            CASE_INT(WAFFLE_SAMPLES, samples)

            CASE_BOOL(WAFFLE_SAMPLE_BUFFERS, sample_buffers);
            CASE_BOOL(WAFFLE_DOUBLE_BUFFERED, double_buffered);
            CASE_BOOL(WAFFLE_ACCUM_BUFFER, accum_buffer);

            default:
                wcore_error_internal("%s", "bad attribute key should have "
                                     "been found by check_keys()");
                return false;


            #undef CASE_INT
            #undef CASE_BOOL
        }
    }

    // Calculate rgb_size.
    attrs->rgb_size = 0;
    if (attrs->red_size != WAFFLE_DONT_CARE)
        attrs->rgb_size += attrs->red_size;
    if (attrs->green_size != WAFFLE_DONT_CARE)
        attrs->rgb_size += attrs->green_size;
    if (attrs->blue_size != WAFFLE_DONT_CARE)
        attrs->rgb_size += attrs->blue_size;

    // Calculate rgba_size.
    attrs->rgba_size = attrs->rgb_size;
    if (attrs->alpha_size != WAFFLE_DONT_CARE)
        attrs->rgba_size += attrs->alpha_size;

    return true;
}

static bool
check_final(struct wcore_config_attrs *attrs)
{
    if (attrs->context_api == WAFFLE_CONTEXT_OPENGL
        && attrs->context_profile == WAFFLE_CONTEXT_CORE_PROFILE
        && attrs->context_full_version >= 32
        && attrs->accum_buffer) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "%s", "WAFFLE_ACCUM_BUFFER must be false for"
                     "OpenGL Core profile");
        return false;
    }

    return true;
}

bool
wcore_config_attrs_parse(
      const int32_t waffle_attrib_list[],
      struct wcore_config_attrs *attrs)
{
    memset(attrs, 0, sizeof(*attrs));

    if (!check_keys(waffle_attrib_list))
        return false;

    if (!parse_context_api(attrs, waffle_attrib_list))
        return false;

    if (!set_context_version_default(attrs))
        return false;

    if (!parse_context_version(attrs, waffle_attrib_list))
        return false;

    if (!set_context_profile_default(attrs))
        return false;

    if (!parse_context_profile(attrs, waffle_attrib_list))
        return false;

    if (!set_misc_defaults(attrs))
        return false;

    if (!parse_misc(attrs, waffle_attrib_list))
        return false;

    if (!check_final(attrs))
        return false;

    return true;
}

/// @}
