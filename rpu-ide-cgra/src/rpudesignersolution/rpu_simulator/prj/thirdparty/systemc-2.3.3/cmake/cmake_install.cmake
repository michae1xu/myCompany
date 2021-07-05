# Install script for directory: C:/Users/jun/Downloads/systemc-2.3.3

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/SystemC")
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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdocx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/systemc" TYPE FILE FILES
    "C:/Users/jun/Downloads/systemc-2.3.3/AUTHORS"
    "C:/Users/jun/Downloads/systemc-2.3.3/ChangeLog"
    "C:/Users/jun/Downloads/systemc-2.3.3/INSTALL"
    "C:/Users/jun/Downloads/systemc-2.3.3/cmake/INSTALL_USING_CMAKE"
    "C:/Users/jun/Downloads/systemc-2.3.3/LICENSE"
    "C:/Users/jun/Downloads/systemc-2.3.3/NEWS"
    "C:/Users/jun/Downloads/systemc-2.3.3/NOTICE"
    "C:/Users/jun/Downloads/systemc-2.3.3/README"
    "C:/Users/jun/Downloads/systemc-2.3.3/RELEASENOTES"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage/SystemCLanguageTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage/SystemCLanguageTargets.cmake"
         "C:/Users/jun/Downloads/systemc-2.3.3/cmake/CMakeFiles/Export/lib/cmake/SystemCLanguage/SystemCLanguageTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage/SystemCLanguageTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage/SystemCLanguageTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage" TYPE FILE FILES "C:/Users/jun/Downloads/systemc-2.3.3/cmake/CMakeFiles/Export/lib/cmake/SystemCLanguage/SystemCLanguageTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage" TYPE FILE FILES "C:/Users/jun/Downloads/systemc-2.3.3/cmake/CMakeFiles/Export/lib/cmake/SystemCLanguage/SystemCLanguageTargets-debug.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage" TYPE FILE FILES "C:/Users/jun/Downloads/systemc-2.3.3/cmake/CMakeFiles/Export/lib/cmake/SystemCLanguage/SystemCLanguageTargets-minsizerel.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage" TYPE FILE FILES "C:/Users/jun/Downloads/systemc-2.3.3/cmake/CMakeFiles/Export/lib/cmake/SystemCLanguage/SystemCLanguageTargets-relwithdebinfo.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage" TYPE FILE FILES "C:/Users/jun/Downloads/systemc-2.3.3/cmake/CMakeFiles/Export/lib/cmake/SystemCLanguage/SystemCLanguageTargets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCLanguage" TYPE FILE FILES
    "C:/Users/jun/Downloads/systemc-2.3.3/cmake/SystemCLanguageConfig.cmake"
    "C:/Users/jun/Downloads/systemc-2.3.3/cmake/SystemCLanguageConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SystemCTLM" TYPE FILE FILES
    "C:/Users/jun/Downloads/systemc-2.3.3/cmake/SystemCTLMConfig.cmake"
    "C:/Users/jun/Downloads/systemc-2.3.3/cmake/SystemCTLMConfigVersion.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/jun/Downloads/systemc-2.3.3/cmake/docs/cmake_install.cmake")
  include("C:/Users/jun/Downloads/systemc-2.3.3/cmake/src/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/jun/Downloads/systemc-2.3.3/cmake/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
