#!/bin/bash

set -euo pipefail

# NOTE in case of problems like we had with eccodes, replace with noop here
if [ "$(uname)" != "Darwin" ] ; then
    rm -rf /tmp/eckit/auditwheel
    auditwheel repair -w /tmp/eckit/auditwheel /tmp/eckit/build/wheel/*whl
    cd /tmp/eckit/auditwheel
    F=$(ls *whl)
    unzip $F
    patchelf --add-rpath '$ORIGIN' eckitlib.libs/*
    rm $F
    zip -r $F ./*
    rm /tmp/eckit/build/wheel/*
    mv /tmp/eckit/auditwheel/$F /tmp/eckit/build/wheel
    cd -
fi

# NOTE on macos we delocate with impunity, because the findlibs recursive depload
# is disabled anyway
if [ "$(uname)" = "Darwin" ] ; then
    delocate-wheel /tmp/eckit/build/wheel/*whl
fi
