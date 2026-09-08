#!/bin/bash
# Script function comment: build project
# Copyright: Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.

script_path=$(realpath $(dirname $0))

if [[ "$DDK_PATH" == "" ]]
then
  echo "DDK path not set."
  exit
fi

mkdir -p build_out
rm -rf build_out/*
cd build_out

cmake_version=$(cmake --version | grep "cmake version" | awk '{print $3}')

opts=$(python3 $script_path/cmake/util/preset_parse.py $script_path/CMakePresets.json)

target=package
if [ "$1"x != ""x ]; then target=$1; fi
if [ "$cmake_version" \< "3.19.0" ] ; then
  cmake .. $opts -DHOST=true
else
  cmake .. --preset=default -DHOST=true
fi

cmake --build . --target $target -j16

make_result=$?
if [ $make_result -ne 0 ]
then
  exit
fi

#install
mkdir -p installFile
rm -rf installFile/*
cd installFile

cmake ../../ $opts -DINSTALL=true
make && make install

insall_result=$?
if [ $make_result -ne 0 ]
then
  exit
fi
echo "Build and install success"