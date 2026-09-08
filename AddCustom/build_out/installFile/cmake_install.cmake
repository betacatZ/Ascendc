# Install script for directory: /home/zdm/code/Ascendc/AddCustom/AddCustom

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/zdm/code/Ascendc/AddCustom/AddCustom/build_out")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/zdm/code/Ascendc/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/../../tools/platform/kirin9020/ops/impl/custom/add_custom.py")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/zdm/code/Ascendc/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/../../tools/platform/kirin9020/ops/impl/custom" TYPE FILE FILES "/home/zdm/code/Ascendc/AddCustom/AddCustom/build_out/_CPack_Packages/Linux/External/custom_opp_ubuntu_x86_64.run/packages/vendors/customize/op_impl/ai_core/tbe/customize_impl/dynamic/add_custom.py")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/zdm/code/Ascendc/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/../../tools/platform/kirin9020/ops/impl/custom/add_custom.cpp")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/zdm/code/Ascendc/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/../../tools/platform/kirin9020/ops/impl/custom" TYPE FILE FILES "/home/zdm/code/Ascendc/AddCustom/AddCustom/build_out/_CPack_Packages/Linux/External/custom_opp_ubuntu_x86_64.run/packages/vendors/customize/op_impl/ai_core/tbe/customize_impl/dynamic/add_custom.cpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/zdm/code/Ascendc/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/../../tools/platform/kirin9020/config/npu_custom_opinfo.json")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/zdm/code/Ascendc/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/../../tools/platform/kirin9020/config" TYPE FILE FILES "/home/zdm/code/Ascendc/AddCustom/AddCustom/build_out/npu_custom_opinfo.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/zdm/code/Ascendc/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/../../tools/platform/kirin9020/config/aic-kirin9020-ops-info.json")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/zdm/code/Ascendc/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/../../tools/platform/kirin9020/config" TYPE FILE FILES "/home/zdm/code/Ascendc/AddCustom/AddCustom/build_out/_CPack_Packages/Linux/External/custom_opp_ubuntu_x86_64.run/packages/vendors/customize/op_impl/ai_core/tbe/config/kirin9020/aic-kirin9020-ops-info.json")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/zdm/code/Ascendc/AddCustom/AddCustom/build_out/installFile/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
