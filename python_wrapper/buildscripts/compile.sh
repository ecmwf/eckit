#!/bin/bash
# (C) Copyright 2024- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

set -euo pipefail

# this script is configured by sourcing its first param. Namely, we expect the following:
# $NAME -- name of the project being compiled. Used for /src/$NAME and /target/$NAME, should thus correspond to git repo name
# $CMAKE_PARAMS -- passed to ecbuild after the `--`. Eg "-DENABLE_THIS=1 -DENABLE_THAT=0"
# $PYPROJECT_DIR -- ignored

source $1

rm -rf /build && mkdir /build && cd /build 
/src/ecbuild/bin/ecbuild --prefix=/target/$NAME -- $CMAKE_PARAMS /src/$NAME
make -j10
make install
