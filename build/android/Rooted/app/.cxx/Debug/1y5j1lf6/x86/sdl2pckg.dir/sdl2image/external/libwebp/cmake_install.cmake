# Install script for directory: /Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/sdl2image/external/libwebp

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/src/libwebpdecoder.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/src/libwebp.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/src/demux/libwebpdemux.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/libcpufeatures-libwebp.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/libwebpdecoder.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/webp" TYPE FILE FILES
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/libwebp/src/webp/decode.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/libwebp/src/webp/types.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/libwebp.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/webp" TYPE FILE FILES
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/libwebp/src/webp/decode.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/libwebp/src/webp/encode.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/libwebp/src/webp/types.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/libwebpdemux.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/webp" TYPE FILE FILES
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/libwebp/src/webp/decode.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/libwebp/src/webp/demux.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/libwebp/src/webp/mux_types.h"
    "/Users/jffzh/Cornell/23-24/CS4152/cugl/sdlapp/buildfiles/cmake/../../external/libwebp/src/webp/types.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/WebP/cmake/WebPTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/WebP/cmake/WebPTargets.cmake"
         "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/CMakeFiles/Export/share/WebP/cmake/WebPTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/WebP/cmake/WebPTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/WebP/cmake/WebPTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/WebP/cmake" TYPE FILE FILES "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/CMakeFiles/Export/share/WebP/cmake/WebPTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/WebP/cmake" TYPE FILE FILES "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/CMakeFiles/Export/share/WebP/cmake/WebPTargets-debug.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/WebP/cmake" TYPE FILE FILES
    "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/WebPConfigVersion.cmake"
    "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/x86/sdl2pckg.dir/sdl2image/external/libwebp/WebPConfig.cmake"
    )
endif()

