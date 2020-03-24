WAFFLE_WAYLAND_INTERFACE(wl_compositor_interface)
WAFFLE_WAYLAND_INTERFACE(wl_registry_interface)
WAFFLE_WAYLAND_INTERFACE(wl_shell_interface)
WAFFLE_WAYLAND_INTERFACE(wl_shell_surface_interface)
WAFFLE_WAYLAND_INTERFACE(wl_surface_interface)


WAFFLE_WAYLAND_SYM(struct wl_display *, wl_display_connect, (const char *name))
WAFFLE_WAYLAND_SYM(void, wl_display_disconnect, (struct wl_display *display))

WAFFLE_WAYLAND_SYM(int, wl_display_roundtrip, (struct wl_display *display))

WAFFLE_WAYLAND_SYM(void, wl_proxy_destroy, (struct wl_proxy *proxy))

WAFFLE_WAYLAND_SYM(int, wl_proxy_add_listener, (struct wl_proxy *proxy,
                                                void (**implementation)(void),
                                                void *data))

WAFFLE_WAYLAND_SYM(void, wl_proxy_marshal, (struct wl_proxy *p,
                                            uint32_t opcode, ...))

WAFFLE_WAYLAND_SYM(struct wl_proxy *, wl_proxy_marshal_constructor,
                   (struct wl_proxy *proxy, uint32_t opcode,
                    const struct wl_interface *interface, ...))

WAFFLE_WAYLAND_SYM(struct wl_proxy *, wl_proxy_marshal_constructor_versioned,
                   (struct wl_proxy *proxy, uint32_t opcode,
                    const struct wl_interface *interface, uint32_t version, ...))
