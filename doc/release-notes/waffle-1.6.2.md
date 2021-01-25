# Waffle 1.6.2 Release Notes

## Fixes since 1.6.1

* apple: Silence deprecation warnings
* apple: cmocka: include librt only when present

* gbm: pass valid arguments to gbm_surface_create_with_modifiers

* wflinfo: window: use glGetStringi() when available
* wflinfo: meson: Install bash-completion file to correct location
* wflinfo: meson: Always install bash-completion file


## Features since 1.6.1

None.


## Changes since 1.6.1

Alexey Brodkin (1):
      tests/gl_basic_test: fix SURFACELESS_EGL only build

Emil Velikov (4):
      apple: hide GL deprecation warnings
      wflinfo: use glGetStringi() only when available
      meson: do not replace datadir for bash completion
      meson: always install bash-completion file

Jesse Natalie (1):
      meson: Fix meson build if bash-completion not found

Marek Olšák (1):
      gbm: don't pass invalid arguments to gbm_surface_create_with_modifiers

Simon Zeni (1):
      gitlab-ci: update to latest ci-template

Yurii Kolesnykov (1):
      cmocka: don't require librt, when it's not unavailable

