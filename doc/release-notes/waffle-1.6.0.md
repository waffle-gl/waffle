# Waffle 1.6.0 Release Notes

## New Features

* Meson: A new meson build system [Dylan Baker]

  Dylan has added a meson based build system. The plan is to keep
  CMake for a couple of releases and then migrate
  
* wflinfo can output JSON [Dylan Baker]

* FreeBSD: out of the box build with meson [Dylan Baker]
  
* EGL/GBM: Add support for modifiers [Jason Ekstrand]
  
* Unit test improvements [Emil Velikov]

* nacl toolchain support for cmake [Tapani Pälli]

## Contributors since 1.5.0

Andrey Rakhmatullin (2):
      debian: Add Build-Depends: libudev-dev.
      debian: Release 1.5.2-2.1.

Bas Nieuwenhuizen (7):
      wcore: Add support for robust access contexts
      glx: Add support for robust access contexts
      wgl: Add support for robust access contexts
      egl: Add support for robust access contexts
      cgl: Add an error when trying to use a robust access context
      nacl: Add an error when trying to use a robust access context
      egl: Support robust access contexts with EGL 1.5.

Brian Paul (1):
      waffle: add waffle_get_current_*() functions to waffle.def.in

Chad Versace (74):
      waffle: Bump post-release version to 1.5.90
      android: Bump waffle_api_version to 1.6
      pkg/archlinux: Move to new repository
      Merge branch 'maint-1.5'
      Merge branch 'tpalli/nacl' into master
      Merge branch 'maint-1.5' into master
      include: Define macro WAFFLE_DEPRECATED_1_06
      include: Deprecate waffle_attrib_list funcs
      core: Rename functions s/wcore_attrib_list/wcore_attrib_list32/
      core: Change return type of wcore_attrib_list32_length
      core: Define wcore_attrib_list funcs with type-generic template
      core: Define intptr_t variants of wcore_attrib_list functions
      core: Add arithmetic functions that detect overflow
      core: Add func wcore_attrib_list_from_int32
      waffle: Fix mismatch in waffle_window_create prototype
      waffle: Fix signature of wcore_platform::window::create()
      core: Add func wcore_error_bad_attribute
      core: Add attrib_list param to func wcore_platform::window::create
      core: Add func wcore_attrib_list_copy()
      core: Add func wcore_attrib_list_pop()
      waffle: Add public func waffle_window_create2()
      tests/gl_basic: Update to use waffle_window_create2()
      examples/gl_basic: Update to use waffle_window_create2()
      Merge branch 'chadv/waffle_window_create2' into master
      Merge branch 'maint-1.5' into master
      Merge branch 'maint-1.5' into master
      Merge branch 'evelikov/android-fixes'
      Merge branch 'evelikov/waffle-teardown'
      Merge branch 'tpalli/nacl'
      Merge branch 'maint-1.5'
      Merge branch 'maint-1.5'
      Merge branch 'maint-1.5'
      .gitignore: Ignore all generated html manpages
      Merge branch 'maint-1.5' into master
      Merge branch 'evelikov/no-libwayland'
      Merge branch 'fjhenigman/gl-basic-alpha-planes'
      Merge branch 'fjhenigman/platform_null'
      Merge branch 'evelikov/for-chad/nacl-fixes'
      Merge branch 'maint-1.5' into master
      examples/gl_basic: Add --fullscreen option
      gbm: Allow user to provide device path in environment
      Merge branch 'maint-1.5'
      wflinfo: Add func error_oom()
      wflinfo: Minor formatting fixes
      wflinfo: Document new --format option in manpage
      wflinfo: Clarify in usage text which options have parameters
      gl_basic: Die if glGetIntegerv fails
      gl_basic: Add a --robust option
      api: Don't guard enum names with WAFFLE_API_VERSION
      man: Don't list all enums in manpage waffle_enum.3
      tests/gl_basic_test: Query context flags also for GLES 3.2
      api: Add functions waffle_get_current_OBJECT()
      tests/gl_basic_test: Sanity-check waffle_get_current_* functions
      egl: Improve support for robust GLES contexts on EGL 1.5
      .gitignore: Ignore CMocka's config.h
      tests/gl_basic: Make GL symbol queries more robust
      examples/gl_basic: Make GL symbol queries more robust
      egl: Let wegl_platform override EGL_SURFACE_TYPE
      egl: Define EGL_PLATFORM_* enums
      egl: Update wegl_platform_init signature
      egl: Move each platform's setenv("EGL_PLATFORM") into core EGL code (v2)
      egl: Query client extensions string
      egl: Optionally query for eglGetPlatformDisplay (v2)
      egl: Update wegl_display_init signature
      egl: Use eglGetPlatformDisplay when possible (v2)
      egl: Use eglGetPlatformDisplayEXT as a fallback
      egl: Rename files wegl_window.* -> wegl_surface.*
      egl: Rename struct wegl_window -> wegl_surface
      egl: Allow pbuffers to back wegl_surface
      cmake: Add option waffle_has_surfaceless_egl (v2)
      surfaceless_egl: Implement new platform (v2)
      wflinfo: Support --platform=surfaceless_egl
      tests/gl_basic_test: Support platform surfaceless_egl
      cmake: Drop condition expresion in 'else' statement

