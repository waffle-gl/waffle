#!/bin/bash
set -eu

case $1 in
  x11)
    platform_option=(-Dglx=enabled -Dx11_egl=enabled)
    test_wrapper=(xvfb-run -a)
    ;;
  wayland)
    platform_option=(-Dwayland=enabled)
    #TODO: find something like xvfb
    test_wrapper=(:)
    ;;
  gbm)
    platform_option=(-Dgbm=enabled)
    test_wrapper=(:)
    ;;
  surfaceless)
    platform_option=(-Dsurfaceless_egl=enabled)
    test_wrapper=(:)
    ;;
esac

rm -rf _build

set -x

meson _build \
  --wrap-mode nofallback \
  --prefix "$PWD"/install \
  --buildtype debug \
  --auto-features disabled \
  -D build-tests=true \
  -D build-manpages=true \
  -D build-htmldocs=true \
  -D build-examples=true \
  "${platform_option[@]}"

ninja -C _build -j4

# X11 connection to xvfb seems flaky; give it a second chance
"${test_wrapper[@]}" ninja -C _build test ||
"${test_wrapper[@]}" ninja -C _build test

ninja -C _build install
