# Install script for directory: /Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/sdl2ttf/external/harfbuzz

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Users/jffzh/Library/Android/sdk/ndk/25.1.8937393/toolchains/llvm/prebuilt/darwin-x86_64/bin/llvm-objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/harfbuzz" TYPE FILE FILES
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-aat-layout.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-aat.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-blob.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-buffer.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-common.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-deprecated.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-draw.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-face.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-font.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-map.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ot-color.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ot-deprecated.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ot-font.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ot-layout.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ot-math.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ot-meta.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ot-metrics.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ot-name.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ot-shape.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ot-var.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ot.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-set.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-shape-plan.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-shape.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-style.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-unicode.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-version.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/harfbuzz/src/hb-ft.h"
    )
endif()

