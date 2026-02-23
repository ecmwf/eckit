#!/usr/bin/env bash

set -eux

cd /home/runner/work/eckit/eckit/_codeql_build_dir/regressions


### Case 1

rm -f ECKIT-175.out && ./ECKIT-175.x 1

out=$(cat ECKIT-175.out | grep "NON Flushed message in myrun")

[[ -z ${out:-} ]] && echo "Failed" && exit 1


### Case 2

rm -f ECKIT-175.out && ./ECKIT-175.x 2

out=$(cat ECKIT-175.out | grep "NON Flushed message in myrun")

[[ -z ${out:-} ]] && echo "Failed" && exit 1


### Case 3

rm -f ECKIT-175.out && ./ECKIT-175.x 3

out=$(cat ECKIT-175.out | grep "NON Flushed message in myrun")

[[ -z ${out:-} ]] && echo "Failed" && exit 1



echo "OK"
