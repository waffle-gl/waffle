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

For more information, visit to Waffle's website.


[1] http://cgit.freedesktop.org/piglit/tree/README
[2] http://github.com/apitrace/apitrace#readme


Links
=====

website:        http://people.freedesktop.org/~chadversary/waffle
mailing-list:   waffle@lists.freedesktop.org
maintainer:     Chad Versace <chad.versace@linux.intel.com>
source:         git://people.freedesktop.org/~chadversary/waffle.git
gitweb:         http://cgit.freedesktop.org/~chadversary/waffle
license:        BSD [http://www.opensource.org/licenses/bsd-license.php]


Installing
==========

On Linux, you likely want to the command below. It will install into
/usr/local.

    cmake \
        -DCMAKE_LIBRARY_PATH=$(echo $LIBRARY_PATH | sed 's/:/;/g') \
        -Dwaffle_has_glx=1 \
        -Dwaffle_has_x11_egl=1 \
        $WAFFLE_SOURCE_DIR
    make
    make check
    make install

If you need to install into a custom location, autoconf-esque variables such
as CMAKE_INSTALL_LIBDIR are supported. For example,

    cmake \
        -DCMAKE_INSTALL_PREFIX=$HOME \
        -DCMAKE_INSTALL_LIBDIR=lib \
        -DCMAKE_LIBRARY_PATH=$(echo $LIBRARY_PATH | sed 's/:/;/g') \
        -Dwaffle_has_glx=1 \
        -Dwaffle_has_x11_egl=1 \
        $WAFFLE_SOURCE_DIR

For full details on configuring, building, and installing Waffle, see
/doc/building.txt.
