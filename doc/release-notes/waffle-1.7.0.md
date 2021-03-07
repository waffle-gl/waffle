# Waffle 1.7.0 Release Notes

## New Features

 * Wayland: Support for the xdg-shell protocol.

   The protocol has been stable for years, and supersedes wl-shell.
   Xdg-shell is used when available and wl-shell our fallback.
   Note that wl-shell is planned for removal with waffle 2.0.0.

 * Surfaceless: Implement window resize

 * GLX/WGL: Behave correctly in the presence of ARB_create_context

   Requesting certain versions, while the extension is present, was
   was resulting in incorrect GL/GLES profile selection.

 * Tests: Rework and extend test suite

   Massive overhall of the test suite, adding tests for profile and
   flags combinations, catching issues like the GLX/WGL above.

 * cmake: Bump requirement to 2.8.12

 * man: Spelling and associated fixes.

 * GBM: Pass valid arguments to gbm_surface_create_with_modifiers

 * Apple: Build fixes

   Remove invalid librt and use correct include (case-sensitive) folder.


## Contributors since 1.6.0

Adrian Negreanu (1):
      WaffleDefineCompilerFlags: FindPkgConfig variables are lists

Alexandros Frantzis (1):
      surfaceless_egl: Implement resize functionality

Alexey Brodkin (2):
      tests/gl_basic_test: s/GBM/SURFACELESS_EGL/ typo
      tests/gl_basic_test: fix SURFACELESS_EGL only build

Dylan Baker (6):
      Version: bump version for 1.6.90 for development
      Merge branch 'release-notes' into 'master'
      move include/waffle to include/waffle-1
      meson: Generate cmake config and version files if meson is new enough
      meson: Separate tests into suites
      meson: remove completed todo comments

Emil Velikov (46):
      gitlab-ci: bump xvfb wait time
      glx: correctly handle ARB_create_context lack/presence
      wgl: correctly handle ARB_create_context lack/presence
      func: add context API tests
      func: add profile tests
      func: actually check the GL* version created
      func: add robust tests
      func: add more debug tests
      egl: drop misleading always true asserts
      glx: drop misleading always true asserts
      wgl: drop misleading always true asserts
      glx: fold GLES* extension checking
      wgl: fold GLES* extension checking
      wayland: bump required version to 1.10
      wayland: rework extern symbols handling
      func: print waffle error message as applicable
      tests/gl_basic_test: remove "custom" CGL tests
      tests/gl_basic_test: reshuffle/group tests
      tests/gl_basic_test: add/use better assert macros
      tests/gl_basic_test: add GL_CONTEXT_PROFILE_MASK check
      tests/gl_basic_test: add compat/fwdcompat workaround
      apple: hide GL deprecation warnings
      tests/gl_basic_test: add apple/core context workaround
      tests/gl_basic_test: complete and document test coverage
      wflinfo: use glGetStringi() only when available
      meson: do not replace datadir for bash completion
      meson: always install bash-completion file
      docs: Add waffle 1.6.1 release notes
      docs: Add waffle 1.6.2 release notes
      tests: meson: remove host_machine.system() guard
      tests: enable all the tests
      gitlab-ci: add surfaceless to the cmake build
      meson: fixup and group dependencies per platform
      meson: use include/compile_args dependencies
      docs: Add waffle 1.6.3 release notes
      release: Publish Waffle 1.6.2
      release: Publish Waffle 1.6.3
      wayland: Generate the core wl_interface symbols
      meson, cmake: wire build support for wayland xdg-shell
      wayland: Add support for xdg-shell stable
      android: meson: dump API version
      wayland: annotate the new/old waffle/wayland API
      .gitlab-ci.yml: use cmake + make
      cmake: bump minimum cmake version to 2.8.12
      basic: add SL supports
      waffle: Bump version to 1.7.0

Eric Engestrom (3):
      gl_basic_test: avoid reading uninitialised platform
      gitlab-ci: introduce pre-merge CI
      meson: use github URL for wraps instead of completely unreliable wrapdb

Jesse Natalie (1):
      meson: Fix meson build if bash-completion not found

Jordan Justen (6):
      .gitlab-ci.yml: Build website from master branch www directory
      doc: Add release-process.txt
      meson: Build third_party/threads with hidden symbols
      man: Add refpurpose on waffle_enum page
      man: Fix spelling, successfull => successful
      wflinfo: Fix spelling, Succesfully => Successfully

Kenneth Graunke (1):
      CMake: Install bash completions into prefix

Marek Olšák (1):
      gbm: don't pass invalid arguments to gbm_surface_create_with_modifiers

Rafael Antognolli (1):
      meson: Redefine 'datadir' instead of 'prefix'.

Simon Zeni (3):
      gitlab-ci: update to latest ci-template
      wayland: handle fullscreen and maximize
      meson: remove explicit python search

Yurii Kolesnykov (2):
      cmocka: don't require librt, when it's not unavailable
      apple: use correct include folder s/Appkit/AppKit/
