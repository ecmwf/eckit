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
    echo "installing deps for platform $(uname)"

    ## curl -- handled by docker container being built with:
    # dnf instal -y libpsl-devel openssl-devel curl-devel
    ## this below if the build would need a customization:
    # git clone https://github.com/curl/curl /src/curl
    # cd /src/curl
    # autoreconf -fi
    # ./configure --prefix /tmp/curl --with-openssl
    # make -j10 && make install
    # cd -

    ## proj
    git clone https://github.com/OSGeo/PROJ /src/proj
    mkdir /src/proj/build && cd /src/proj/build
    cmake .. -DCMAKE_INSTALL_PREFIX=/tmp/proj
    cmake --build . -j8
    cmake --build . --target install
    cd -

    # NOTE we will use auditwheel in post-compile. But in case of problems like we had with eccodes, introduce the libcopies here
else
    # TODO check macos agent has curl and proj
    echo "no deps installation for platform $(uname)"
fi

wget https://raw.githubusercontent.com/rockdaboot/libpsl/master/LICENSE -O python_wrapper/src/copying/libpsl.txt
wget https://raw.githubusercontent.com/openssl/openssl/master/LICENSE.txt -O python_wrapper/src/copying/libssl.txt
wget https://raw.githubusercontent.com/gnosis/libunistring/master/COPYING -O python_wrapper/src/copying/libunistring.txt
wget https://raw.githubusercontent.com/libidn/libidn2/master/COPYINGv2 -O python_wrapper/src/copying/libidn2.txt
wget https://raw.githubusercontent.com/curl/curl/master/COPYING -O python_wrapper/src/copying/libcurl.COPYING
wget https://raw.githubusercontent.com/curl/curl/master/LICENSES -O python_wrapper/src/copying/libcurl
wget https://raw.githubusercontent.com/OSGeo/PROJ/master/COPYING -O python_wrapper/src/copying/libproj.txt
echo '{"libpsl": {"path": "copying/libpsl.txt", "home": "https://github.com/rockdaboot/libpsl"}, "libssl": {"path": "copying/libssl.txt", "home": "https://github.com/openssl/openssl"}, "libcrypto": {"path": "copying/libssl.txt", "home": "https://github.com/openssl/openssl"}, "libunistring": {"path": "copying/libunistring.txt", "home": "https://github.com/gnosis/libunistring/"}, "libidn2": {"path": "copying/libidn2.txt", "home": "https://github.com/libidn/libidn2"}, "libcurl": {"path": "copying/liburl.COPYING", "home": "https://github.com/curl/curl"}, "libproj": {"path": "copying/libproj.txt", "home": "https://github.com/OSGeo/PROJ"}}' > python_wrapper/src/copying/list.json
