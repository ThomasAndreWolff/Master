#!/bin/bash
# gurobi_cl
# Create necessary directories
mkdir -p build
conan profile detect
conan install . --build=missing