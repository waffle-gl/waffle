Waffle - a library for selecting GL API and window system at runtime

homepage:       http://people.freedesktop.org/~chadversary/waffle
mailing-list:   TODO
maintainer:     Chad Versace <chad@chad.versace@linux.intel.com>
source:         git://people.freedesktop.org/~chadversary/waffle.git
gitweb:         http://cgit.freedesktop.org/~chadversary/waffle
license:        Apache 2.0


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


Features
========

Waffle supports the following GL API's:
    - OpenGL
    - OpenGL ES1
    - OpenGL ES2

Waffle does not yet support selection of OpenGL version and profile, but
that's on the short todo list.

Waffle supports, or has planned support, for the following window systems:
    - Android   (planned)
    - Apple/GLX (planned)
    - Apple/CGL (planned)
    - GLX       (planned, coming next)
    - Wayland   (planned, after GLX)
    - Windows   (planned)
    - X11/EGL   (complete)


Example Usage
=============

The example below is just a code snippet that demonstrates basic usage of
Waffle. It is not complete; it does not handle errors.  For a complete
example, see `/examples/gl_basic.c` [3].

[3] http://cgit.freedesktop.org/~chadversary/waffle/tree/gl_basic.c.

-----------------------------
    #include <waffle/waffle.h>

    // Declare function pointers and enums for glClear, glClearColor.

    int
    main()
    {
        const int32_t init_attrs[] = {
            WAFFLE_PLATFORM,            WAFFLE_PLATFORM_X11_EGL,
            WAFFLE_OPENGL_API,          WAFFLE_OPENGL_ES2,
            0,
        };

        const int32_t config_attrs[] = {
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
        struct waffle_context *ctx = waffle_context_creat(config, NULL);

        glClearColor = waffle_dlsym_gl("glClearColor");
        glClear = waffle_dlsym_gl("glClear");

        waffle_make_current(dpy, window, ctx);
        glClearColor(1.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        waffle_window_swap_buffers(window);

        // Teardown waffle.
        waffle_window_destroy(window);
        waffle_context_destroy(ctx);
        waffle_config_destroy(config);
        waffle_display_disconnect(dpy);
        waffle_finish();

        return 0;
    }
-----------------------------


Building
========

Below is a full description of Waffle's configuration options. If CMake scares
you and you don't like reading, just do this:
    cmake \
        -DCMAKE_LIBRARY_PATH=$(echo $LIBRARY_PATH | sed 's/:/;/g') \
        -Dwaffle_default_platform=x11_egl \
        -Dwaffle_default_api=gl \
        $WAFFLE_SOURCE_DIR
    make
    make check
    make install


Configuration Options
---------------------

Options are given in -Dname=value format.

CMake options:
    Unlike Autoconf, CMake doesn't detect environment variables like
    LIBRARY_PATH and CFLAGS. You need to set some of the options below to get
    that behavior.

    - CMAKE_BUILD_TYPE
        [default=?; choices=debug,release]
        This affects CFLAGS.

    - CMAKE_C_FLAGS
        [default="--std=c99 -Wall -Werror"]

    - CMAKE_C_FLAGS_DEBUG
        [default="-g3 -O0 -DDEBUG"]
        Flags to append to CMAKE_C_FLAGS when CMAKE_BUILD_TYPE=debug.

    - CMAKE_C_FLAGS_RELEASE
        [default="-g1 -O2 -DNEDEBUG"]
        Flags to append to CMAKE_C_FLAGS when CMAKE_BUILD_TYPE=release.

    - CMAKE_LIBRARY_PATH
        [default=none]
        A semicolon-separated list of directories in which to search for
        libraries.

    - CMAKE_INSTALL_PREFIX
        [default=/usr/local]
        Equivalent to autoconf's --prefix.

Platform options:

    - waffle_has_x11_egl
        [default=1 if libraries are autodetected] [choices=0,1]
        Build Waffle with support for X11/EGL.

Install options:
    - waffle_install_includedir
        [default=${CMAKE_INSTALL_PREFIX}/include]
        Directory where libraries will be instaled.

    - waffle_install_libdir
        [default=${CMAKE_INSTALL_PREFIX}/lib]
        Directory where libraries will be installed.

    - waffle_install_docdir
        [default=${CMAKE_INSTALL_PREFIX}/share/doc/waffle]
        Directory where documentation will be instaled.

Miscellaneous options:

    - waffle_build_tests
        [default=1; choices=0,1]
        Build the unit tests and functional tests ran by `make check`.

    - waffle_build_examples
        [default=1; choices=0,1]
        Build the stuff in the /examples directory.


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
