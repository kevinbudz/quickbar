# quickbar

A Plasma 6+ panel widget that shows the active application's global menu (the same DBus app-menu protocol as KDE's built-in Global Menu), with more appearance and behavior options.

## Requirements

- Plasma 6.0+ (KF6 / Qt6)
- Build deps: `cmake`, `extra-cmake-modules`, `gcc`, `libplasma`, `plasma-workspace` (LibTaskManager), `qt6-base`, `qt6-declarative`, `kconfig`, `kcoreaddons`, `ki18n`, `kitemmodels`, `kwindowsystem`, `kirigami`

`libdbusmenuqt` is vendored from plasma-workspace (no separate package required).

## Installation

| Distro | Method | Package |
|--------|--------|---------|
| Arch / CachyOS | AUR | `plasma6-applets-quickbar` |
| openSUSE Tumbleweed | OBS (`home:kevinbudz`) | `quickbar` |
| Fedora 41 / 40 | OBS (`home:kevinbudz`) — ⚠️ build currently failing, use manual RPM build below | `quickbar` |
| Debian / Ubuntu (Plasma 6 only) | Manual `.deb` build | `quickbar` |
| Any distro | `install.sh` / CMake from source | — |

Requires Plasma 6.0+ (KF6 / Qt6). It will not build against Plasma 5 (e.g. Debian 12 / Ubuntu 22.04 LTS without a newer KDE stack).

### Arch / CachyOS (AUR)

```bash
yay -S plasma6-applets-quickbar
# or: paru -S plasma6-applets-quickbar
```

### openSUSE Tumbleweed (OBS)

```bash
sudo zypper addrepo https://download.opensuse.org/repositories/home:kevinbudz/openSUSE_Tumbleweed/home:kevinbudz.repo
sudo zypper refresh
sudo zypper install quickbar
```

### Fedora (manual RPM build)

The OBS Fedora repos (`Fedora_41`, `Fedora_40`) are set up but currently failing to build (`plasma_add_applet` unknown on F41). Until that is fixed, build locally:

```bash
cd /path/to/quickbar
sudo dnf install cmake extra-cmake-modules gcc-c++ rpm-build \
  qt6-qtbase-devel qt6-qtdeclarative-devel libplasma-devel plasma-workspace-devel \
  kf6-kconfig-devel kf6-kcoreaddons-devel kf6-ki18n-devel \
  kf6-kitemmodels-devel kf6-kwindowsystem-devel kf6-kirigami-devel \
  libX11-devel libXtst-devel
mkdir -p ~/rpmbuild/SOURCES
tar -czf ~/rpmbuild/SOURCES/quickbar-1.0.tar.gz \
  --transform "s,^,quickbar-1.0/," --exclude=build --exclude=.git .
rpmbuild -ba packaging/rpm/quickbar.spec
sudo dnf install ~/rpmbuild/RPMS/x86_64/quickbar-*.rpm
```

Once OBS is green, the repo install will be:

```bash
sudo dnf config-manager addrepo --from-repofile=https://download.opensuse.org/repositories/home:kevinbudz/Fedora_41/home:kevinbudz.repo
sudo dnf install quickbar
```

### Debian / Ubuntu (manual `.deb` build)

On **Plasma 6** systems only (e.g. Debian Trixie/testing, Ubuntu 24.10+, KDE Neon). There is no APT repo yet — build locally:

```bash
cd /path/to/quickbar
ln -sf packaging/debian debian
sudo apt install devscripts debhelper cmake extra-cmake-modules \
  qt6-base-dev qt6-declarative-dev libplasma-dev plasma-workspace-dev \
  libkf6config-dev libkf6coreaddons-dev libkf6i18n-dev libkf6itemmodels-dev \
  libkf6windowsystem-dev libkirigami-dev libx11-dev libxtst-dev
debuild -b -us -uc
sudo apt install ../quickbar_*.deb
rm debian   # remove symlink if you used one
```

Package names may differ slightly per release; use `apt-cache search libplasma-dev` if a dependency is not found.


## From source (any distro)

From a git checkout, `./install.sh` detects your distro (Arch, Debian/Ubuntu, Fedora, openSUSE) and installs any missing build dependencies before compiling:

```bash
./install.sh
```

Manual build:

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
```

Then restart Plasma or run:

```bash
kquitapp6 plasmashell && plasmashell &
```

Add quickbar from the widget gallery (category: Windows and Tasks).
