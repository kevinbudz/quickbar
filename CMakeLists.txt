cmake_minimum_required(VERSION 3.16)

project(quickbar VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Must be set before any Qt6 component that pulls in Qt6Qml (e.g. Quick)
set(QT_MAJOR_VERSION 6)
set(QT_DEFAULT_MAJOR_VERSION 6)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

# Core must be found first so Qt6CoreConfigExtras sets QT_DEFAULT_MAJOR_VERSION
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets Quick DBus)
find_package(X11 QUIET COMPONENTS Xtst)
# Plasma::Plasma links KF6::ConfigCore, KF6::CoreAddons, etc. — find them before Plasma
find_package(KF6 REQUIRED COMPONENTS Config CoreAddons I18n WindowSystem)
find_package(Plasma REQUIRED)
find_package(LibTaskManager REQUIRED)

# libdbusmenuqt is vendored from plasma-workspace (not packaged separately on most distros)
add_subdirectory(third_party/libdbusmenuqt)

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.quickbar.globalmenu\")

plasma_add_applet(org.quickbar.globalmenu
    QML_SOURCES
        qml/main.qml
        qml/MenuDelegate.qml
        qml/AppNameLabel.qml
        qml/config.qml
        qml/configGeneral.qml
        qml/configAbout.qml
    CPP_SOURCES
        src/quickbarapplet.cpp
        src/quickbarapplet.h
        src/appmenumodel.cpp
        src/appmenumodel.h
        src/genericmenu.cpp
        src/genericmenu.h
        src/menushortcutsender.cpp
        src/menushortcutsender.h
    RESOURCES
        main.xml
    QML_ARGS
        DEPENDENCIES
            QtCore
            org.kde.plasma.core
            org.kde.kirigami
            org.kde.plasma.components
            org.kde.kcmutils
            org.kde.plasma.configuration
)

# KDECMakeSettings unsets CMAKE_LIBRARY_OUTPUT_DIRECTORY on Linux, which makes
# plasma_add_applet try to link into /plasma/applets (absolute path at FS root).
set_target_properties(org.quickbar.globalmenu PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plasma/applets"
)

target_include_directories(org.quickbar.globalmenu PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(org.quickbar.globalmenu PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Quick
    Qt::Qml
    Qt::DBus
    Plasma::Plasma
    KF6::I18n
    KF6::WindowSystem
    PW::LibTaskManager
    dbusmenuqt
)

if(TARGET X11::Xtst)
    target_link_libraries(org.quickbar.globalmenu PRIVATE X11::X11 X11::Xtst)
endif()

# compile i18n strings into project
ki18n_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
