# Maintainer: Kevin Budz <https://github.com/kevinbudz>
pkgname=plasma6-applets-quickbar
pkgver=1.0
pkgrel=1
pkgdesc="Configurable Plasma 6 global menu panel widget (QuickBar)"
arch=('x86_64' 'aarch64')
url="https://github.com/kevinbudz/quickbar"
license=('GPL-2.0-or-later')
depends=(
    'libplasma'
    'plasma-workspace'
    'qt6-base'
    'qt6-declarative'
    'kconfig'
    'kcoreaddons'
    'ki18n'
    'kwindowsystem'
    'kirigami'
)
makedepends=(
    'cmake'
    'extra-cmake-modules'
    'gcc'
    'kitemmodels'
    'libx11'
    'libxtst'
)
optdepends=(
    'dolphin: Finder-style desktop menu (Dolphin menu bar on desktop)'
)
source=("$url/archive/refs/tags/v$pkgver.tar.gz")
sha256sums=('acdf80e6a56c4e5afa429503c964ae761a9494b57d36068048b50884a46a571b')

build() {
    cmake -B build -S "quickbar-$pkgver" \
        -DCMAKE_BUILD_TYPE=None \
        -DCMAKE_INSTALL_PREFIX=/usr
    cmake --build build
}

package() {
    DESTDIR="$pkgdir" cmake --install build
}
