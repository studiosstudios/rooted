IF(NOT EXISTS "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/arm64-v8a/cugl.dir/libdatachannel.dir/deps/openssl/install_manifest.txt")
  MESSAGE(FATAL_ERROR "Cannot find install manifest:
\"/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/arm64-v8a/cugl.dir/libdatachannel.dir/deps/openssl/install_manifest.txt\"")
ENDIF(NOT EXISTS "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/arm64-v8a/cugl.dir/libdatachannel.dir/deps/openssl/install_manifest.txt")

FILE(READ "/Users/jffzh/Cornell/23-24/CS4152/rooted/build/android/Rooted/app/.cxx/Debug/1y5j1lf6/arm64-v8a/cugl.dir/libdatachannel.dir/deps/openssl/install_manifest.txt" files)
STRING(REGEX REPLACE "\n" ";" files "${files}")
FOREACH(file ${files})
  MESSAGE(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
  IF(EXISTS "$ENV{DESTDIR}${file}")
    EXEC_PROGRAM(
      "/Users/jffzh/Library/Android/sdk/cmake/3.22.1/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    IF(NOT "${rm_retval}" STREQUAL 0)
      MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    ENDIF(NOT "${rm_retval}" STREQUAL 0)
  ELSE(EXISTS "$ENV{DESTDIR}${file}")
    MESSAGE(STATUS "File \"$ENV{DESTDIR}${file}\" does not exist.")
  ENDIF(EXISTS "$ENV{DESTDIR}${file}")
ENDFOREACH(file)
