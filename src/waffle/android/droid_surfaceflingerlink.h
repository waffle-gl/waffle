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

#ifdef __cplusplus
extern "C" {
#endif

struct droid_surfaceflinger_container;
struct droid_ANativeWindow_container;

void
droid_destroy_surface(
    struct droid_surfaceflinger_container* pSFContainer,
    struct droid_ANativeWindow_container* pANWContainer);

bool
droid_show_surface(
    struct droid_surfaceflinger_container* pSFContainer,
    struct droid_ANativeWindow_container* pANWContainer);

bool
droid_resize_surface(
    struct droid_surfaceflinger_container* pSFContainer,
    struct droid_ANativeWindow_container* pANWContainer,
    int width,
    int height);

struct droid_ANativeWindow_container*
droid_create_surface(
    int width,
    int height,
    struct droid_surfaceflinger_container* pSFContainer);

bool
droid_deinit_gl(struct droid_surfaceflinger_container* pSFContainer);

struct droid_surfaceflinger_container*
droid_init_gl();

#ifdef __cplusplus
};
#endif
