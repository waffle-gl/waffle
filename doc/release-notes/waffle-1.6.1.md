# Waffle 1.6.1 Release Notes

## Fixes since 1.6.0

* wflinfo: Install bash completion file correctly

* GLX/WGL: Correctly handle ARB_create_context

* Wayland: Require wayland-client v1.10
* Wayland: Resolve build issues with -fno-common, default with GCC10

* Tests: Add new tests - context API, profile, robustness, debug

* GitLab-CI: Introduction and small fixes


## Features since 1.6.0

All new features introduced in minor releases are *minor* and introduce no
instability.

* surfaceless_egl: Implement resize functionality
  Add support for the waffle_window_resize() function when using the EGL
  surfaceless platform. This is achieved by recreating the backing pbuffer
  with the new size, and rebinding it to the current context.


## Changes since 1.6.0

Adrian Negreanu (1):
      WaffleDefineCompilerFlags: FindPkgConfig variables are lists

Alexandros Frantzis (1):
      surfaceless_egl: Implement resize functionality

Emil Velikov (10):
      gitlab-ci: bump xvfb wait time
      glx: correctly handle ARB_create_context lack/presence
      wgl: correctly handle ARB_create_context lack/presence
      func: add context API tests
      func: add profile tests
      func: actually check the GL* version created
      func: add robust tests
      func: add more debug tests
      wayland: bump required version to 1.10
      wayland: rework extern symbols handling

Eric Engestrom (2):
      gl_basic_test: avoid reading uninitialised platform
      gitlab-ci: introduce pre-merge CI

Kenneth Graunke (1):
      CMake: Install bash completions into prefix

Rafael Antognolli (1):
      meson: Redefine 'datadir' instead of 'prefix'.
