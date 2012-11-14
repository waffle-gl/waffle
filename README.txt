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


Build Requirements
==================

Linux
-----

Waffle uses CMake for it build system.

    Archlinux: pacman -S cmake
    Fedora 17: yum install cmake

To build the manpages, xsltproc and the Docbook XSL stylesheets are required.

    Archlinux: pacman -S libxslt docbook-xsl
    Fedora 17: yum install libxslt docbook-style-xsl

If you choose to enable support for a given platform (for example,
-Dwaffle_has_glx=1), then CMake will complain if the necessary libraries are
not installed. Listed below are the required packages for each platform.

If you install Mesa from source, then Mesa must be configured with option
`--with-egl-platforms=PLATFORM_LIST`, where PLATFORM_LIST is
a comman-separated list of any combination of "x11", "wayland", and "drm".

    - GLX:
        - Archlinux: pacman -S libgl libxcb libx11
        - Fedora 17: yum install mesa-libGL-devel libxcb-devel libX11-devel

    - X11/EGL:
        - all: If you choose to install Mesa from source, use --with-egl-platforms=x11.
        - Archlinux: pacman -S libegl libxcb libx11
        - Fedora 17: yum install mesa-libEGL-devel libxcb-devel libX11-devel

    - Wayland:
        - all: Install wayland>=1.0 from source.
        - all: Install mesa-9.1-devel from source. Use --with-egl-platforms=wayland.

    - GBM:
        - all: Install mesa-9.1-devel from source. Use --with-egl-platforms=drm.
        - Archlinux: pacman -S systemd
        - Fedora 17: yum install libudev-devel


Installing
==========

If any of Waffle's dependencies are installed in custom locations, you must
set the PKG_CONFIG_PATH environment variable.

First, enter the top of the waffle source tree.

    git clone git://people.freedesktop.org/~chadversary/waffle
    cd waffle

or

    tar xvf waffle-0.0.0.tar.xz
    cd waffle-0.0.0

On Linux, you likely want to use the commands below. The commands configure
waffle with support for only GLX and installs into /usr/local.

    cmake -Dwaffle_has_glx=1 .

    make
    make check
    make install

If in addition to GLX you want support for X11/EGL, then add
-Dwaffle_has_x11_egl=1 to the cmake arguments. Likewise for Wayland and
-Dwaffle_has_wayland=1.

If you need to install into a custom location, autoconf-esque variables such
as CMAKE_INSTALL_LIBDIR are supported. For example,

    cmake \
        -DCMAKE_INSTALL_PREFIX=$HOME \
        -DCMAKE_INSTALL_LIBDIR=lib \
        -Dwaffle_has_glx=1 \
        .

For full details on configuring, building, and installing Waffle, see
/doc/building.txt.
