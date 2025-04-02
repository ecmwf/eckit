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

mkdir -p python_wrapper/src/copying
mkdir -p /tmp/eckit/target/eckit/lib64/

if [ "$(uname)" != "Darwin" ] ; then
    echo "no deps installation for platform $(uname)"
    # echo "installing deps for platform $(uname)"
    ## lz4
    # git clone https://github.com/lz4/lz4 /src/lz4 && cd /src/lz4
    # make -j10 && make install DESTDIR=/tmp/lz4
    # cd -
else
    echo "no deps installation for platform $(uname)"
fi


wget https://raw.githubusercontent.com/lz4/lz4/dev/LICENSE -O python_wrapper/src/copying/liblz4.txt
echo '{"liblz4": {"path": "copying/liblz4.txt", "home": "https://github.com/lz4/lz4"}}' > python_wrapper/src/copying/list.json
