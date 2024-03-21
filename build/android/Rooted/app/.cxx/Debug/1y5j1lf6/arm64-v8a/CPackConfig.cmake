# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BINARY_DEB "OFF")
set(CPACK_BINARY_FREEBSD "OFF")
set(CPACK_BINARY_IFW "OFF")
set(CPACK_BINARY_NSIS "OFF")
set(CPACK_BINARY_RPM "OFF")
set(CPACK_BINARY_STGZ "ON")
set(CPACK_BINARY_TBZ2 "OFF")
set(CPACK_BINARY_TGZ "ON")
set(CPACK_BINARY_TXZ "OFF")
set(CPACK_BINARY_TZ "ON")
set(CPACK_BUILD_SOURCE_DIRS "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/jni;/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/arm64-v8a")
set(CPACK_CMAKE_GENERATOR "Ninja")
set(CPACK_COMPONENTS_ALL "Unspecified;headers")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "/Users/jffzh/Library/Android/sdk/cmake/3.22.1/share/cmake-3.22/Templates/CPack.GenericDescription.txt")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY "rooted built using CMake")
set(CPACK_GENERATOR "STGZ;TGZ;TZ")
set(CPACK_INSTALL_CMAKE_PROJECTS "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/arm64-v8a;rooted;ALL;/")
set(CPACK_INSTALL_PREFIX "/usr/local")
set(CPACK_MODULE_PATH "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/sdl2atk/modules;/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/sdl2atk/external/ogg/modules")
set(CPACK_NSIS_DISPLAY_NAME "rooted 1.3.5")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
set(CPACK_NSIS_PACKAGE_NAME "rooted 1.3.5")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_OUTPUT_CONFIG_FILE "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/arm64-v8a/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "/Users/jffzh/Library/Android/sdk/cmake/3.22.1/share/cmake-3.22/Templates/CPack.GenericDescription.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "rooted built using CMake")
set(CPACK_PACKAGE_FILE_NAME "rooted-1.3.5-Android")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "rooted 1.3.5")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "rooted 1.3.5")
set(CPACK_PACKAGE_NAME "rooted")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "Humanity")
set(CPACK_PACKAGE_VERSION "1.3.5")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_RESOURCE_FILE_LICENSE "/Users/jffzh/Library/Android/sdk/cmake/3.22.1/share/cmake-3.22/Templates/CPack.GenericLicense.txt")
set(CPACK_RESOURCE_FILE_README "/Users/jffzh/Library/Android/sdk/cmake/3.22.1/share/cmake-3.22/Templates/CPack.GenericDescription.txt")
set(CPACK_RESOURCE_FILE_WELCOME "/Users/jffzh/Library/Android/sdk/cmake/3.22.1/share/cmake-3.22/Templates/CPack.GenericWelcome.txt")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_GENERATOR "TBZ2;TGZ;TXZ;TZ")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/arm64-v8a/CPackSourceConfig.cmake")
set(CPACK_SOURCE_RPM "OFF")
set(CPACK_SOURCE_TBZ2 "ON")
set(CPACK_SOURCE_TGZ "ON")
set(CPACK_SOURCE_TXZ "ON")
set(CPACK_SOURCE_TZ "ON")
set(CPACK_SOURCE_ZIP "OFF")
set(CPACK_SYSTEM_NAME "Android")
set(CPACK_THREADS "1")
set(CPACK_TOPLEVEL_TAG "Android")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/arm64-v8a/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
