# Install script for directory: /Users/macbookpro/leopard-webkit-build/sources_610/Source/WebCore

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
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/WebCore.framework")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/lib" TYPE DIRECTORY FILES "/Users/macbookpro/leopard-webkit-build/build-610/lib/WebCore.framework" USE_SOURCE_PERMISSIONS)
  if(EXISTS "$ENV{DESTDIR}/usr/local/lib/WebCore.framework/Versions/SOVERSION/WebCore" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/lib/WebCore.framework/Versions/SOVERSION/WebCore")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/macbookpro/leopard-webkit-build/build-610/lib"
      "$ENV{DESTDIR}/usr/local/lib/WebCore.framework/Versions/SOVERSION/WebCore")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/usr/local/lib/WebCore.framework/Versions/SOVERSION/WebCore")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/WebCoreTestSupport.framework")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/lib" TYPE DIRECTORY FILES "/Users/macbookpro/leopard-webkit-build/build-610/lib/WebCoreTestSupport.framework" USE_SOURCE_PERMISSIONS)
  if(EXISTS "$ENV{DESTDIR}/usr/local/lib/WebCoreTestSupport.framework/Versions/A/WebCoreTestSupport" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/lib/WebCoreTestSupport.framework/Versions/A/WebCoreTestSupport")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/macbookpro/leopard-webkit-build/build-610/lib"
      "$ENV{DESTDIR}/usr/local/lib/WebCoreTestSupport.framework/Versions/A/WebCoreTestSupport")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/usr/local/lib/WebCoreTestSupport.framework/Versions/A/WebCoreTestSupport")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/WebCore.framework")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/local/lib" TYPE DIRECTORY FILES "/Users/macbookpro/leopard-webkit-build/build-610/lib/WebCore.framework" USE_SOURCE_PERMISSIONS)
  if(EXISTS "$ENV{DESTDIR}/usr/local/lib/WebCore.framework/Versions/SOVERSION/WebCore" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/usr/local/lib/WebCore.framework/Versions/SOVERSION/WebCore")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/macbookpro/leopard-webkit-build/build-610/lib"
      "$ENV{DESTDIR}/usr/local/lib/WebCore.framework/Versions/SOVERSION/WebCore")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/usr/local/lib/WebCore.framework/Versions/SOVERSION/WebCore")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  include("/Users/macbookpro/leopard-webkit-build/build-610/Source/WebCore/CMakeFiles/WebCore.dir/install-cxx-module-bmi-Release.cmake" OPTIONAL)
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/macbookpro/leopard-webkit-build/build-610/Source/WebCore/PAL/pal/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/macbookpro/leopard-webkit-build/build-610/Source/WebCore/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
