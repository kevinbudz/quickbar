Name:           quickbar
Version:        1.0
Release:        1%{?dist}
Summary:        Configurable Plasma 6 global menu panel widget

License:        GPL-2.0-or-later
URL:            https://github.com/kevinbudz/quickbar
Source0:        https://github.com/kevinbudz/quickbar/archive/v%{version}/quickbar-%{version}.tar.gz

BuildRequires:  cmake
BuildRequires:  extra-cmake-modules
BuildRequires:  gcc-c++
%if 0%{?suse_version}
BuildRequires:  qt6-base-devel
BuildRequires:  qt6-declarative-devel
BuildRequires:  libplasma6-devel
BuildRequires:  plasma6-workspace-devel
BuildRequires:  kf6-kconfig-devel
BuildRequires:  kf6-kcoreaddons-devel
BuildRequires:  kf6-ki18n-devel
BuildRequires:  kf6-kitemmodels-devel
BuildRequires:  kf6-kwindowsystem-devel
BuildRequires:  kf6-kirigami-devel
%else
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtdeclarative-devel
BuildRequires:  libplasma-devel
BuildRequires:  plasma-workspace-devel
BuildRequires:  kf6-kconfig-devel
BuildRequires:  kf6-kcoreaddons-devel
BuildRequires:  kf6-ki18n-devel
BuildRequires:  kf6-kitemmodels-devel
BuildRequires:  kf6-kwindowsystem-devel
BuildRequires:  kf6-kirigami-devel
%endif
BuildRequires:  libX11-devel
BuildRequires:  libXtst-devel

%if 0%{?suse_version}
Requires:       libplasma6%{?_isa}
Requires:       plasma6-workspace%{?_isa}
%else
Requires:       libplasma%{?_isa}
Requires:       plasma-workspace%{?_isa}
%endif
Requires:       qt6-qtbase%{?_isa}
Requires:       qt6-qtdeclarative%{?_isa}
Requires:       kf6-kconfig%{?_isa}
Requires:       kf6-kcoreaddons%{?_isa}
Requires:       kf6-ki18n%{?_isa}
Requires:       kf6-kwindowsystem%{?_isa}
Requires:       kf6-kirigami%{?_isa}

%description
QuickBar is a Plasma 6+ panel widget that shows the active application's
global menu (DBus app-menu), with more appearance and behavior options than
the stock Global Menu widget. Do not run both at once.

%prep
%autosetup -n quickbar-%{version}

%build
%cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo
%cmake_build

%install
%cmake_install

%files
%{_libdir}/qt6/plugins/plasma/applets/org.quickbar.globalmenu.so
%{_datadir}/locale/*/LC_MESSAGES/plasma_applet_org.quickbar.globalmenu.mo

%changelog
* Sat Sep 05 2026 Kevin Budz <https://github.com/kevinbudz> - 1.0-1
- Release 1.0

* Fri May 22 2026 Kevin Budz <https://github.com/kevinbudz> - 0.2.1-1
- Release 0.2.1

* Thu May 21 2026 Kevin Budz <https://github.com/kevinbudz> - 0.1.0-1
- Initial package
