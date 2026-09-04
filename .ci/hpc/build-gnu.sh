#!/bin/bash

#SBATCH --qos=nf
#SBATCH --gres=ssdtmp:20G
#SBATCH --time=00:40:00
#SBATCH --nodes=1
#SBATCH --ntasks=8

module load prgenv/gnu
# gcc/old, not a version number: it is the alias for the compiler a login node
# gives you with nothing loaded (8.5.0 today), which is what this cluster's GNU
# builds actually target. `module avail gcc` lists no 8.5.0 to pin directly.
module unload gcc
module load gcc/old
module load cmake
module load ninja

echo "Using: $(command -v gcc) ($($(command -v gcc) --version | head -1))"
echo "Using: $(command -v g++) ($($(command -v g++) --version | head -1))"

for tool in bison flex; do
  if command -v "$tool" >/dev/null 2>&1; then
    "$tool" --version | head -1
  else
    echo "$tool: NOT on PATH -- ENABLE_ECKIT_SQL/CMD will fail configure below"
  fi
done

cmake -S "$CI_SOURCE_DIR" -B "${TMPDIR:-/tmp}/build" \
  -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER="$(command -v gcc)" \
  -DCMAKE_CXX_COMPILER="$(command -v g++)" \
  -DENABLE_TESTS=ON \
  -DENABLE_AEC=1 \
  -DENABLE_ECKIT_SQL=ON \
  -DENABLE_ECKIT_CMD=ON \
  -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON \
  -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
  -DCMAKE_INSTALL_PREFIX="$CI_INSTALL_PREFIX"
cmake --build "${TMPDIR:-/tmp}/build" --parallel "${SLURM_NTASKS:-8}"
ctest --test-dir "${TMPDIR:-/tmp}/build" --output-on-failure -j "${SLURM_NTASKS:-8}"
cmake --install "${TMPDIR:-/tmp}/build"

# The fetcher takes the artifact from CI_INSTALL_ARCHIVE, not from the install
# tree; .part + mv so it only ever appears complete.
mkdir -p "$(dirname "$CI_INSTALL_ARCHIVE")"
tar -cf - -C "$CI_INSTALL_PREFIX" . | zstd -T0 -q -o "$CI_INSTALL_ARCHIVE.part"
mv "$CI_INSTALL_ARCHIVE.part" "$CI_INSTALL_ARCHIVE"
