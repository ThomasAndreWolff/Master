#!/bin/bash
# gurobi_cl
# Create necessary directories
mkdir -p build
conan profile detect
export CC=clang
export CXX=clang++
conan install . --output-folder=conan_build --build=missing \
  -s build_type=Debug \
  -s compiler=clang \
  -s compiler.version=16 \
  -s compiler.libcxx=libstdc++11 \
  -s compiler.cppstd=17
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=conan_build/conan_toolchain.cmake