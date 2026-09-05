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

On Rawhide swap `Fedora_44` for `Fedora_Rawhide`.

### Debian Testing / Unstable (OBS)

```bash
curl -fsSL https://download.opensuse.org/repositories/home:kevinbudz/Debian_Testing/Release.key \
  | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/home_kevinbudz.gpg > /dev/null
echo 'deb https://download.opensuse.org/repositories/home:kevinbudz/Debian_Testing/ /' \
  | sudo tee /etc/apt/sources.list.d/home-kevinbudz.list
sudo apt update && sudo apt install quickbar
```

On Sid swap `Debian_Testing` for `Debian_Next`.


## From source

```bash
./install.sh
```

Then restart Plasma (`kquitapp6 plasmashell && plasmashell &`) and add QuickBar from the widget gallery. Manual builds: see [`packaging/`](packaging/).