Dylan Baker (17):
      cmake: remove FindWaffle.cmake
      wflinfo: Split out flags struct
      wflinfo: Use ARRAY_SIZE macro
      wflinfo: Split version, renderer, and vendor checks
      wflinfo: Add a --format flag that prints json or the classic output
      rename readme to markdown
      CMake: Fix install of README after rename
      examples: remove double const qualifier
      import wglext.h
      README: use # style headers
      README: drop numbers from headers
      Initial meson build system
      meson.build: use elif instead of if
      meson: fix installation of man files
      waffle/gbm: remove linux/input.h
      meson: make udev required for gbm
      meson: Fix build on FreeBSD

Emil Velikov (81):
      android: compilation fix
      android: include gl_basic & wflinfo to the build
      android: export only the required functions
      android: silence compiler unused-parameter warnings
      waffle: add public func waffle_teardown()
      examples: make use of waffle_teardown
      wflinfo: use waffle_teardown
      wcore: silence compiler unused-parameter warnings
      egl: silence compiler unused-parameter warnings
      cmake: update minimum required MSVC version to 2013 Update 4
      man: fix waffle_teardown description
      wayland: fix indentation
      wayland: fetch wl_egl_* function pointers at wayland_platform_create
      wayland: do not link against wayland-egl
      wayland: resolve wayland-client symbols at runtime (via libdl)
      third_party/threads: Use PTHREAD_MUTEX_RECURSIVE by default
      wcore: rework non-compatible mutex initialization
      third_party/threads: remove the _MTX_INITIALIZER_NP hack
      api/core: annotate structs/functions for C linkage
      nacl: move dlfcn.h inclusion to where it's needed
      nacl: move header inclusion outside of the extern "C" block
      nacl: rework nacl_dl functions
      nacl: emit errors when things fail
      nacl: use nacl_container prefix on relevant functions
      nacl: untangle header inclusions
      nacl: add missing pragma once guards
      android: remove no longer needed extern "C"
      android: add missing pragma once guard
      cgl,nacl: print the dlerror() when dlopen() fails
      glx: Provide our own GLX_ARB_create_context_profile defines
      wcore: Do not use get_with_default() when default_value is the same as value
      glx: Remove dl_can_open restriction for supports_context_api()
      tests/gl_basic_test: query context flags only for desktop GL 3.0 or later
      glx: Don't use ARB_create_context with pre 3.2 contexts
      wgl: Don't use ARB_create_context with pre 3.2 contexts
      gbm: don't fetch gbm_bo_* symbols from libgbm
      tests/gl_basic: introduce --platform param
      tests/gl_basic: fold ifndef _WIN32 blocks
      tests/gl_basic: use actual/expect arrays for the pixel storage
      tests/gl_basic: add test_XX_rgb{,a} macro
      tests/gl_basic: add test_glXX() macro
      tests/gl_basic: add test_glXX_fwdcompat() macro
      tests/gl_basic: add test_glXX_core() test macro
      tests/gl_basic: add test_glXX_core_fwdcompat() test macro
      tests/gl_basic: add test_glXX_compat() macro
      tests/gl_basic: add test_glesXX() macro
      tests/gl_basic: add test_XX_fwdcompat() macro
      tests/gl_basic: add test_gl_debug() macro
      cmocka: update to cmocka-1.0.1-ab3ec0da8c5
      UPSTREAM: cmocka: include strings.h for strcasecmp
      core: convert unittests to new cmocka API
      core: wcore_error_reset() after calloc()
      tests/gl_basic: disable tests, convert to cmocka step 1
      waffle_test: bye bye
      tests/gl_basic: setup CREATE_TESTSUITE macro and enable glx
      tests/gl_basic: enable wayland
      tests/gl_basic: enable x11_egl
      tests/gl_basic: enable wgl
      tests/gl_basic: reshuffle the cgl tests
      tests/gl_basic: enable cgl
      tests/gl_basic: add support for GBM platform
      tests/gl_basic: call waffle_error_get_code() less often
      wgl: remove unneeded asserts
      wgl: indent with spaces, not tabs
      glx: remove unneeded asserts
      wcore: don't parse/validate the exact major/minor version
      wcore: indent with spaces, not tabs
      wegl: add closing bracket in error message
      wegl: untangle dl_can_open() and support_api()
      glx: remove dl_can_open() logic from supports_api()
      waffle: replace wcore_error_internal with assert()
      cgl: fold all ES handling in supports_api()
      linux: replace wcore_error_internal with assert
      linux: cosmetic/coding style fixes
      linux: return NULL for libGL.so on Android
      egl: resolve build issue with pre EGL 1.5 headers
      Revert "wegl: add EGL image create/destroy"
      gbm: coding style cleanup
      egl: fold eglGetConfigAttrib() within wegl_config_choose()
      gbm: factor out gbm_window_{init,teardown}
      gbm: implement window_resize

