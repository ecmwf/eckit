import os
from setuptools import setup
import platform
from wheel.bdist_wheel import bdist_wheel
import sys

with open('VERSION', 'r') as fVersion:
    version = fVersion.readlines()[0].strip()
install_requires = [
    f"eckitlib=={version}",
    "findlibs",
    "pyyaml",
    "requests",
]

# NOTE see ci-utils/wheelmaker/buildscripts/setup_utils, we need to get the right abi compat tag
class bdist_wheel_ext(bdist_wheel):
    def get_tag(self):
        python, abi, plat = bdist_wheel.get_tag(self)
        return python, abi, f"manylinux_2_28_{platform.machine()}"


ext_kwargs = {
    "darwin": {},
    "linux": {"cmdclass": {"bdist_wheel": bdist_wheel_ext}},
}

setup(
    version=version,
    packages=["eckit"],
    package_data={
        "": ["*.so"],
    },
    has_ext_modules=lambda: True,
    install_requires=install_requires,
    **ext_kwargs[sys.platform],
)
