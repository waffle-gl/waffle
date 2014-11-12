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

Waffle uses CMake for its build system.

Linux
-----
On Linux it's recommended to install the cmake package using your
distribution package manager.

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


Windows - cross-building under Linux
------------------------------------
Make sure that CMake is installed on your system.

    Archlinux: pacman -S cmake
    Fedora 17: yum install cmake
    Debian: apt-get install cmake

The MinGW-W64 cross-build toolchain is recommended and its CMake wrapper.

    Archlinux: pacman -S mingw-w64-gcc mingw-w64-cmake (latter is in AUR)
    Fedora 17: yum install FINISHME
    Debian: apt-get install FINISHME


Windows - native builds
-----------------------
Download and install the latest version CMake from the official website:

    http://cmake.org/

Install Microsoft Visual Studio 2013* or later.
Install 'Visual C++' feature.

Download OpenGL Core API and Extension Header Files.

    http://www.opengl.org/registry/#headers

Copy the header files to MSVC.

    C:\Program Files\Microsoft Visual Studio 12.0\VC\include\GL


[*] Waffle heavily requires on features introduced by the C99 standard. As
such only reasonable compiler (at the time of writing) from the Microsoft
Visual Compiler series is MSVC 2013. Building with older versions is likely
to be broken.

Windows - CYGWIN
----------------
Waffle is not tested to build under CYGWIN and is likely to be broken.
Patches addressing it are more than welcome.

For build requirements, build and installation instructions, refer to the
Linux notes in the relevant sections.


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
Compiling for Windows does require any additional dependencies, as such
this step can be omitted.

If any of Waffle's dependencies are installed in custom locations, you must
set the PKG_CONFIG_PATH environment variable. For example, if you installed
a dependeny into /usr/local, then:

    export PKG_CONFIG_PATH=/usr/local/share/pkgconfig:/usr/local/$libdir/pkgconfig:$PKG_CONFIG_PATH


2. Configure CMake
------------------

2.1 Linux and Mac
-----------------
On Linux and Mac, running CMake with no arguments as below will configure
Waffle for a release build (optimized compiler flags and basic debug symbols)
and will auto-enable support for features whose dependencies are installed:

    cmake .

To manually control the configuration process, or to later modify the an
already-configured source tree, run one of the following:

    # An ncurses interface to CMake configuration.
    ccmake .

    # A graphical Qt interface to CMake configuration.
    cmake-gui .

    # Edit the raw configuration file.
    vim CMakeCache.txt

All configuration options can also be set on the command line during the
*initial* invocation of cmake. For example, to configure Waffle for a debug
build, require support for Wayland, and install into '/usr' instead of
'/usr/local', run the following:

    cmake -DCMAKE_BUILD_TYPE=Debug \
          -DCMAKE_INSTALL_PREFIX=/usr \
          -Dwaffle_has_wayland=1 \
          .

2.2 Windows - cross-building under Linux
----------------------------------------
The following sh snippet can be used to ease the configuration process.

    _architectures="i686-w64-mingw32 x86_64-w64-mingw32"
    unset LDFLAGS
    for _arch in ${_architectures}; do
      _install_prefix=/usr/${_arch}
      mkdir -p build-${_arch} && pushd build-${_arch}
      ${_arch}-cmake .. \
        -DCMAKE_INSTALL_PREFIX=${_install_prefix} \
        -DCMAKE_INSTALL_LIBDIR=${_install_prefix}/lib \
        \
        -Dwaffle_build_tests=0 \
        -Dwaffle_build_examples=1
      make
      popd
    done

Make sure to adjust _install_prefix to "" if the resulting library will
not be used for further cross-building.


2.3 Windows - native builds
---------------------------

For native Windows builds, one must provide a generator argument and
optionally a toolset if the resulting library must be compatible with
Windows XP. When the resulting library is to be 64bit "Win64" needs to be
appended to the generator argument.

    @echo Configuring Waffle as Windows Vista compatible 32bit library
    cmake -G "Visual Studio 12" -H%CD% -B%CD%\build\msvc32 -DCMAKE_INSTALL_PREFIX=""

    @echo Configuring Waffle as Windows Vista compatible 64bit library
    cmake -G "Visual Studio 12 Win64" -H%CD% -B%CD%\build\msvc64 -DCMAKE_INSTALL_PREFIX=""

    @echo Configuring Waffle as Windows XP compatible 32bit library
    cmake -G "Visual Studio 12" -T "v120_xp" -H%CD% -B%CD%\build\msvc32 -DCMAKE_INSTALL_PREFIX=""

For alternative control of the configuration process, or to later modify an
already-configured source tree, run the graphical Qt interface via:

    cmake-gui

3. Build and Install
--------------------
The following commands build Waffle, run its tests, installs the project and
creates a binary archive in a platform agnostic way.

Note that not all steps may be required in your case and the configuration
settings (cache) are located in the current directory as indicated by ".".

    cmake --build .
    cmake --build . --target check
    cmake --build . --target check-func
    cmake --build . --target install
    cpack

Calling `cmake ... check` only runs unittests that do not access the native
OpenGL platform. To run additional functional tests, which do access the
native OpenGL platform, call `cmake ... check-func`.

3.1 Linux and Mac
-----------------
On Linux and Mac the default CMake generator is Unix Makefiles, as such we
can use an alternative version of the above commands:

    make
    make check
    make check-func
    make install
    make package


3.2 Windows - cross-building under Linux
----------------------------------------

    _architectures="i686-w64-mingw32 x86_64-w64-mingw32"
    unset LDFLAGS
    for _arch in ${_architectures}; do
      pushd build-${_arch}
      make
      make install
      make package
      popd
    done

Note: Running the tests (`make check` and/or `make check-func`) is not tested
but may work if the appropriate environment is setup via wine.


3.3 Windows - native builds
---------------------------
One can manage the build/install/package process via Visual Studio's GUI
or via the command line.

When using the GUI open .\build\msvc*\waffle-VERSION.sln, where * can be
either 32 or 64 depending on your requirements.

If building via the command line, navigate to the correct folder and invoke
the desired command as outlined in `Section 3. Build and Install`

For example the following will build 32bit Waffle and will package/archive
it into a file called waffle1-VERSION-win32.zip.

    @echo Preparing to build 32 bit version of waffle
    cd .\build\msvc32
    cmake --build .
    cpack
