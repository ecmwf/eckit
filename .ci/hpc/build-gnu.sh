#!/bin/bash
# eckit's HPC build recipe, submitted as a SLURM job by build-on-hpc.
#
# This is where the upstream HPC artifacts are actually exercised: it configures
# against the resolved $CMAKE_PREFIX_PATH (the cluster-built ecbuild and
# stack-deps), builds the library and its tests, and runs ctest — so a green job
# proves those artifacts link and run on a compute node.
#
# ci-infrastructure wraps this file (it unpacks the transferred source into
# node-local $TMPDIR and cds there, exports $CMAKE_PREFIX_PATH /
# $CI_INSTALL_PREFIX, appends the sentinel), so this script owns only its #SBATCH
# resources, module loads and the build/test/install — and must NOT print
# "Finished: ..." itself.

# atos (hpc2020) selects on QoS rather than partition; ssdtmp sizes the
# node-local SSD behind $TMPDIR, which holds the unpacked source and the build.
# Plain #SBATCH lines: troika's site API does not read its "# troika key=value"
# directives.
#SBATCH --qos=nf
#SBATCH --gres=ssdtmp:20G
#SBATCH --time=00:40:00
#SBATCH --nodes=1
#SBATCH --ntasks=8

module load prgenv/gnu
module load cmake
module load ninja

for tool in bison flex; do
  if command -v "$tool" >/dev/null 2>&1; then
    "$tool" --version | head -1
  else
    echo "$tool: NOT on PATH -- ENABLE_ECKIT_SQL/CMD will fail configure below"
  fi
done

# ENABLE_AEC is on by default and finds libaec in the stack-deps prefix. The
# remaining optional features (CONVEX_HULL, PROJ, SSL, CURL, MPI, ...) stay at
# their defaults, matching the runner leg.
cmake -S "$CI_SOURCE_DIR" -B "${TMPDIR:-/tmp}/build" \
  -GNinja \
  -DCMAKE_BUILD_TYPE=Release \
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
