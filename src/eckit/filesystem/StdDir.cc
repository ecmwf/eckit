/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/StdDir.h"

#include <dirent.h>
#include <unistd.h>

#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/os/Stat.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

StdDir::StdDir(const char* d) {
    d_ = ::opendir(d);
}

StdDir::StdDir(const LocalPathName& p) {
    d_ = ::opendir(p.localPath());
}

StdDir::~StdDir() /* noexcept(false) */ {
    if (d_) {
        ::closedir(d_);
    }
}

struct dirent* StdDir::dirent() {

    struct dirent* e = nullptr;
    errno            = 0;

    /// @todo Note that readdir_r() has been deprecated by POSIX standard
    ///       however readdir() isn't thread-safe yet, not even in glic implementation
    ///       until then we prefer to use readdir_r
#if eckit_HAVE_READDIR_R
// Disable deprecation warning of using readdir_r
#if defined(__INTEL_COMPILER)
#pragma warning ( disable:1478 )
#elif defined(__NVCOMPILER)
#pragma diag_suppress 1216
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
    ::readdir_r(d_, &buf, &e);
#else
    e = ::readdir(d_);
#endif

    if (e == nullptr && errno) {
#if eckit_HAVE_READDIR_R
        const char* func = "readdir_r";
#else
        const char* func = "readdir";
#endif
        throw eckit::FailedSystemCall(func, Here());
    }

    return e;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
