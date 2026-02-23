#!/usr/bin/env bash

set -eux

cd /home/runner/work/eckit/eckit/_codeql_build_dir/regressions

exe="./ECKIT-221.x"

fname="file1.txt"
str="abc123"

### Case 1 - Serial

function create_file {
  rm -f $fname*
  echo "$str" >> $fname
  echo "$str" >> $fname
  echo "$str" >> $fname
}

create_file

$exe "$fname"

cmp "$fname" "$fname.0"

### Case 2 - Parallel

if [ "0" == "1" ] && [ "MPIEXEC_EXECUTABLE-NOTFOUND" != "MPIEXEC-NOTFOUND" ]; then

  create_file

  MPIEXEC_EXECUTABLE-NOTFOUND  -n 4 $exe "$fname"

  set +e
  filed_found=0
  for i in $(awk 'BEGIN {for(i=0;i<40;i++) print i}'); do
    ls -l $fname.0 $fname.1 $fname.2 $fname.3;
    status=$?
    if [ "$status" -eq 0 ]; then
      files_found=1
      break
    else
      sleep 5
    fi
    echo "Files not yet found or not yet accessible. Retry ($i)"
  done
  set -e

  if [ "${files_found}" -eq 0 ]; then
    echo "Files not found or not accessible"
    exit 1
  fi

  for i in 0 1 2 3; do
    cmp "$fname" "$fname.$i"
  done

fi

echo "OK"
