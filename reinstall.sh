#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr -DKDE_INSTALL_USE_QT_SYS_PATHS=ON
cmake --build build -j"$(nproc)"
echo "Built $(md5sum build/plasma/applets/org.quickbar.globalmenu.so)"
sudo cmake --install build
echo "Installed to /usr/lib/qt6/plugins/plasma/applets/org.quickbar.globalmenu.so"
echo "Restarting plasmashell..."
kquitapp6 plasmashell || true
sleep 1
kstart plasmashell &
