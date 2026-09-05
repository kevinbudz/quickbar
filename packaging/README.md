# Distribution packaging

QuickBar is a compiled Plasma 6 applet. These recipes install:

```text
/usr/lib/qt6/plugins/plasma/applets/org.quickbar.globalmenu.so
```

**Requires Plasma 6.0+** (KF6 / Qt6). It will not build against Plasma 5 (e.g. Debian 12 / Ubuntu 22.04 LTS without a newer KDE stack).

| Distro family | Recipe | Binary package name |
|---------------|--------|---------------------|
| Arch / CachyOS | [`arch/PKGBUILD`](arch/PKGBUILD) | `plasma6-applets-quickbar` |
| Debian / Ubuntu / Mint | [`debian/`](debian/) | `quickbar` |
| Fedora / RHEL / openSUSE | [`rpm/quickbar.spec`](rpm/quickbar.spec) | `quickbar` |

Keep `pkgver` / `Version` / `changelog` in sync with `project(quickbar VERSION …)` in the root `CMakeLists.txt` and tag releases as `v1.0` on GitHub.

## Arch Linux (AUR)

1. Tag a release: `git tag v1.0 && git push origin v1.0`
2. Clone the empty AUR package (first time only):

   ```bash
   git clone ssh://aur@aur.archlinux.org/plasma6-applets-quickbar.git
   cd plasma6-applets-quickbar
   ```

3. Copy `packaging/arch/PKGBUILD` (and generate `.SRCINFO`):

   ```bash
   cp /path/to/quickbar/packaging/arch/PKGBUILD .
   updpkgsums   # fills sha256sums from the GitHub tarball
   makepkg -si  # test build and install locally
   makepkg --printsrcinfo > .SRCINFO
   git add PKGBUILD .SRCINFO
   git commit -m "Release: plasma6-applets-quickbar 0.2.1"
   git push
   ```

Users install with:

```bash
yay -S plasma6-applets-quickbar
# or: paru -S plasma6-applets-quickbar
```

After install, restart Plasma: `kquitapp6 plasmashell && plasmashell &`

See [`arch/README.md`](arch/README.md) and [`arch/AUR.md`](arch/AUR.md) for maintainer notes and AUR listing text.

CI runs [`.github/workflows/packaging.yml`](../.github/workflows/packaging.yml) on pull requests (CMake build on Arch and Fedora containers) and on `v*` tags (adds `makepkg` / `rpmbuild` and tarball URL checks).

## Debian / Ubuntu (apt)

On **Plasma 6** systems (e.g. Debian Trixie/testing, Ubuntu 24.10+, KDE Neon):

```bash
cd /path/to/quickbar
ln -sf packaging/debian debian
sudo apt install devscripts debhelper cmake extra-cmake-modules \
  qt6-base-dev qt6-declarative-dev libplasma-dev plasma-workspace-dev \
  libkf6config-dev libkf6coreaddons-dev libkf6i18n-dev \
  libkf6windowsystem-dev libkirigami-dev
debuild -b -us -uc
sudo apt install ../quickbar_*.deb
rm debian   # remove symlink if you used one
```

Package names for `-dev` libraries may differ slightly on your release; use `apt-cache search libplasma-dev` if a dependency is not found.

## Fedora / RHEL / openSUSE (dnf / zypper)

```bash
cd /path/to/quickbar
sudo dnf builddep packaging/rpm/quickbar.spec   # Fedora
# sudo zypper build-deps-install packaging/rpm/quickbar.spec   # openSUSE
rpmbuild -ba packaging/rpm/quickbar.spec \
  --define "_sourcedir $(pwd)" \
  --define "_srcrpmdir $(pwd)/packaging/rpm" \
  --define "_rpmdir $(pwd)/packaging/rpm"
sudo dnf install packaging/rpm/x86_64/quickbar-*.rpm
```

For a clean build from the release tarball, place `quickbar-1.0.tar.gz` in `~/rpmbuild/SOURCES/` (or set `_sourcedir` as above) before `rpmbuild`.

## openSUSE Build Service (optional)

The same `quickbar.spec` can be submitted to [OBS](https://build.opensuse.org/) to build for multiple distributions from one spec. Add Fedora, openSUSE, and SLE targets that ship Plasma 6.

## Post-install (all distros)

1. Remove the stock **Global Menu** widget from the panel (QuickBar uses the same `org.kde.kappmenuview` DBus service).
2. Add **QuickBar** from the widget gallery (Windows and Tasks).
3. Restart `plasmashell` if the widget does not appear.
