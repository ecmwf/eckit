eckit
=====

[![eckit release version](https://img.shields.io/github/release/ecmwf/eckit.svg)](https://github.com/ecmwf/eckit/releases/latest)
[![travis master](https://img.shields.io/travis/ecmwf/eckit/master.svg?label=master&logo=travis)](http://travis-ci.org/ecmwf/eckit "master")
[![travis develop](https://img.shields.io/travis/ecmwf/eckit/develop.svg?label=develop&logo=travis)](http://travis-ci.org/ecmwf/eckit "develop")
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/094b5cd7496e46558d4495e6ec7d3c65)](https://www.codacy.com/app/tlmquintino/eckit?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=ecmwf/eckit&amp;utm_campaign=Badge_Grade)
[![codecov](https://codecov.io/gh/ecmwf/eckit/branch/develop/graph/badge.svg)](https://codecov.io/gh/ecmwf/eckit)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://github.com/ecmwf/eckit/blob/develop/LICENSE)

Project home: https://software.ecmwf.int/wiki/display/ECKIT


ecKit is a cross-platform c++ toolkit that supports development of tools and applications at ECMWF.
It is based on code developed over the last 20 years within the MARS software and was re-factored out to be reused by other applications.
It provides a an abstraction layer on top of the operating system, so it is easier to port code to new architectures.
It is developed taking into account the robustness requirements of running production systems at ECMWF.
The main focus is UNIX/POSIX systems, and it has been thoroughly tested on AIX, Linux and Mac OSX.
Historically, the code base pre-dates and in some way can be seen as a leaner substitute for some 'Boost' libraries.

It features facilities to easily handle, in a cross-platform way:
* multi-threading
* json and yaml parsing
* object serialization and persistence
* configuration of user options and resources
* file-system abstraction
* regular expressions
* socket networking
* http protocol
* type-to-type conversions
* asynchronous IO
* asynchronous processing
* exception handling and stack dumping
* MPI object-oriented wrapper
* linear algebra abstraction with multiple backends (BLAS, MKL, Armadillo, Eigen3)
* advanced container classes
    * space partition trees
    * file-mapped arrays

Requirements
------------

Tested compilers include:

- GCC 4.9.1, 5.3.0, 6.3.0, 7.2.0
- Intel 15.0.2, 16.0.3, 17, 18
- CCE 8.4.5, 8.5.8, 8.6.2
- Apple LLVM 9.1.0 (clang-902.0.39.1)

Required dependencies:

- CMake --- For use and installation see http://www.cmake.org/
- ecbuild --- ECMWF library of CMake macros
- Perl5 --- For some code generation

Installation
------------

Eckit employs an out-of-source build/install based on CMake.

Make sure ecbuild is installed and the ecbuild executable script is found ( `which ecbuild` ).

Now proceed with installation as follows:

```bash
# Environment --- Edit as needed
srcdir=$(pwd)
builddir=build
installdir=$HOME/local

# 1. Create the build directory:
mkdir $builddir
cd $builddir

# 2. Run CMake
ecbuild --prefix=$installdir -- $srcdir

# 3. Compile / Install
make -j10
make install

# 4. Check installation
$installdir/bin/eckit-version
```



