#!/bin/bash
set -e
cd /workspace

export CC=clang
export CXX=clang++

# Conan-Profil ggf. anlegen
if ! conan profile list | grep -q "clang16"; then
    mkdir -p ~/.conan2/profiles
    cat > ~/.conan2/profiles/clang16 <<EOF
[settings]
os=Linux
arch=x86_64
compiler=clang
compiler.version=16
compiler.libcxx=libstdc++11
compiler.cppstd=17
build_type=Debug
EOF
fi

# Default-Profil anlegen (nur falls nicht vorhanden)
if [ ! -f ~/.conan2/profiles/default ]; then
    conan profile detect --force
fi

mkdir -p /workspace/build && cd /workspace/build

# Abhängigkeiten installieren
conan install .. \
  --output-folder=. \
  --profile:host=clang16 \
  --profile:build=clang16 \
  --build=missing \
  -s build_type=Debug

cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug

cmake --build .

# Projekt bauen
# conan build . \
#   -s build_type=Debug \
#   --build=missing