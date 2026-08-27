#!/usr/bin/env bash
# Build and install QuickBar. Detects the distro and installs missing build deps.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
prefix="${PREFIX:-/usr}"
build_dir="${BUILD_DIR:-build}"
skip_deps=false

usage() {
    cat <<'EOF'
Usage: ./install.sh [OPTIONS] [PREFIX]

Build and install QuickBar (Plasma 6 applet).

Options:
  --no-deps     Skip automatic dependency installation
  -h, --help    Show this help

Environment:
  PREFIX        Install prefix (default: /usr)
  BUILD_DIR     CMake build directory (default: build)
  SKIP_DEPS=1   Same as --no-deps

Examples:
  ./install.sh                  # install to /usr (uses sudo)
  ./install.sh ~/.local         # user-local install (no sudo)
  ./install.sh --no-deps        # build only; deps must already be installed

Requires Plasma 6.0+ (KF6 / Qt6). See README.md for supported distributions.
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --no-deps)
            skip_deps=true
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        /*|./*|~/*)
            prefix="$1"
            shift
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage >&2
            exit 1
            ;;
    esac
done

if [[ "${SKIP_DEPS:-}" == 1 ]]; then
    skip_deps=true
fi

detect_distro() {
    if [[ ! -f /etc/os-release ]]; then
        echo "unknown"
        return
    fi
    # shellcheck source=/dev/null
    source /etc/os-release
    local id="${ID,,}"
    local id_like="${ID_LIKE:-}"

    case "$id" in
        arch|cachyos|manjaro|endeavouros|garuda|artix)
            echo arch
            ;;
        debian|ubuntu|linuxmint|pop|popos|neon|elementary|zorin|kubuntu|xubuntu|lubuntu|mint)
            echo debian
            ;;
        fedora|nobara|ultramarine|azurelinux)
            echo fedora
            ;;
        rhel|centos|rocky|almalinux|mariner)
            echo fedora
            ;;
        opensuse*|sles|sle*)
            echo opensuse
            ;;
        *)
            if [[ "$id_like" == *arch* ]]; then
                echo arch
            elif [[ "$id_like" == *debian* || "$id_like" == *ubuntu* ]]; then
                echo debian
            elif [[ "$id_like" == *fedora* || "$id_like" == *rhel* ]]; then
                echo fedora
            elif [[ "$id_like" == *suse* ]]; then
                echo opensuse
            else
                echo unknown
            fi
            ;;
    esac
}

run_privileged() {
    if [[ "${EUID:-$(id -u)}" -eq 0 ]]; then
        "$@"
    elif command -v sudo >/dev/null 2>&1; then
        sudo "$@"
    else
        echo "Need root privileges (or sudo) to install packages." >&2
        exit 1
    fi
}

pkg_installed_pacman() {
    pacman -Q "$1" &>/dev/null
}

pkg_installed_dpkg() {
    dpkg-query -W -f='${Status}' "$1" 2>/dev/null | grep -q 'install ok installed'
}

pkg_installed_rpm() {
    rpm -q "$1" &>/dev/null
}

install_deps_arch() {
    local packages=(
        base-devel
        cmake
        extra-cmake-modules
        gcc
        libplasma
        plasma-workspace
        qt6-base
        qt6-declarative
        kconfig
        kcoreaddons
        ki18n
        kitemmodels
        kwindowsystem
        kirigami
        libx11
        libxtst
    )
    local missing=()
    local pkg
    for pkg in "${packages[@]}"; do
        if ! pkg_installed_pacman "$pkg"; then
            missing+=("$pkg")
        fi
    done
    if ((${#missing[@]} == 0)); then
        echo "All build dependencies are already installed (pacman)."
        return
    fi
    echo "Installing missing build dependencies (${#missing[@]} packages) via pacman..."
    run_privileged pacman -S --needed --noconfirm "${missing[@]}"
}

install_deps_debian() {
    local packages=(
        cmake
        extra-cmake-modules
        g++
        qt6-base-dev
        qt6-declarative-dev
        libplasma-dev
        plasma-workspace-dev
        libkf6config-dev
        libkf6coreaddons-dev
        libkf6i18n-dev
        libkf6itemmodels-dev
        libkf6windowsystem-dev
        libkirigami-dev
        libx11-dev
        libxtst-dev
    )
    local missing=()
    local pkg
    for pkg in "${packages[@]}"; do
        if ! pkg_installed_dpkg "$pkg"; then
            missing+=("$pkg")
        fi
    done
    if ((${#missing[@]} == 0)); then
        echo "All build dependencies are already installed (apt)."
        return
    fi
    echo "Installing missing build dependencies (${#missing[@]} packages) via apt..."
    run_privileged apt-get update -qq
    run_privileged apt-get install -y --no-install-recommends "${missing[@]}"
}

install_deps_fedora() {
    local pm
    if command -v dnf >/dev/null 2>&1; then
        pm=dnf
    elif command -v yum >/dev/null 2>&1; then
        pm=yum
    else
        echo "dnf/yum not found." >&2
        return 1
    fi
    local packages=(
        cmake
        extra-cmake-modules
        gcc-c++
        qt6-qtbase-devel
        qt6-qtdeclarative-devel
        libplasma-devel
        plasma-workspace-devel
        kf6-kconfig-devel
        kf6-kcoreaddons-devel
        kf6-ki18n-devel
        kf6-kitemmodels-devel
        kf6-kwindowsystem-devel
        kf6-kirigami-devel
        libX11-devel
        libXtst-devel
    )
    local missing=()
    local pkg
    for pkg in "${packages[@]}"; do
        if ! pkg_installed_rpm "$pkg"; then
            missing+=("$pkg")
        fi
    done
    if ((${#missing[@]} == 0)); then
        echo "All build dependencies are already installed (${pm})."
        return
    fi
    echo "Installing missing build dependencies (${#missing[@]} packages) via ${pm}..."
    run_privileged "$pm" install -y "${missing[@]}"
}

install_deps_opensuse() {
    local spec="$ROOT/packaging/rpm/quickbar.spec"
    if [[ -f "$spec" ]] && command -v zypper >/dev/null 2>&1; then
        echo "Installing build dependencies from RPM spec via zypper..."
        run_privileged zypper -n install -y rpm-build 2>/dev/null || true
        run_privileged zypper -n build-deps-install "$spec"
        return
    fi
    local packages=(
        cmake
        extra-cmake-modules
        gcc-c++
        libqt6-qtbase-devel
        libqt6-qtdeclarative-devel
        plasma6-libplasma-devel
        plasma6-workspace-devel
        kf6-kconfig-devel
        kf6-kcoreaddons-devel
        kf6-ki18n-devel
        kf6-kitemmodels-devel
        kf6-kwindowsystem-devel
        kf6-kirigami-devel
        libX11-devel
        libXtst-devel
    )
    local missing=()
    local pkg
    for pkg in "${packages[@]}"; do
        if ! pkg_installed_rpm "$pkg"; then
            missing+=("$pkg")
        fi
    done
    if ((${#missing[@]} == 0)); then
        echo "All build dependencies are already installed (zypper)."
        return
    fi
    echo "Installing missing build dependencies (${#missing[@]} packages) via zypper..."
    run_privileged zypper -n install -y "${missing[@]}"
}

print_manual_deps() {
    cat <<'EOF' >&2

Could not detect a supported package manager, or dependency install failed.
Install Plasma 6 build dependencies manually, then re-run:

  Arch / CachyOS:
    sudo pacman -S --needed base-devel cmake extra-cmake-modules gcc \
      libplasma plasma-workspace qt6-base qt6-declarative \
      kconfig kcoreaddons ki18n kitemmodels kwindowsystem kirigami libx11 libxtst

  Debian / Ubuntu (Plasma 6):
    sudo apt install cmake extra-cmake-modules g++ \
      qt6-base-dev qt6-declarative-dev libplasma-dev plasma-workspace-dev \
      libkf6config-dev libkf6coreaddons-dev libkf6i18n-dev libkf6itemmodels-dev \
      libkf6windowsystem-dev libkirigami-dev libx11-dev libxtst-dev

  Fedora:
    sudo dnf install cmake extra-cmake-modules gcc-c++ \
      qt6-qtbase-devel qt6-qtdeclarative-devel libplasma-devel \
      plasma-workspace-devel kf6-kconfig-devel kf6-kcoreaddons-devel \
      kf6-ki18n-devel kf6-kitemmodels-devel kf6-kwindowsystem-devel kf6-kirigami-devel \
      libX11-devel libXtst-devel

  openSUSE:
    sudo zypper build-deps-install packaging/rpm/quickbar.spec

See README.md and packaging/README.md for details.
EOF
}

ensure_build_dependencies() {
    if $skip_deps; then
        echo "Skipping dependency installation (--no-deps)."
        return
    fi

    local distro pretty_name="unknown"
    if [[ -f /etc/os-release ]]; then
        # shellcheck source=/dev/null
        source /etc/os-release
        pretty_name="${PRETTY_NAME:-${NAME:-unknown}}"
    fi
    distro="$(detect_distro)"
    echo "Detected distribution: ${distro} (${pretty_name})"

    case "$distro" in
        arch)
            if ! command -v pacman >/dev/null 2>&1; then
                echo "pacman not found." >&2
                print_manual_deps
                exit 1
            fi
            install_deps_arch
            ;;
        debian)
            if ! command -v apt-get >/dev/null 2>&1; then
                echo "apt-get not found." >&2
                print_manual_deps
                exit 1
            fi
            install_deps_debian
            ;;
        fedora)
            if ! install_deps_fedora; then
                print_manual_deps
                exit 1
            fi
            ;;
        opensuse)
            if ! command -v zypper >/dev/null 2>&1; then
                echo "zypper not found." >&2
                print_manual_deps
                exit 1
            fi
            install_deps_opensuse
            ;;
        *)
            echo "Unsupported or unknown distribution for automatic dependency install." >&2
            print_manual_deps
            exit 1
            ;;
    esac
}

ensure_build_dependencies

cd "$ROOT"

if ! command -v cmake >/dev/null 2>&1; then
    echo "cmake not found after dependency install. Install build dependencies and retry." >&2
    exit 1
fi

cmake -B "$build_dir" -DCMAKE_INSTALL_PREFIX="$prefix" -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
cmake --build "$build_dir"

if [[ "$prefix" == /usr ]]; then
    run_privileged cmake --install "$build_dir"
else
    cmake --install "$build_dir"
fi

echo "Installed QuickBar to $prefix. Restart plasmashell and add the QuickBar widget."
