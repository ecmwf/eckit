# (C) Copyright 2024- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

# this docker image contains utilities for building shared libraries and wrap them in python wheel, test and publish

# TODO parametrize this via build arg to support other manylinuxes
from quay.io/pypa/manylinux_2_28_x86_64:2024.11.24-1

run \
	yum install -y bison flex \
	&& curl -LsSf https://astral.sh/uv/install.sh | sh \
	&& mkdir /src /target \
	&& git clone --branch master --depth=1 https://github.com/ecmwf/ecbuild.git /src/ecbuild
copy buildscripts /buildscripts

# we workdir in /src so that the action checkout result ends up where expected
workdir /src

