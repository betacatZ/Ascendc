#!/usr/bin/env bash
set -euo pipefail

RUNNER_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# This script pulls libhiai.so from a connected HarmonyOS device
# You need to have adb/hdc connected to the target device

HDC="${HDC:-hdc}"
DEVICE_LIB_PATH="/system/lib64/platformsdk/libhiai.so"
TARGET_DIR="${RUNNER_DIR}/device-libs"

if ! command -v "${HDC}" &> /dev/null; then
  printf 'hdc command not found. Please install HarmonyOS SDK tools.\n' >&2
  exit 2
fi

# Check device connection
if ! "${HDC}" list targets | grep -q .; then
  printf 'No HarmonyOS device connected. Please connect a device first.\n' >&2
  exit 2
fi

mkdir -p "${TARGET_DIR}"

printf 'Pulling libhiai.so from device...\n'
"${HDC}" file recv "${DEVICE_LIB_PATH}" "${TARGET_DIR}/libhiai.so"

if [[ -f "${TARGET_DIR}/libhiai.so" ]]; then
  printf 'Successfully pulled libhiai.so to %s\n' "${TARGET_DIR}"
else
  printf 'Failed to pull libhiai.so from device\n' >&2
  exit 2
fi
