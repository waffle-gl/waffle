Waffle - a library for selecting GL API and window system at runtime

Summary
=======

Waffle is a cross-platform library that allows one to defer selection of GL
API and of window system until runtime. For example, on Linux, Waffle enables
an application to select X11/EGL with an OpenGL 3.3 core profile, Wayland
with OpenGL ES2, and other window system / API combinations.

Waffle's immediate goal is to enable Piglit [1] to test multiple GL flavors in
a cross-platform way, and to allow each Piglit test to choose its GL API and
window system at runtime. A future goal is to enable the ability to record
(with another tool such APITrace [2]) an application's GL calls on one
operating system or window system, and then replay that trace on a different
system.


[1] http://cgit.freedesktop.org/piglit/tree/README
[2] http://github.com/apitrace/apitrace#readme


Links
=====

homepage:       http://people.freedesktop.org/~chadversary/waffle
mailing-list:   waffle@lists.freedesktop.org
maintainer:     Chad Versace <chad.versace@linux.intel.com>
source:         git://people.freedesktop.org/~chadversary/waffle.git
gitweb:         http://cgit.freedesktop.org/~chadversary/waffle
license:        Apache 2.0 [http://www.apache.org/licenses/LICENSE-2.0]


Features
========

Waffle supports the following GL API's:
    - OpenGL
    - OpenGL ES1
    - OpenGL ES2

Waffle supports, or has planned support, for the following window systems:
    - Android   (planned)
    - Apple/GLX (planned)
    - Apple/CGL (planned, coming next)
    - GLX       (complete)
    - Wayland   (complete)
    - Windows   (planned)
    - X11/EGL   (complete)


Example Usage
=============

The example below is just a code snippet that demonstrates basic usage of
Waffle. It is not complete; it does not handle errors.  For a complete
example, see `/examples/gl_basic.c` [3].

[3] http://cgit.freedesktop.org/~chadversary/waffle/tree/gl_basic.c.

    #include <waffle/waffle.h>

    // Declare function pointers and enums for glClear, glClearColor.

    int
    main()
    {
        const int32_t init_attrs[] = {
            WAFFLE_PLATFORM, WAFFLE_PLATFORM_X11_EGL,
            0,
        };

        const int32_t config_attrs[] = {
            WAFFLE_CONTEXT_API,                 WAFFLE_CONTEXT_OPENGL,
            WAFFLE_CONTEXT_MAJOR_VERSION,       3,
            WAFFLE_CONTEXT_MINOR_VERSION,       2,
            WAFFLE_CONTEXT_PROFILE,             WAFFLE_CONTEXT_CORE_PROFILE,

            WAFFLE_RED_SIZE,            8,
            WAFFLE_BLUE_SIZE,           8,
            WAFFLE_GREEN_SIZE,          8,

            0,
        };

        // Setup waffle objects.
        waffle_init(init_attrs);
        struct waffle_display *dpy = waffle_display_connect(NULL);
        struct waffle_config *config = waffle_config_choose(dpy, config_attrs);
        struct waffle_window *window = waffle_window_create(dpy, width, height);
        struct waffle_context *ctx = waffle_context_create(config, NULL);

        glClearColor = waffle_dl_sym(WAFFLE_DL_OPENGL, "glClearColor");
        glClear = waffle_dl_sym(WAFFLE_DL_OPENGL, "glClear");

        waffle_make_current(dpy, window, ctx);
        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        waffle_window_show(window);
        waffle_window_swap_buffers(window);

        // Teardown waffle.
        waffle_make_current(dpy, NULL, NULL);
        waffle_window_destroy(window);
        waffle_context_destroy(ctx);
        waffle_config_destroy(config);
        waffle_display_disconnect(dpy);

        return 0;
    }


Installing
==========

On Linux, you likely want to do this:

    cmake \
        -DCMAKE_LIBRARY_PATH=$(echo $LIBRARY_PATH | sed 's/:/;/g') \
        -Dwaffle_has_glx=1 \
        -Dwaffle_has_x11_egl=1 \
        $WAFFLE_SOURCE_DIR
    make
    make check
    make install

For full details on configuring, building, and installing Waffle, see
/doc/building.txt.


License
=======

For a list of copyright holders, see NOTICE.txt.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
