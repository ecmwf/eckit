#!/usr/bin/env bash

set -eux

cd /home/runner/work/eckit/eckit/_codeql_build_dir/regressions

exe="./ECKIT-166.x"

if [ "0" == "1" ] && [ "MPIEXEC_EXECUTABLE-NOTFOUND" != "MPIEXEC-NOTFOUND" ]; then

  MPIEXEC_EXECUTABLE-NOTFOUND  -n 1 $exe

fi

echo "OK"
