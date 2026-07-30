#!/bin/bash

set -euo pipefail

# TODO:
# The situation, at the time of execution of the script, is as follows
# 1/ a wheel with libeckit.so, libeckit_geo.so, and some more, exists, at the location /tmp/eckit/build/wheel/*whl (see $F below)
# 2/ some of these .so files have a dependency which is not inside the wheel
# 3/ we invoke auditwheel to fix this, which puts `eckitlib.libs` module into the wheel, containing these external dependencies, with changed names, and updates the original libeckit.so and others to properly link them
# 4/ however, there is _another_ wheel which will be built later on, that refers to these external dependencies as well
# 5/ we do *not* want to call auditwheel on it -- instead, we would like this another wheel to utilize the eckitlib.libs instead! That another wheel will depend on the wheel with these libs, hence it should be runtime safe. We also have exact dependency constraints, hence we are even ABI-stable
# 6/ this _another_ wheel is not built yet, but the .so file which will go in there already exists! So we can simply patchelf it -- we will need to add rpath to it (relative, ie, assume that the two wheels are installed in the same venv folder and are `..`-reachable) and change the `Shared library` name to the name which auditwheel created
# 7/ the list of dependencies may change over time -- hence you will need to do it as follows: for each library name that is required by the .so that will be in the _another_ wheel, you need to check whether a library starting with the same name has been handled by auditwheel (example: the another wheel may require 'libproj.so.25' and auditwheel has created 'libproj-8e62344d.so.25.9.8.1' -- that is a match!)
# 8/ the _another_ wheel is not just one .so, it can be multiple ones -- and those will be nested at a different location. Which affects what you will put to rpath -- for an .so that will reside in the _another_ wheel's top level, you put there like '$ORIGIN/../eckitlib.libs/', for an .so that is in a submodule you'll have there instead '../../'.
# 9/ the .so files which will constitute the another wheel are located at ../eckit/src relative to this script (use the dirname BASH_SOURCE[0] thing to get it). Invoke find to get all .so files there, fix each in place

# the above describes the linux situation. For macos, we need to do the same -- note we currently dont inspect the delocated wheel at all, you will need to do so (but unlike in linux, you can then drop the inspected wheel). There are a few more differences -- delocate uses a different naming convention than auditwheel, and install name in the _another_ wheel's dylib would probably have to be changed as well, not just the rpath


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


# idea: 
# - have a look at the eckit.libs/ content, and inspect the bindings lip, and reroute
