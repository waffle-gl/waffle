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

#include "wcore_error.h"
#include "wcore_attrib_list.h"
#include "wegl_config.h"
#include "qnx_display.h"
#include "qnx_window_priv.h"
#include "qnx_display_priv.h"

struct qnx_window_priv*
qnx_window_priv_create(struct qnx_display *dpy,
                       int32_t width,
                       int32_t height)
{
    struct qnx_window_priv *self;

    int rc;
    const int format = SCREEN_FORMAT_RGB565;
    const int usage = SCREEN_USAGE_OPENGL_ES2 | SCREEN_USAGE_OPENGL_ES3;
    const int position[] = { 0, 0};
    const int size[] = {width, height};
    const int nbuffers = 2;
    const int interval = 1;
    const int transparency = SCREEN_TRANSPARENCY_NONE;

    self = wcore_calloc(sizeof(*self));
    if (self == NULL)
        return NULL;

    rc = screen_create_window(&self->screen_win, dpy->priv->screen_ctx);
    if (rc != 0) {
        wcore_error_errno("screen_create_window failed");
        goto error;
    }

    rc = screen_set_window_property_iv(self->screen_win,
                                       SCREEN_PROPERTY_FORMAT,
                                       &format
                                      );
    if (rc != 0) {
        wcore_error_errno("SCREEN_PROPERTY_FORMAT failed");
        goto error;
    }

    rc = screen_set_window_property_iv(self->screen_win,
                                       SCREEN_PROPERTY_USAGE,
                                       &usage
                                      );
    if (rc != 0) {
        wcore_error_errno("SCREEN_PROPERTY_USAGE failed");
        goto error;
    }

    rc = screen_set_window_property_iv(self->screen_win,
                                       SCREEN_PROPERTY_POSITION,
                                       &position[0]
                                      );
    if (rc != 0) {
        wcore_error_errno("SCREEN_PROPERTY_POSITION failed");
        goto error;
    }

    rc = screen_set_window_property_iv(self->screen_win,
                                       SCREEN_PROPERTY_SIZE,
                                       size
                                      );
    if (rc != 0) {
        wcore_error_errno("SCREEN_PROPERTY_SIZE failed");
        goto error;
    }

    rc = screen_create_window_buffers(self->screen_win,
                                      nbuffers
                                     );
    if (rc != 0) {
        wcore_error_errno("screen_create_window_buffers failed");
        goto error;
    }

    rc = screen_set_window_property_iv(self->screen_win,
                                       SCREEN_PROPERTY_SWAP_INTERVAL,
                                       &interval
                                      );
    if (rc != 0) {
        wcore_error_errno("SCREEN_PROPERTY_SWAP_INTERVAL failed");
        goto error;
    }

    rc = screen_set_window_property_iv(self->screen_win,
                                       SCREEN_PROPERTY_TRANSPARENCY,
                                       &transparency
                                      );
    if (rc != 0) {
        wcore_error_errno("SCREEN_PROPERTY_TRANSPARENCY failed");
        goto error;
    }

    return self;
error:
    qnx_window_priv_destroy(self);
    return NULL;
}

bool
qnx_window_priv_destroy(struct qnx_window_priv *self)
{
    int rc;

    if (self == NULL)
        return true;

    rc = screen_destroy_window(self->screen_win);
    if (rc !=0 ) {
        wcore_error_errno("screen_destroy_window failed");
    }

    free(self);
    return (rc == 0);
}

bool
qnx_window_priv_show(struct qnx_window_priv *self)
{
    int rc;
    const int is_visible = 1;

    rc = screen_set_window_property_iv(self->screen_win,
                                       SCREEN_PROPERTY_VISIBLE,
                                       &is_visible
                                      );
    if (rc != 0) {
        wcore_error_errno("SCREEN_PROPERTY_VISIBLE failed");
        goto error;
    }

    return true;
error:
    return false;
}

bool
qnx_window_priv_resize(struct qnx_window_priv *self,
                       int32_t width,
                       int32_t height)
 {
    int rc;
    const int size[] = {width, height};
    const int nbuffers = 2;
    const int transparency = SCREEN_TRANSPARENCY_NONE;

    rc = screen_destroy_window_buffers(self->screen_win);
    if (rc != 0) {
        wcore_error_errno("screen_destroy_window_buffers failed");
        goto error;
    }

    rc = screen_set_window_property_iv(self->screen_win,
                                       SCREEN_PROPERTY_SIZE,
                                       size
                                      );
    if (rc != 0) {
        wcore_error_errno("SCREEN_PROPERTY_SIZE failed");
        goto error;
    }

    rc = screen_create_window_buffers(self->screen_win, nbuffers);
    if (rc != 0) {
        wcore_error_errno("screen_create_window_buffers failed");
        goto error;
    }

    rc = screen_set_window_property_iv(self->screen_win,
                                       SCREEN_PROPERTY_TRANSPARENCY,
                                       &transparency
                                      );
    if (rc != 0) {
        wcore_error_errno("SCREEN_PROPERTY_TRANSPARENCY failed");
        goto error;
    }

    return true;
error:
    return false;
}
