cmake_minimum_required(VERSION 3.16)

project(add_custom LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(RUNNER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
set(ROOT_DIR "${RUNNER_DIR}/../..")
set(OHOS_NATIVE_ROOT
    "${ROOT_DIR}/sdk/default/openharmony/native"
    CACHE PATH "OpenHarmony native SDK root")
set(HIAI_INCLUDE_DIR
    "${ROOT_DIR}/tool/DDK-tools-next-6.0.1.0/tools/tools_ascendc/include/ddk"
    CACHE PATH "Directory containing HiAiModelManagerService.h")
set(HIAI_DEVICE_LIB_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/device-libs"
    CACHE PATH "Directory containing device-pulled libhiai.so")

find_path(HIAI_MODEL_MANAGER_INCLUDE
    NAMES HiAiModelManagerService.h
    PATHS "${HIAI_INCLUDE_DIR}"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)
find_library(HIAI_PLATFORM_LIB
    NAMES hiai
    PATHS "${HIAI_DEVICE_LIB_DIR}"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH)

if(NOT HIAI_MODEL_MANAGER_INCLUDE)
    message(FATAL_ERROR "HiAiModelManagerService.h not found under ${HIAI_INCLUDE_DIR}")
endif()
if(NOT HIAI_PLATFORM_LIB)
    message(FATAL_ERROR "libhiai.so not found; run pull_device_libs.sh first")
endif()

add_executable(add-custom-runner src/add_custom_runner.cpp)

# 强制使用 SDK 的 C++ 头文件，避免混用系统头文件
get_filename_component(LLVM_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
get_filename_component(LLVM_DIR "${LLVM_DIR}" DIRECTORY)
set(OHOS_CXX_INCLUDE "${LLVM_DIR}/include/libcxx-ohos/include/c++/v1")
set(OHOS_SYSROOT "${CMAKE_SYSROOT}")

target_include_directories(add-custom-runner SYSTEM BEFORE PRIVATE
    "${OHOS_CXX_INCLUDE}"
    "${HIAI_MODEL_MANAGER_INCLUDE}")

target_compile_options(add-custom-runner PRIVATE
    -Wall -Wextra -Wpedantic
    -nostdinc++
    -isystem "${OHOS_CXX_INCLUDE}"
    -isystem "${OHOS_SYSROOT}/usr/include"
    -isystem "${OHOS_SYSROOT}/usr/include/aarch64-linux-ohos")

target_link_libraries(add-custom-runner PRIVATE "${HIAI_PLATFORM_LIB}")
target_link_options(add-custom-runner PRIVATE
    -Wl,--allow-shlib-undefined
    -Wl,-rpath,/system/lib64/platformsdk)
