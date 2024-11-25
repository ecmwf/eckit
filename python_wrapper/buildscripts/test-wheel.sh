#!/bin/bash
# (C) Copyright 2024- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

# ****

# just tests whether the package can be installed, imported, and __version__ matches what is expected

set -euo pipefail

source $1
PYTHON="python$2"

uv venv --python $PYTHON /tmp/venv
source /tmp/venv/bin/activate
uv pip install ./wheel-manylinux2_28-$2

INSTALLED_VERSION=$(python -c "import ${NAME}libs as l; print(l.__version__)")
EXPECTED_VERSION=$(cat /src/$NAME/VERSION)

test "$INSTALLED_VERSION" == "$EXPECTED_VERSION"
