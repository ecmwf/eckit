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

mkdir -p python/eckitlib/src/copying

if [ "$(uname)" != "Darwin" ] ; then
    # echo "no deps installation for platform $(uname)"
    prf="lib64"
    suf="so"
    # echo "installing deps for platform $(uname)"
    # NOTE lz4 in the base image already
    ## lz4
    # git clone https://github.com/lz4/lz4 /src/lz4 && cd /src/lz4
    # make -j10 && make install DESTDIR=/tmp/lz4
    # cd -
    echo "installing PROJ platform $(uname)"
    # build PROJ -- the DNF install does not seem to work out of the box, ie, cmake doesnt find it
    VERSION="9.7.0"
    pushd /tmp
    wget https://download.osgeo.org/proj/proj-$VERSION.tar.gz
    command -v md5sum >/dev/null 2>&1 && echo "MD5 (proj-$VERSION.tar.gz) = b7188aab7a22613a2f5a0bc41c4077b0" | md5sum --check
    tar xzf proj-$VERSION.tar.gz
    cd proj-$VERSION
    mkdir build target && cd build
    cmake ..
    CMAKE_BUILD_PARALLEL_LEVEL=8 cmake --build .
    cmake --install . --prefix /tmp/proj/target
    popd

else
    echo "no deps installation for platform $(uname)"
    prf="lib"
    suf="dylib"
fi

wget https://raw.githubusercontent.com/lz4/lz4/dev/LICENSE -O python/eckitlib/src/copying/liblz4.txt
wget https://raw.githubusercontent.com/OSGeo/PROJ/refs/heads/master/COPYING -O python/eckitlib/src/copying/proj.txt
echo '{"liblz4": {"path": "copying/liblz4.txt", "home": "https://github.com/lz4/lz4"}, "proj": {"path": "copying/proj.txt", "home": "https://github.com/OSGeo/PROJ"}}' > python/eckitlib/src/copying/list.json

