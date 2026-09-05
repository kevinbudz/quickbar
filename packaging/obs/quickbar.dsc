Format: 3.0 (quilt)
Source: quickbar
Version: 1.0-1
Binary: quickbar
Maintainer: Kevin Budz <https://github.com/kevinbudz>
Architecture: any
Standards-Version: 4.7.0
Build-Depends: debhelper-compat (= 13),
               cmake,
               extra-cmake-modules,
               g++,
               qt6-base-dev,
               qt6-declarative-dev,
               libplasma-dev (>= 6.5),
               plasma-workspace-dev (>= 6.5),
               libkf6config-dev,
               libkf6coreaddons-dev,
               libkf6i18n-dev,
               libkf6itemmodels-dev,
               libkf6windowsystem-dev,
               libkirigami-dev,
               libx11-dev,
               libxtst-dev

Package: quickbar
Architecture: any
Depends: ${shlibs:Depends},
         ${misc:Depends},
         libplasma6,
         plasma-workspace,
         qt6-base,
         qt6-declarative,
         libkf6config6,
         libkf6coreaddons6,
         libkf6i18n6,
         libkf6windowsystem6,
         libkirigami6
Description: Plasma 6 global menu panel widget (QuickBar)
 QuickBar shows the active application's global menu on the Plasma panel,
 with configurable appearance and behavior. Requires Plasma 6.5+.
 Remove the stock Global Menu widget before using QuickBar.
