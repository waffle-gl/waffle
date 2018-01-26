Waffle - a library for selecting an OpenGL API and window system at runtime

# Summary

Waffle is a cross-platform library that allows one to defer selection of an
OpenGL API and of window system until runtime. For example, on Linux, Waffle
enables an application to select X11/EGL with an OpenGL 3.3 core profile,
Wayland with OpenGL ES2, and other window system / API combinations.

Waffle's immediate goal is to enable [Piglit] to test multiple OpenGL
flavors in a cross-platform way, and to allow each Piglit test to choose its
OpenGL API and window system at runtime. A future goal is to enable the
ability to record (with another tool such [APITrace]) an application's
OpenGL calls on one operating system or window system, and then replay
that trace on a different system.

For more information, visit to Waffle's website.


[Piglit]: http://piglit.freedesktop.org
[APITrace]: http://github.com/apitrace/apitrace#readme


# Links

- source:         https://gitlab.freedesktop.org/mesa/waffle.git
- gitweb:         https://gitlab.freedesktop.org/mesa/waffle
- issue-tracker:  https://gitlab.freedesktop.org/mesa/waffle/issues
- website:        http://www.waffle-gl.org
- mailing-list:   waffle@lists.freedesktop.org
- license:        BSD [http://www.opensource.org/licenses/bsd-license.php]
- maintainer:     Chad Versace <chad.versace@linux.intel.com>


# Examples

For example code and makefiles that demonstrate how to use and build with
Waffle, see the following:

    - The 'examples' directory in the source repository:
      https://gitlab.freedesktop.org/mesa/waffle/tree/master/examples

    - Installed examples at $PREFIX/share/doc/waffle-VERSION/examples, if
      Waffle is installed on your system.


# Build Requirements

Waffle has two build systems, a mature cmake build system and an new meson
build system. We recommend using meson, but if you run into problems you may
want to try cmake.

## Linux

On Linux it's recommended to install the cmake or meson package using your
distribution package manager.

    Archlinux: pacman -S cmake meson
    Fedora 17: yum install cmake meson
    Debian: apt-get install cmake meson

To build the manpages or html documentation, xsltproc and the Docbook XSL
stylesheets are required.

    Archlinux: pacman -S libxslt docbook-xsl
    Fedora 17: yum install libxslt docbook-style-xsl
    Debian: apt-get install xsltproc docbook-xsl

If you choose to enable support for a given platform (for example,
-Dwaffle_has_glx=1), then CMake will complain if the necessary libraries are
not installed. Listed below are the required packages for each platform.

If you install Mesa from source, then Mesa must be configured with option
`-D platforms=PLATFORM_LIST`, where PLATFORM_LIST is
a comma-separated list of any combination of "x11", "wayland", and "drm".

    - GLX:
        - Archlinux: pacman -S libgl libxcb libx11
        - Fedora 17: yum install mesa-libGL-devel libxcb-devel libX11-devel
        - Debian: apt-get install libgl1-mesa-dev libxcb1-dev libx11-dev

    - X11/EGL:
        - all: If you choose to install Mesa from source, use -D platforms=x11
        - Archlinux: pacman -S libegl libxcb libx11
        - Fedora 17: yum install mesa-libEGL-devel libxcb-devel libX11-devel
        - Debian: apt-get install libegl1-mesa-dev libxcb1-dev libx11-dev

    - Wayland:
        - all: Install wayland>=1.0 from source.
        - all: Install Mesa from source. Use -D platforms=wayland
        - Debian: apt-get install libwayland-dev

    - GBM:
        - all: Install Mesa from source. Use -D platforms=drm
        - Archlinux: pacman -S systemd
        - Fedora 17: yum install libudev-devel
        - Debian: apt-get install libgbm-dev libudev-dev


## Windows 

### cross-building under Linux

Make sure that CMake or meson is installed on your system.

    Archlinux: pacman -S cmake meson
    Fedora 17: yum install cmake meson
    Debian: apt-get install cmake meson

The MinGW-W64 cross-build toolchain is recommended and its CMake wrapper.

    Archlinux: aura -A mingw-w64-gcc mingw-w64-cmake (both are in the aur)
    Fedora 17: yum install FINISHME
    Debian: apt-get install FINISHME

For meson you will need a mingw cross file.


### native builds

Install Microsoft Visual Studio 2013 Update 4* or later.
Install 'Visual C++' feature.

### CMake

Download and install the latest version CMake from the official website:

    http://cmake.org/

Download OpenGL Core API and Extension Header Files.

    http://www.opengl.org/registry/#headers

Copy the header files to MSVC.

    C:\Program Files\Microsoft Visual Studio 12.0\VC\include\GL

### Meson

Download and install the latest meson from github:

    https://github.com/mesonbuild/meson/releases

OR, install python 3.x (currently 3.5 or later is supported) and install meson using pip:

    py -3 -m pip install meson

There is no need to copy header files, meson uses a bundled copy


### CYGWIN
Waffle is not tested to build under CYGWIN and is likely to be broken.
Patches addressing it are more than welcome.

For build requirements, build and installation instructions, refer to the
Linux notes in the relevant sections.


## Build and Installation

### Be in the correct directory

    git clone https://gitlab.freedesktop.org/mesa/waffle.git
    cd waffle

or

    tar xvf waffle-0.0.0.tar.xz
    cd waffle-0.0.0


### Configure pkg-config

Compiling for Windows does require any additional dependencies, as such
this step can be omitted.

If any of Waffle's dependencies are installed in custom locations, you must
set the PKG_CONFIG_PATH environment variable. For example, if you installed
a dependeny into /usr/local, then:

    export PKG_CONFIG_PATH=/usr/local/share/pkgconfig:/usr/local/$libdir/pkgconfig:$PKG_CONFIG_PATH


## Building with CMake

### Configuring

#### Linux and Mac

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

#### Windows - cross-building under Linux

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


#### Windows - native builds

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

### Build and Install

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

#### Linux and Mac

On Linux and Mac the default CMake generator is Unix Makefiles, as such we
can use an alternative version of the above commands:

    make
    make check
    make check-func
    make install
    make package


#### Windows - cross-building under Linux

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

#### Windows - native builds

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


## Building with Meson

Once meson is installed you can configure the build with meson:

    meson builddir

The default buildtype for waffle is `debugoptimzed`, which has debugging
symbols but is optimized. This is equivalent to the CMake builds `release`.
Meson's release mode does not have debugging symbols. If you wish to pass your
own flags via CFLAGS you should set buildtype to `plain`:

    meson builddir -Dbuildtype=plain -Dc_args='-03 -march=native'

__NOTE__: meson only read CFLAGS (and CXXFLAGS, etc) at initial configuration
time. If you wish to change these flags you'll need to use the meson command
line option `-D${lang}_args=` (such as `-Dc_args=`). This can be used at
configure time as well. It is _highly_ recommended that you use -Dc_args
instead of CFLAGS.

You can check configuration options by:

    meson configure builddir

Meson will be default try to find dependencies and enables targets that have
dependencies met. This behavior can be controlled using configure flags.

Meson detects compilers from the environment, rather from a toolchain file like
CMake does, the easiest way to set up MSVC is to launch a visual studio
terminal prompt, and run meson from there. Meson supports using both ninja (the
default) and msbuild on windows, to use msbuild add:

    meson builddir -Dbackend=vs

On mac there is an xcode backend for meson, but it is not very mature and is
not recommended.

To cross compile pass the location of a cross file, as described
[here](http://mesonbuild.com/Cross-compilation.html).

If you want to build tests and don't have cmocka installed, meson will download
and build cmocka for you while building the rest of waffle. This works on
windows, but due to the way that .dll files work on windows it is only
supported if you add:

    -Ddefault_library=static

Which will also build libwaffle as a static library.
