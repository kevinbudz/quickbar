# quickbar

A Plasma 6+ panel widget that shows the active application's global menu (the same DBus app-menu protocol as KDE's built-in Global Menu), with more appearance and behavior options.

## Requirements

- Plasma 6.5+ (KF6 / Qt6) — `plasma_add_applet()` needs libplasma ≥ 6.3, the `org.kde.plasma.configuration` QML module needs ≥ 6.5
- Build deps: `cmake`, `extra-cmake-modules`, `gcc`, `libplasma`, `plasma-workspace` (LibTaskManager), `qt6-base`, `qt6-declarative`, `kconfig`, `kcoreaddons`, `ki18n`, `kitemmodels`, `kwindowsystem`, `kirigami`

`libdbusmenuqt` is vendored from plasma-workspace (no separate package required).

## Installation

| Distro | Method | Package |
|--------|--------|---------|
| Arch / CachyOS | AUR | `plasma6-applets-quickbar` |
| openSUSE Tumbleweed | OBS (`home:kevinbudz`) | `quickbar` |
| Fedora 44 / Rawhide | OBS (`home:kevinbudz`) | `quickbar` |
| Debian Testing / Unstable | OBS (`home:kevinbudz`) | `quickbar` |
| Other distros (Plasma 6.5+) | Manual build | `quickbar` |
| Any distro | `install.sh` / CMake from source | — |

Requires Plasma 6.5+ (KF6 / Qt6). Older stacks (e.g. Fedora ≤ 43's Plasma ≤ 6.4, Debian 13 Trixie / Ubuntu ≤ 25.10's Plasma ≤ 6.4) fail configure with a clear message.

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

### Fedora (OBS)

```bash
sudo dnf config-manager addrepo --from-repofile=https://download.opensuse.org/repositories/home:kevinbudz/Fedora_44/home:kevinbudz.repo
sudo dnf install quickbar
```

(Use `Fedora_Rawhide` in the URL on Rawhide. `Fedora_43` was dropped — it ships Plasma 6.4.5, below the required 6.5. If you are on an older Fedora, build the RPM locally:)

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

### Debian Testing / Unstable (OBS)

On **Debian Testing (Forky) / Unstable (Sid)** with Plasma 6.5+ (currently 6.7):

```bash
# Add repo key
curl -fsSL https://download.opensuse.org/repositories/home:kevinbudz/Debian_Testing/Release.key \
  | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/home_kevinbudz.gpg > /dev/null
# Add repo (use Debian_Next for Unstable/Sid)
echo 'deb https://download.opensuse.org/repositories/home:kevinbudz/Debian_Testing/ /' \
  | sudo tee /etc/apt/sources.list.d/home-kevinbudz.list
sudo apt update
sudo apt install quickbar
```

### Debian / Ubuntu (manual `.deb` build)

On **Plasma 6.5+** systems only (e.g. Debian Testing/Unstable, current KDE Neon). Debian 13 Trixie stock (Plasma 6.3) and Ubuntu ≤ 25.10 (Plasma ≤ 6.4) ship older Plasma and are unsupported — the build fails early via `libplasma-dev (>= 6.5)`. To build locally:

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
