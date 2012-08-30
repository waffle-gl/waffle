# Copyright 1999-2012 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=4

inherit cmake-utils

DESCRIPTION="Library that allows selection of GL API and of window system at runtime"
HOMEPAGE="http://people.freedesktop.org/~chadversary/waffle/"
SRC_URI="http://people.freedesktop.org/~chadversary/waffle/files/release/${P}/${P}.tar.xz"

LICENSE="MIT"
SLOT="0"
KEYWORDS="~amd64"
IUSE="egl gles1 gles2 wayland"

DEPEND="
	media-libs/mesa[egl?,gles1?,gles2?]
	virtual/opengl
	wayland? ( >=dev-libs/wayland-0.95.0 )
	x11-libs/libX11
	x11-libs/libxcb"
RDEPEND="${DEPEND}"

src_configure() {
	mycmakeargs=(
		-Dwaffle_has_glx=ON
		$(cmake-utils_use egl waffle_has_x11_egl)
		$(cmake-utils_use wayland waffle_has_wayland)
	)

	cmake-utils_src_configure
}