Eric Engestrom (5):
      wflinfo: add bash completion
      README: update Mesa build instructions
      README: update links to point to gitlab.fdo
      README: fix links list
      adjust a few URLs to the new GitLab home

Frank Henigman (11):
      gl_basic: request alpha planes
      wegl: enable deriving from wegl_context (v2)
      gbm: make platform friendlier to derived classes
      gbm: make wgbm_get_default_fd_for_pattern public
      wegl: fix wegl_util.h includes and declarations
      wegl: add EGL image create/destroy
      gbm: wegl_display ok in wgbm_config_get_gbm_format
      waffle: add full screen window request
      core: store platform type in wcore_platform
      core: store context API in wcore_context
      core: wrap realloc() and strdup()

Jason Ekstrand (4):
      gbm: Allow for optional libgbm symbols
      egl: Add a macro for extension checking
      egl: Add support for EGL_EXT_image_dma_buf_import_modifiers
      gbm: Use modifiers to create the surface when available

Jordan Justen (19):
      Merge branch 'maint-1.4'
      Merge branch 'maint-1.5' for debian changelog
      Merge branch 'maint-1.5'
      Merge branch 'maint-1.5'
      debian: Release 1.5.2-1
      Merge branch 'maint-1.5'
      debian: Attempt to fix formatting of waffle-utils description
      debian: Fix package descriptions
      debian: Release 1.5.2-2
      Merge branch 'maint-1.5'
      Merge tag 'debian/1.5.2-2.1' into maint-1.5
      Merge branch 'maint-1.5'
      debian: Add watch file
      debian: Upgrade Standards-Version to 3.9.8
      debian: Release 1.5.2-3
      Merge tag 'debian/1.5.2-3' into maint-1.5
      Merge branch 'maint-1.5'
      debian: Remove debian from upstream waffle tree
      README.md: Fix footnote formatting for GitLab markdown

Michael M (2):
      third_party/threads: allow building for macOS
      tests/gl_basic_test: copy definition of removeXcodeArgs

Michel Dänzer (2):
      linux: Use full libGLES*.so.[12] SONAMEs
      wayland: Wrap wl_proxy_marshal_constructor_versioned (v3)

Mircea Gherzan (1):
      cmake: use the EGL CFLAGS in the waffle CFLAGS

Tapani Pälli (13):
      examples: add waffle_has_x11_egl check for simple-x11-egl
      waffle: add support for building Waffle using NaCl toolchain
      waffle: initial empty implementation of nacl backend
      waffle: update man pages with nacl changes
      waffle: make gl_basic to work with nacl
      nacl: add attrib_list parameter to create_window
      nacl: add supports_context_api implementation
      nacl: add implementation for waffle_config_choose
      nacl: add implementation for waffle_context_create
      nacl: add implementation for window create and resize
      nacl: add implementation for waffle_make_current
      nacl: add implementation for waffle_window_swap_buffers
      nacl: add implementation for waffle_dl_sym

