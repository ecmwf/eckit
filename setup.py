# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


from os import getenv
from pathlib import Path

from Cython.Build import cythonize
from setuptools import Extension
from setuptools import setup

source_dir = getenv("ECKIT_SOURCE_DIR", str(Path(getenv("HOME"), "git", "eckit")))
binary_dir = getenv("ECKIT_BUILD_DIR", str(Path(getenv("HOME"), "build", "eckit")))
library_dirs = getenv("ECKIT_LIB_DIR", str(Path(binary_dir, "lib"))).split(":")

include_dirs_default = str(Path(source_dir, "src")) + ":" + str(Path(binary_dir, "src"))
include_dirs = getenv("ECKIT_INCLUDE_DIRS", include_dirs_default).split(":")

extra_compile_args = ["-std=c++17"]


def _ext(name: str, sources: list, libraries: list) -> Extension:
    return Extension(
        name,
        sources,
        language="c++",
        libraries=libraries,
        library_dirs=library_dirs,
        include_dirs=include_dirs,
        extra_compile_args=extra_compile_args,
        runtime_library_dirs=library_dirs,
        extra_link_args=extra_compile_args,
    )


setup(
    name="eckit-python",
    version="0.0.0",
    ext_modules=cythonize(
        [
            _ext("eckit._eckit", ["src/_eckit/_eckit.pyx"], ["eckit"]),
            _ext("eckit.geo._eckit_geo", ["src/_eckit/_eckit_geo.pyx"], ["eckit_geo"]),
        ],
        compiler_directives={"language_level": 3, "c_string_encoding": "default"},
    ),
)
