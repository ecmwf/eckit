eckit
=====

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/094b5cd7496e46558d4495e6ec7d3c65)](https://www.codacy.com/app/tlmquintino/eckit?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=ecmwf/eckit&amp;utm_campaign=Badge_Grade)

Project home: https://software.ecmwf.int/wiki/display/ECKIT


ecKit is a cross-platform c++ toolkit that supports development of tools and applications at ECMWF.
It is based on code developed over the last 20 years within the MARS software and was re-factored out to be reused by other applications.
It provides a an abstraction layer on top of the operating system, so it is easier to port code to new architectures.
It is developed taking into account the robustness requirements of running production systems at ECMWF.
The main focus is UNIX/POSIX systems, and it has been thoroughly tested on AIX, Linux and Mac OSX.
Historically, the code base pre-dates and in some way can be seen as a leaner substitute for some 'Boost' libraries.

It features facilities to easily handle, in a cross-platform way:
* multi-threading
* serialization
* json and yaml parsing
* object persistence
* configuration of user options and resources
* file-system abstraction
* regular expressions
* socket networking
* http protocol
* type-to-type conversions
* asynchronous IO
* asynchronous processing
* exception handling and stack dumping
* smart pointers
* container classes
* space partition trees
* file-mapped arrays
* MPI object-oriented wrapper
* linear algebra abstraction with multiple backends (BLAS, MKL, Armadillo, Eigen3)

Requirements
------------

Tested compilers include:

- GCC 4.9.1, 5.3.0, 6.3.0, 7.2.0
- Intel 15.0.2, 16.0.3, 17, 18
- CCE 8.4.5, 8.5.8, 8.6.2

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



