#!/bin/bash

set -e # Stop script immediately if any command fails

# Handle Command Line Arguments:
BUILD_TYPE="Debug"
EXTRA_CXXFLAGS="-Wall -Wextra"
CLEAN_FIRST=0
FORCE_RECONFIGURE=0

while [ $# -gt 0 ]; do
  case "$1" in
    release|Release|--release)
      BUILD_TYPE="Release"
      shift
      ;;
    -O2|-O3|-O0)
      EXTRA_CXXFLAGS="$EXTRA_CXXFLAGS $1"
      shift
      ;;
    --clean|-c)
      CLEAN_FIRST=1
      shift
      ;;
    --reconfigure|-r)
      FORCE_RECONFIGURE=1
      shift
      ;;
    --asm|-S|--assembly)
      # -save-temps generates .s assembly files without stopping the executable build
      EXTRA_CXXFLAGS="$EXTRA_CXXFLAGS -save-temps=obj"
      shift
      ;;
    *)
      echo "Unknown option: $1"
      exit 1
      ;;
  esac
done

# Change current directory to the directory containing this script
cd "$(dirname "$0")"

# 1. Handle clean FIRST before setting up directories
if [ $CLEAN_FIRST -eq 1 ]; then
  echo "Cleaning build directory..."
  rm -rf build
fi

# 2. Force re-configuration if requested
if [ $FORCE_RECONFIGURE -eq 1 ] && [ -d "build" ]; then
  echo "Removing CMake cache to force re-configuration..."
  rm -rf build/CMakeCache.txt build/CMakeFiles
fi

# 3. Ensure build directory exists
mkdir -p build

# 4. Single CMake configuration step
cmake -B build \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -DCMAKE_CXX_FLAGS="$EXTRA_CXXFLAGS" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# 5. Symlink compile_commands.json to root so clangd finds it instantly
ln -sf build/compile_commands.json .

# 6. Build in parallel (uses all cores automatically)
cmake --build build --parallel

echo "--Build Complete--"
