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
# - $NAME -- name of the project being compiled. Used for /target/$NAME/lib64 where the shared libraries are expected, and for naming the python module `$NAME-libs`
# - $CMAKE_PARAMS -- ignored
# - $PYPROJECT_DIR -- where pyproject.toml/setup.py are expected, prefixed with src/$NAME. Param to `python -m build`
# additionally:
# - second param is the target python version, in the x.y format
# - version, license, authors and readme are expected to be root-located in src/$NAME

source $1
PYTHON="python$2"
PYPROJECT_DIR=/src/$NAME/$PYPROJECT_DIR

mkdir -p $PYPROJECT_DIR/src
ln -s /target/$NAME/lib64 $PYPROJECT_DIR/src/${NAME}libs
ln -s ../LICENSE $PYPROJECT_DIR
ln -s ../README.md $PYPROJECT_DIR
ln -s ../AUTHORS $PYPROJECT_DIR
VERSION=$(cat /src/$NAME/VERSION)
echo "__version__ = '$VERSION'" > $PYPROJECT_DIR/src/${NAME}libs/__init__.py

PYTHONPATH=/buildscripts NAME=$NAME VERSION=$VERSION uv run --python $PYTHON python -m build --installer uv $PYPROJECT_DIR

mkdir /build/wheel
mv dist/*whl /build/wheel
