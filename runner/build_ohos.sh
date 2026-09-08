#!/usr/bin/env bash
set -euo pipefail

RUNNER_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SDK_ROOT="${SDK_ROOT:-${RUNNER_DIR}/../../sdk/default/openharmony}"
BUILD_DIR="${ADD_CUSTOM_RUNNER_BUILD_DIR:-${RUNNER_DIR}/build/ohos-arm64}"
CMAKE_BIN="${CMAKE_BIN:-${SDK_ROOT}/native/build-tools/cmake/bin/cmake}"
DDK_PATH="${DDK_PATH:-${RUNNER_DIR}/../../tool/DDK-tools-next-6.0.1.0}"

OMC_SDK_ARG=""
while [[ $# -gt 0 ]]; do
  case "$1" in
    --sdk) OMC_SDK_ARG="$2"; shift 2 ;;
    --sdk=*) OMC_SDK_ARG="${1#*=}"; shift ;;
    --help)
      printf 'usage: %s [--sdk /path/to/openharmony]\n' "$0"
      exit 0
      ;;
    *)
      printf 'unknown argument: %s\n' "$1" >&2
      exit 2
      ;;
  esac
done

# Check DDK path
if [[ ! -d "${DDK_PATH}" ]]; then
  printf 'DDK path not found: %s\n' "${DDK_PATH}" >&2
  printf 'Please set DDK_PATH environment variable or adjust path in script\n' >&2
  exit 2
fi

# Check SDK root
[[ -n "${OMC_SDK_ARG}" ]] && SDK_ROOT="${OMC_SDK_ARG}"
if [[ ! -d "${SDK_ROOT}" ]]; then
  printf 'No HarmonyOS SDK found at: %s\n' "${SDK_ROOT}" >&2
  printf 'Pass the SDK root via --sdk or set SDK_ROOT environment variable.\n' >&2
  exit 2
fi

CMAKE_BIN="${SDK_ROOT}/native/build-tools/cmake/bin/cmake"
if [[ ! -x "${CMAKE_BIN}" ]]; then
  CMAKE_BIN=cmake
fi

if [[ ! -f "${RUNNER_DIR}/device-libs/libhiai.so" ]]; then
  printf 'missing %s/libhiai.so; run %s/pull_device_libs.sh first\n' \
    "${RUNNER_DIR}/device-libs" "${RUNNER_DIR}" >&2
  exit 2
fi

"${CMAKE_BIN}" -S "${RUNNER_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_TOOLCHAIN_FILE="${SDK_ROOT}/native/build/cmake/ohos.toolchain.cmake" \
  -DOHOS_ARCH=arm64-v8a \
  -DOHOS_PLATFORM_LEVEL=20 \
  -DOHOS_STL=c++_shared \
  -DCMAKE_BUILD_TYPE=Release \
  -DHIAI_DEVICE_LIB_DIR="${RUNNER_DIR}/device-libs" \
  -DHIAI_INCLUDE_DIR="${DDK_PATH}/tools/tools_ascendc/include/ddk"
"${CMAKE_BIN}" --build "${BUILD_DIR}" --target add-custom-runner -j "${JOBS:-4}"
printf 'Harmony runner binary: %s/add-custom-runner\n' "${BUILD_DIR}"
