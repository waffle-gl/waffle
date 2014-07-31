Waffle - a library for selecting an OpenGL API and window system at runtime

Summary
=======

Waffle is a cross-platform library that allows one to defer selection of an
OpenGL API and of window system until runtime. For example, on Linux, Waffle
enables an application to select X11/EGL with an OpenGL 3.3 core profile,
Wayland with OpenGL ES2, and other window system / API combinations.

Waffle's immediate goal is to enable Piglit [1] to test multiple OpenGL
flavors in a cross-platform way, and to allow each Piglit test to choose its
OpenGL API and window system at runtime. A future goal is to enable the
ability to record (with another tool such APITrace [2]) an application's
OpenGL calls on one operating system or window system, and then replay
that trace on a different system.

For more information, visit to Waffle's website.


[1] http://piglit.freedesktop.org
[2] http://github.com/apitrace/apitrace#readme


Links
=====

source:         git://github.com/waffle-gl/waffle
gitweb:         https://github.com/waffle-gl/waffle
issue-tracker:  https://github.com/waffle-gl/waffle/issues
website:        http://www.waffle-gl.org
mailing-list:   waffle@lists.freedesktop.org
license:        BSD [http://www.opensource.org/licenses/bsd-license.php]
maintainer:     Chad Versace <chad.versace@linux.intel.com>


Examples
========

For example code and makefiles that demonstrate how to use and build with
Waffle, see the following:

    - The 'examples' directory in the source repository:
      https://github.com/waffle-gl/waffle/tree/master/examples

    - Installed examples at $PREFIX/share/doc/waffle-VERSION/examples, if
      Waffle is installed on your system.


Build Requirements
==================

Linux
-----

Waffle uses CMake for it build system.

    Archlinux: pacman -S cmake
    Fedora 17: yum install cmake
    Debian: apt-get install cmake

To build the manpages or html documentation, xsltproc and the Docbook XSL
stylesheets are required.

    Archlinux: pacman -S libxslt docbook-xsl
    Fedora 17: yum install libxslt docbook-style-xsl
    Debian: apt-get install xsltproc docbook-xsl

If you choose to enable support for a given platform (for example,
-Dwaffle_has_glx=1), then CMake will complain if the necessary libraries are
not installed. Listed below are the required packages for each platform.

If you install Mesa from source, then Mesa must be configured with option
`--with-egl-platforms=PLATFORM_LIST`, where PLATFORM_LIST is
a comman-separated list of any combination of "x11", "wayland", and "drm".

    - GLX:
        - Archlinux: pacman -S libgl libxcb libx11
        - Fedora 17: yum install mesa-libGL-devel libxcb-devel libX11-devel
        - Debian: apt-get install libgl1-mesa-dev libxcb1-dev libx11-dev

    - X11/EGL:
        - all: If you choose to install Mesa from source, use --with-egl-platforms=x11.
        - Archlinux: pacman -S libegl libxcb libx11
        - Fedora 17: yum install mesa-libEGL-devel libxcb-devel libX11-devel
        - Debian: apt-get install libegl1-mesa-dev libxcb1-dev libx11-dev

    - Wayland:
        - all: Install wayland>=1.0 from source.
        - all: Install mesa-9.1-devel from source. Use --with-egl-platforms=wayland.
        - Debian: apt-get install libwayland-dev

    - GBM:
        - all: Install mesa-9.1-devel from source. Use --with-egl-platforms=drm.
        - Archlinux: pacman -S systemd
        - Fedora 17: yum install libudev-devel
        - Debian: apt-get install libgbm-dev libudev-dev


Build and Installation
======================

0. Be in the correct directory
------------------------------

    git clone git://github.com/waffle-gl/waffle
    cd waffle

or

    tar xvf waffle-0.0.0.tar.xz
    cd waffle-0.0.0


1. Configure pkg-config
-----------------------
If any of Waffle's dependencies are installed in custom locations, you must
set the PKG_CONFIG_PATH environment variable. For example, if you installed
a dependeny into /usr/local, then:

    export PKG_CONFIG_PATH=/usr/local/share/pkgconfig:/usr/local/$libdir/pkgconfig:$PKG_CONFIG_PATH


2. Configure CMake
------------------
On Linux and Mac, running CMake with no arguments as below will configure
Waffle for a release build (optimized compiler flags and basic debug symbols)
and will auto-enable support for features whose dependencies are installed:

    cmake .

To manually control the configuration process, or to later modify the an already-configured source tree,
run one of the following:

    # An ncurses interface to CMake configuration.
    ccamke $BUILD_DIR

    # A graphical Qt interface to CMake configuration.
    cmake-gui $BUILD_DIR

    # Edit the raw configuration file.
    vim $BUILD_DIR/CMakeCache.txt

All configuration options can also be set on the command line during the
*initial* invocation of cmake. For example, to configure Waffle for a debug
build, require support for Wayland, and install into '/usr' instead of
'/usr/local', run the following:

    cmake -DCMAKE_BUILD_TYPE=Debug \
          -DCMAKE_INSTALL_PREFIX=/usr/local \
          -Dwaffle_has_wayland=1 \
          .

3. Build and Install
--------------------

    make
    make check
    make install

Calling `make check` only runs unittests that do not access the native OpenGL
platform. To run additional functional tests, which do access the native
OpenGL platform, call `make check-func`.
