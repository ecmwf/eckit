#!/bin/bash

# the procedure for adding a new ext dependency to be bundled in here:
# - add git checkout, compile, etc
# - ensure the version ends up in python_wrapper/src/versions.txt
# - ensure the licence ends up in python_wrapper/src/copying/, and fname is referenced in copying/list.json
# - ensure the .so ends up in target/lib64/ with the expected libname
# - validate that the resulting wheel contains all the above
# additionally, make sure this script is aligned with /buildscripts/compile.sh and /buildscripts/wheel-linux.sh,
# in particular when it comes to install targets and package data, etc

# note also that for macos, we assume that the agent has the libraries already installed, as we can't run this in docker
# we thus only prepare the license files

set -euo pipefail

# Source the buildconfig so we can inspect $CMAKE_PARAMS below (e.g. to decide
# whether to bundle PROJ's data files).
# shellcheck source=/dev/null
source python/eckitlib/buildconfig

mkdir -p python/eckitlib/src/copying
mkdir -p /tmp/eckit/target/eckit/lib64/

if [ "$(uname)" != "Darwin" ] ; then
    echo "no deps installation for platform $(uname)"
    # echo "installing deps for platform $(uname)"
    ## lz4
    # git clone https://github.com/lz4/lz4 /src/lz4 && cd /src/lz4
    # make -j10 && make install DESTDIR=/tmp/lz4
    # cd -
    PROJ_ROOT="${PROJ_ROOT:-/opt/proj}"
else
    echo "no deps installation for platform $(uname)"
    PROJ_ROOT="${PROJ_ROOT:-/cxx-deps}"
fi


wget https://raw.githubusercontent.com/lz4/lz4/dev/LICENSE -O python/eckitlib/src/copying/liblz4.txt

# Bundle PROJ's data files (proj.db, proj.ini, grids) inside the wheel if we
# built eckit with PROJ support. The runtime code in eckit/geo/__init__.py
# points eckit's bundled libproj at this dir via the PROJ context API, so it
# works offline without any system PROJ install and without leaking PROJ_DATA
# into the wider process (which would interfere with pyproj/fiona/GDAL/etc.).
if echo " $CMAKE_PARAMS " | grep -qE '[[:space:]]-DENABLE_PROJ=(1|ON)[[:space:]]' ; then
    if [ ! -d "$PROJ_ROOT/share/proj" ] ; then
        echo "ERROR: ENABLE_PROJ=1 but no PROJ data found at $PROJ_ROOT/share/proj" >&2
        echo "       (override with PROJ_ROOT=/path/to/proj/prefix)" >&2
        exit 1
    fi
    mkdir -p /tmp/eckit/target/eckit/share/proj
    cp -r "$PROJ_ROOT/share/proj/." /tmp/eckit/target/eckit/share/proj/
    wget https://raw.githubusercontent.com/OSGeo/PROJ/master/COPYING -O python/eckitlib/src/copying/libproj.txt
    cat > python/eckitlib/src/copying/list.json <<'JSON'
{
  "liblz4": {"path": "copying/liblz4.txt", "home": "https://github.com/lz4/lz4"},
  "libproj": {"path": "copying/libproj.txt", "home": "https://proj.org"}
}
JSON
else
    echo '{"liblz4": {"path": "copying/liblz4.txt", "home": "https://github.com/lz4/lz4"}}' > python/eckitlib/src/copying/list.json
fi

uv pip install cython
