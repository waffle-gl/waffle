#!/bin/bash

set -e -x

export CMAKE_BUILD_PARALLEL_LEVEL=$(getconf _NPROCESSORS_ONLN)

build() {
  target=$1

  cmake \
    -S . \
    -B _build/$target \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_INSTALL_PREFIX=publish/$target/waffle \
    -DCMAKE_TOOLCHAIN_FILE=.gitlab-ci/$target.cmake \
    -Dwaffle_build_tests=0 \
    -Dwaffle_build_examples=1

  cmake --build _build/$target

  # XXX: Unfortunately the "package" target doesn't work for Windows, because
  # cmake/Modules/GNUInstallDirs.cmake hardcodes absolute CMAKE_INSTALL_PREFIX
  # into several CMAKE_INSTALL_* cached vars that don't get updated when
  # installing to the archive scratch directory.

  rm -rf publish/$target
  cmake --build _build/$target --target install

  pushd publish/$target
  cmake -E tar cf waffle-$target.zip --format=zip waffle
  popd
}

build mingw32
build mingw64
