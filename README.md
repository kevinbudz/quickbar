# quickbar

A Plasma 6+ panel widget that shows the active application's global menu (the same DBus app-menu protocol as KDE's built-in Global Menu), with more appearance and behavior options.

## Requirements

- Plasma 6.5+ (KF6 / Qt6)
- Build dependencies: `cmake`, `extra-cmake-modules`, `gcc`, `libplasma`, `plasma-workspace` (LibTaskManager), `qt6-base`, `qt6-declarative`, `kconfig`, `kcoreaddons`, `ki18n`, `kitemmodels`, `kwindowsystem`, `kirigami` (`libdbusmenuqt` is vendored)

## Installation

### Arch (AUR)

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

### Fedora 44 / Rawhide (OBS)

```bash
sudo dnf config-manager addrepo --from-repofile=https://download.opensuse.org/repositories/home:kevinbudz/Fedora_44/home:kevinbudz.repo
sudo dnf install quickbar
```

On Rawhide, replace `Fedora_44` with `Fedora_Rawhide`.

### Debian Testing / Unstable (OBS)

```bash
curl -fsSL https://download.opensuse.org/repositories/home:kevinbudz/Debian_Testing/Release.key \
  | gpg --dearmor | sudo tee /etc/apt/trusted.gpg.d/home_kevinbudz.gpg > /dev/null
echo 'deb https://download.opensuse.org/repositories/home:kevinbudz/Debian_Testing/ /' \
  | sudo tee /etc/apt/sources.list.d/home-kevinbudz.list
sudo apt update && sudo apt install quickbar
```

On Sid, replace `Debian_Testing` with `Debian_Next`.


### From source

```bash
./install.sh
```

Then restart Plasma (`kquitapp6 plasmashell && plasmashell &`) and add QuickBar from the widget gallery. For manual builds, see [`packaging/`](packaging/).
