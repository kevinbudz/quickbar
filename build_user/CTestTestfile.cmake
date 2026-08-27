# CMake generated Testfile for 
# Source directory: /home/kevin/Documents/GitHub/quickbar
# Build directory: /home/kevin/Documents/GitHub/quickbar/build_user
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(appstreamtest "/usr/bin/cmake" "-DAPPSTREAMCLI=/usr/bin/appstreamcli" "-DINSTALL_FILES=/home/kevin/Documents/GitHub/quickbar/build_user/install_manifest.txt" "-P" "/usr/share/ECM/kde-modules/appstreamtest.cmake")
set_tests_properties(appstreamtest PROPERTIES  _BACKTRACE_TRIPLES "/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;177;add_test;/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;195;appstreamtest;/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;0;;/home/kevin/Documents/GitHub/quickbar/CMakeLists.txt;16;include;/home/kevin/Documents/GitHub/quickbar/CMakeLists.txt;0;")
subdirs("third_party/libdbusmenuqt")
