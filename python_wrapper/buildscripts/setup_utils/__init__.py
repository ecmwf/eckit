# (C) Copyright 2024- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

"""
Utilities for setup invocation for shared-libs-only python wheels

Customization should mostly happen in setup.cfg

Resulting package's name & version come from env vars named NAME and VERSION
"""

from setuptools import setup, find_packages
from setuptools.dist import Distribution
from wheel.bdist_wheel import bdist_wheel
import pathlib
import os

class BinaryDistribution(Distribution):
    # we do this for making the packager aware of presence of binary modules, to include
    # the python version in the classifier
    def has_ext_modules(foo):
        return True

class bdist_wheel_ext(bdist_wheel):
    # This forces the platform tag from linux_x64_64 to manylinux_2_28
    # There is a chance that this is wrong, ie, the wheels are actually not compatible
    # More reliable would be to auditwheel this, eg,
    # LD_LIBRARY_PATH=/target/$NAME/lib64/ auditwheel repair dist/*whl --plat manylinux_2_28_x86_64 -L "libs"
    # However, the problem is that this messes up with the whole idea of creating a chain of wheels
    # Thus we would need to rip out the added .so files out and fix the rpaths, only making sure we preserve
    # the right .so files in case auditwheel decided to make changes (this manifests as eg `libeckit-xxyyzz.so`
    # appearing in the auditwheel-outputted wheel, alongside the original). Alternatively, we could replicate
    # what auditwheel is doing -- in our original compilation, thus address the incompatibility at the root
    def get_tag(self):
        python, abi, plat = bdist_wheel.get_tag(self)
        return python, abi, "manylinux_2_28_x86_64"

def plain_setup():
    setup(
        name=os.environ["NAME"],
        version=os.environ["VERSION"],
        package_dir={"": "src"},
        packages=find_packages(where="src"),
        package_data={"": ["*.so"]},
        distclass=BinaryDistribution,
        cmdclass={"bdist_wheel": bdist_wheel_ext},
    )
