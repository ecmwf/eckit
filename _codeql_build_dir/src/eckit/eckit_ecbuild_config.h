/*
 * (C) Copyright 2011- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef ECKIT_ecbuild_config_h
#define ECKIT_ecbuild_config_h

/* ecbuild info */

#ifndef ECBUILD_VERSION_STR
#define ECBUILD_VERSION_STR "3.13.1"
#endif
#ifndef ECBUILD_VERSION
#define ECBUILD_VERSION "3.13.1"
#endif
#ifndef ECBUILD_MACROS_DIR
#define ECBUILD_MACROS_DIR  "/home/runner/work/eckit/ecbuild/cmake"
#endif

/* config info */

#define ECKIT_OS_NAME          "Linux-6.11.0-1018-azure"
#define ECKIT_OS_BITS          64
#define ECKIT_OS_BITS_STR      "64"
#define ECKIT_OS_STR           "linux.64"
#define ECKIT_OS_VERSION       "6.11.0-1018-azure"
#define ECKIT_SYS_PROCESSOR    "x86_64"

#define ECKIT_BUILD_TIMESTAMP  "20260223132228"
#define ECKIT_BUILD_TYPE       "Release"

#define ECKIT_C_COMPILER_ID      "GNU"
#define ECKIT_C_COMPILER_VERSION "13.3.0"

#define ECKIT_CXX_COMPILER_ID      "GNU"
#define ECKIT_CXX_COMPILER_VERSION "13.3.0"

#define ECKIT_C_COMPILER       "/home/runner/work/eckit/.codeql-scratch/dbs/cpp/working/autobuild/bin/cc"
#define ECKIT_C_FLAGS          " -pipe -O3 -DNDEBUG"

#define ECKIT_CXX_COMPILER     "/home/runner/work/eckit/.codeql-scratch/dbs/cpp/working/autobuild/bin/c++"
#define ECKIT_CXX_FLAGS        " -pipe -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -O3 -DNDEBUG"

/* Needed for finding per package config files */

#define ECKIT_INSTALL_DIR       "/usr/local"
#define ECKIT_INSTALL_BIN_DIR   "/usr/local/bin"
#define ECKIT_INSTALL_LIB_DIR   "/usr/local/lib"
#define ECKIT_INSTALL_DATA_DIR  "/usr/local/share/eckit"

#define ECKIT_DEVELOPER_SRC_DIR "/home/runner/work/eckit/eckit"
#define ECKIT_DEVELOPER_BIN_DIR "/home/runner/work/eckit/eckit/_codeql_build_dir"

/* Fortran support */

#if 0

#define ECKIT_Fortran_COMPILER_ID      ""
#define ECKIT_Fortran_COMPILER_VERSION ""

#define ECKIT_Fortran_COMPILER ""
#define ECKIT_Fortran_FLAGS    ""

#endif

#endif /* ECKIT_ecbuild_config_h */
