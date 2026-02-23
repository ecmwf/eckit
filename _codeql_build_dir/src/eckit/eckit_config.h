#ifndef eckit_config_h
#define eckit_config_h

#include "eckit/eckit_ecbuild_config.h"

#include "eckit/eckit_version.h"

// endiness

#define eckit_BIG_ENDIAN 0
#define eckit_LITTLE_ENDIAN 1

// system

#define ECKIT_SIZEOF_TIME_T    8

#define eckit_HAVE_DLFCN_H 1
#define eckit_HAVE_DLADDR 1
#define eckit_HAVE_MAP_ANONYMOUS 1
#define eckit_HAVE_MAP_ANON 1
#define eckit_HAVE_FUNOPEN 0
#define eckit_HAVE_FSYNC 1
#define eckit_HAVE_FDATASYNC 1
#define eckit_HAVE_F_FULLFSYNC 0
#define eckit_HAVE_FMEMOPEN 1
#define eckit_HAVE_DLINFO 0
#define eckit_HAVE_FOPENCOOKIE 1
#define eckit_HAVE_EXECINFO_BACKTRACE 1
#define eckit_HAVE_CXXABI_H 1
#define eckit_HAVE_GMTIME_R 1
#define eckit_HAVE_READDIR_R 1
#define eckit_HAVE_DIRFD 1
#define eckit_HAVE_DIRENT_D_TYPE 1
#define eckit_HAVE_CXX_INT_128 1
#define eckit_HAVE_AIO 1
#define eckit_HAVE_UNICODE 1
#define eckit_HAVE_XXHASH 1

// maths

#define eckit_HAVE_FEENABLEEXCEPT 0
#define eckit_HAVE_FEDISABLEEXCEPT 0

// memory

#define eckit_HAVE_ECKIT_MEMORY_FACTORY_BUILDERS_DEBUG 0
#define eckit_HAVE_ECKIT_MEMORY_FACTORY_EMPTY_DESTRUCTION 0

// external packages

#define eckit_HAVE_CUDA 0
#define eckit_HAVE_CURL 0
#define eckit_HAVE_EIGEN 0
#define eckit_HAVE_LAPACK 0
#define eckit_HAVE_MKL 0
#define eckit_HAVE_MPI 0
#define eckit_HAVE_OMP 0
#define eckit_HAVE_SSL 0
#define eckit_HAVE_ZIP 0

// Have we built certain libraries

#define eckit_HAVE_ECKIT_CMD 1
#define eckit_HAVE_ECKIT_SQL 1
#define eckit_HAVE_ECKIT_MPI 0

#endif // eckit_config_h
