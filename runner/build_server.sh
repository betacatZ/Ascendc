#!/usr/bin/env bash
set -euo pipefail

# 服务器构建脚本 - 使用服务器上的实际路径
RUNNER_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 服务器上的实际路径
SDK_ROOT="${SDK_ROOT:-/data2/zhangdeming/GEWU/tools/Harmony_SDK/openharmony}"
DDK_PATH="${DDK_PATH:-/data2/zhangdeming/ascendc/tool/DDK-tools-next-6.0.1.0}"
BUILD_DIR="${BUILD_DIR:-${RUNNER_DIR}/build/ohos-arm64}"

# 检查 DDK 路径
if [[ ! -d "${DDK_PATH}" ]]; then
  printf 'DDK path not found: %s\n' "${DDK_PATH}" >&2
  printf 'Please set DDK_PATH environment variable\n' >&2
  exit 2
fi

# 检查 SDK 路径
if [[ ! -d "${SDK_ROOT}" ]]; then
  printf 'No HarmonyOS SDK found at: %s\n' "${SDK_ROOT}" >&2
  printf 'Please set SDK_ROOT environment variable\n' >&2
  exit 2
fi

CMAKE_BIN="${SDK_ROOT}/native/build-tools/cmake/bin/cmake"
if [[ ! -x "${CMAKE_BIN}" ]]; then
  CMAKE_BIN=cmake
fi

# 检查是否已拉取设备库
if [[ ! -f "${RUNNER_DIR}/device-libs/libhiai.so" ]]; then
  printf 'missing %s/device-libs/libhiai.so; run pull_device_libs.sh first\n' "${RUNNER_DIR}" >&2
  exit 2
fi

echo "=== Building AddCustom Runner ==="
echo "SDK_ROOT: ${SDK_ROOT}"
echo "DDK_PATH: ${DDK_PATH}"
echo "BUILD_DIR: ${BUILD_DIR}"
echo ""

"${CMAKE_BIN}" -S "${RUNNER_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_TOOLCHAIN_FILE="${SDK_ROOT}/native/build/cmake/ohos.toolchain.cmake" \
  -DOHOS_ARCH=arm64-v8a \
  -DOHOS_PLATFORM_LEVEL=20 \
  -DOHOS_STL=c++_shared \
  -DCMAKE_BUILD_TYPE=Release \
  -DHIAI_DEVICE_LIB_DIR="${RUNNER_DIR}/device-libs" \
  -DHIAI_INCLUDE_DIR="${DDK_PATH}/tools/tools_ascendc/include/ddk"

"${CMAKE_BIN}" --build "${BUILD_DIR}" --target add-custom-runner -j "${JOBS:-8}"

echo ""
echo "=== Build Success ==="
echo "Binary: ${BUILD_DIR}/add-custom-runner"
