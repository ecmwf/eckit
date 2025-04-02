#!/bin/bash

# NOTE in case of problems like we had with eccodes, replace with noop here
if [ "$(uname)" != "Darwin" ] ; then
    auditwheel repair -w /tmp/eckit/auditwheel /tmp/eckit/build/wheel/*whl
    rm /tmp/eckit/build/wheel/*
    mv /tmp/eckit/auditwheel/* /tmp/eckit/build/wheel
fi

# NOTE on macos we delocate with impunity, because the findlibs recursive depload
# is disabled anyway
if [ "$(uname)" = "Darwin" ] ; then
    delocate-wheel /tmp/eckit/build/wheel/*whl
fi
