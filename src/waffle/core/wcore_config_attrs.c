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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "wcore_attrib_list.h"
#include "wcore_config_attrs.h"
#include "wcore_error.h"
#include "wcore_util.h"

enum {
    DEFAULT_ACCUM_BUFFER = false,
    DEFAULT_DOUBLE_BUFFERED = true,
    DEFAULT_SAMPLE_BUFFERS = false,
};

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
            case WAFFLE_CONTEXT_FORWARD_COMPATIBLE:
            case WAFFLE_CONTEXT_DEBUG:
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
parse_bool(const int32_t attrib_list[],
		   int32_t attrib_name,
		   bool *value,
		   bool default_value)
{
	int32_t raw_value;

	wcore_attrib_list_get_with_default(attrib_list, attrib_name,
                                       &raw_value, default_value);

	if (raw_value == WAFFLE_DONT_CARE) {
		*value = default_value;
	} else if (raw_value == true || raw_value == false) {
		*value = raw_value;
	} else {
		wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
					 "%s has bad value 0x%x. "
					 "Must be true(1), false(0), or WAFFLE_DONT_CARE(-1)",
					 wcore_enum_to_string(attrib_name), raw_value);
		return false;
	}

    return true;
}

static bool
parse_context_api(struct wcore_config_attrs *attrs,
                  const int32_t attrib_list[])
{
    bool found;

    found = wcore_attrib_list_get(attrib_list,
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
    wcore_attrib_list_get_with_default(attrib_list,
                                       WAFFLE_CONTEXT_MAJOR_VERSION,
                                       &attrs->context_major_version,
                                       attrs->context_major_version);
    wcore_attrib_list_get_with_default(attrib_list,
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

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (wcore_config_attrs_version_lt(attrs, 10)) {
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "for OpenGL, the requested context version "
                             "must be >= 1.0");
                return false;
            }
            break;

        case WAFFLE_CONTEXT_OPENGL_ES1:
            if (!wcore_config_attrs_version_eq(attrs, 10) &&
                !wcore_config_attrs_version_eq(attrs, 11)) {
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
            if (wcore_config_attrs_version_ge(attrs, 32)) {
                attrs->context_profile = WAFFLE_CONTEXT_CORE_PROFILE;
            }
            else {
                attrs->context_profile = WAFFLE_NONE;
            }
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
    wcore_attrib_list_get_with_default(attrib_list,
                                       WAFFLE_CONTEXT_PROFILE,
                                       &attrs->context_profile,
                                       attrs->context_profile);

    switch (attrs->context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            if (wcore_config_attrs_version_ge(attrs, 32)
                && attrs->context_profile != WAFFLE_CONTEXT_CORE_PROFILE
                && attrs->context_profile != WAFFLE_CONTEXT_COMPATIBILITY_PROFILE) {
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "for OpenGL >= 3.2, "
                             "WAFFLE_CONTEXT_PROFILE must be either "
                             "WAFFLE_CONTEXT_CORE_PROFILE or "
                             "WAFFLE_CONTEXT_COMPATIBILITY_PROFILE");
                return false;
            }
            else if (wcore_config_attrs_version_lt(attrs, 32)
                     && attrs->context_profile != WAFFLE_NONE) {
                wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                             "for OpenGL < 3.2, WAFFLE_CONTEXT_PROFILE must be "
                             "WAFFLE_NONE");
                return false;
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
parse_context_forward_compatible(struct wcore_config_attrs *attrs,
                                 const int32_t attrib_list[])
{
    if (!parse_bool(attrib_list, WAFFLE_CONTEXT_FORWARD_COMPATIBLE,
                    &attrs->context_forward_compatible, false)) {
        return false;
    }

    if (!attrs->context_forward_compatible) {
        /* It's always valid to request a non-forward-compatible context. */
        return true;
    }

    if (attrs->context_api != WAFFLE_CONTEXT_OPENGL) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "To request a forward-compatible context, the context "
                     "API must be WAFFLE_CONTEXT_OPENGL");
        return false;

    }

    if (wcore_config_attrs_version_lt(attrs, 30)) {
        wcore_errorf(WAFFLE_ERROR_BAD_ATTRIBUTE,
                     "To request a forward-compatible context, the context "
                     "version must be at least 3.0");
        return false;
    }

    return true;
}

static bool
set_misc_defaults(struct wcore_config_attrs *attrs)
{
    // Per the GLX [1] and EGL [2] specs, the default value of each size
    // attribute and `samples` is 0.
    //
    // [1] GLX 1.4 spec (2005.12.16), Table 3.4
    // [2] EGL 1.4 spec (2011.04.06), Table 3.4

    attrs->context_debug        = false;

    attrs->rgba_size            = 0;
    attrs->red_size             = 0;
    attrs->green_size           = 0;
    attrs->blue_size            = 0;
    attrs->alpha_size           = 0;
    attrs->depth_size           = 0;
    attrs->stencil_size         = 0;
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
                    else {                                                     \
                        /*                                                     \
                         * Pass WAFFLE_DONT_CARE to platform module.           \
                         *                                                     \
                         * From the GLX 1.4 (2005.12.16) spec:                 \
                         *     GLX_DONT_CARE may be specified for all          \
                         *     attributes except GLX_LEVEL.                    \
                         *                                                     \
                         * From the EGL 1.4 (2011.04.06) spec:                 \
                         *     EGL_DONT_CARE may be specified for all          \
                         *     attributes except EGL_LEVEL and                 \
                         *     EGL_MATCH_NATIVE_PIXMAP.                        \
                         */                                                    \
                        attrs->struct_memb = value;                            \
                    }                                                          \
                    break;

            #define CASE_BOOL(enum_name, struct_memb, default_value)               \
                case enum_name:                                                    \
                    if (!parse_bool(attrib_list, enum_name,                        \
                                    &attrs->struct_memb, default_value)) {         \
                        return false;                                              \
                    }                                                              \
                break;

            case WAFFLE_CONTEXT_API:
            case WAFFLE_CONTEXT_MAJOR_VERSION:
            case WAFFLE_CONTEXT_MINOR_VERSION:
            case WAFFLE_CONTEXT_PROFILE:
            case WAFFLE_CONTEXT_FORWARD_COMPATIBLE:
                // These keys have already been parsed.
                break;

            CASE_INT(WAFFLE_RED_SIZE, red_size)
            CASE_INT(WAFFLE_GREEN_SIZE, green_size)
            CASE_INT(WAFFLE_BLUE_SIZE, blue_size)
            CASE_INT(WAFFLE_ALPHA_SIZE, alpha_size)
            CASE_INT(WAFFLE_DEPTH_SIZE, depth_size)
            CASE_INT(WAFFLE_STENCIL_SIZE, stencil_size)
            CASE_INT(WAFFLE_SAMPLES, samples)

            CASE_BOOL(WAFFLE_CONTEXT_DEBUG, context_debug, false);
            CASE_BOOL(WAFFLE_SAMPLE_BUFFERS, sample_buffers, DEFAULT_SAMPLE_BUFFERS);
            CASE_BOOL(WAFFLE_DOUBLE_BUFFERED, double_buffered, DEFAULT_DOUBLE_BUFFERED);
            CASE_BOOL(WAFFLE_ACCUM_BUFFER, accum_buffer, DEFAULT_ACCUM_BUFFER);

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
        && wcore_config_attrs_version_ge(attrs, 32)
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

    if (!parse_context_forward_compatible(attrs, waffle_attrib_list))
        return false;

    if (!set_misc_defaults(attrs))
        return false;

    if (!parse_misc(attrs, waffle_attrib_list))
        return false;

    if (!check_final(attrs))
        return false;

    return true;
}

bool
wcore_config_attrs_version_eq(
      const struct wcore_config_attrs *attrs,
      int merged_version)
{
    return
        attrs->context_major_version == (merged_version / 10) &&
        attrs->context_minor_version == (merged_version % 10);
}

bool
wcore_config_attrs_version_gt(
      const struct wcore_config_attrs *attrs,
      int merged_version)
{
    return
        attrs->context_major_version > (merged_version / 10) ||
        (attrs->context_major_version == (merged_version / 10) &&
         attrs->context_minor_version > (merged_version % 10));
}

bool
wcore_config_attrs_version_ge(
      const struct wcore_config_attrs *attrs,
      int merged_version)
{
    return
        attrs->context_major_version > (merged_version / 10) ||
        (attrs->context_major_version == (merged_version / 10) &&
         attrs->context_minor_version >= (merged_version % 10));
}

bool
wcore_config_attrs_version_lt(
      const struct wcore_config_attrs *attrs,
      int merged_version)
{
    return !wcore_config_attrs_version_ge(attrs, merged_version);
}

bool
wcore_config_attrs_version_le(
      const struct wcore_config_attrs *attrs,
      int merged_version)
{
    return !wcore_config_attrs_version_gt(attrs, merged_version);
}
