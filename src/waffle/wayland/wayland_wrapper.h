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

#pragma once

#include <stdbool.h>

#include "wayland-util.h"

bool
wayland_wrapper_init(void);

bool
wayland_wrapper_teardown(void);


// Data symbols
const struct wl_interface *wfl_wl_compositor_interface;
const struct wl_interface *wfl_wl_registry_interface;
const struct wl_interface *wfl_wl_shell_interface;
const struct wl_interface *wfl_wl_shell_surface_interface;
const struct wl_interface *wfl_wl_surface_interface;


// Forward declaration of the structs required by the functions
struct wl_proxy;
struct wl_display;


// Functions
struct wl_display *
(*wfl_wl_display_connect)(const char *name);

void
(*wfl_wl_display_disconnect)(struct wl_display *display);

int
(*wfl_wl_display_roundtrip)(struct wl_display *display);


void
(*wfl_wl_proxy_destroy)(struct wl_proxy *proxy);

int
(*wfl_wl_proxy_add_listener)(struct wl_proxy *proxy,
                             void (**implementation)(void), void *data);

void
(*wfl_wl_proxy_marshal)(struct wl_proxy *p, uint32_t opcode, ...);

struct wl_proxy *
(*wfl_wl_proxy_marshal_constructor)(struct wl_proxy *proxy,
                                    uint32_t opcode,
                                    const struct wl_interface *interface,
                                    ...);

struct wl_proxy *
(*wfl_wl_proxy_marshal_constructor_versioned)(struct wl_proxy *proxy,
					      uint32_t opcode,
					      const struct wl_interface *interface,
					      uint32_t version,
					      ...);

#ifdef _WAYLAND_CLIENT_H
#error Do not include wayland-client.h ahead of wayland_wrapper.h
#endif

#define wl_compositor_interface (*wfl_wl_compositor_interface)
#define wl_registry_interface (*wfl_wl_registry_interface)
#define wl_shell_interface (*wfl_wl_shell_interface)
#define wl_shell_surface_interface (*wfl_wl_shell_surface_interface)
#define wl_surface_interface (*wfl_wl_surface_interface)

#define wl_display_connect (*wfl_wl_display_connect)
#define wl_display_disconnect (*wfl_wl_display_disconnect)
#define wl_display_roundtrip (*wfl_wl_display_roundtrip)
#define wl_proxy_destroy (*wfl_wl_proxy_destroy)
#define wl_proxy_add_listener (*wfl_wl_proxy_add_listener)
#define wl_proxy_marshal (*wfl_wl_proxy_marshal)
#define wl_proxy_marshal_constructor (*wfl_wl_proxy_marshal_constructor)
#define wl_proxy_marshal_constructor_versioned (*wfl_wl_proxy_marshal_constructor_versioned)
