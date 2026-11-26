// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Tiago Quintino
/// @date   Feb 2021

#pragma once

#include <dirent.h>
#include <sys/types.h>

namespace eckit {

class LocalPathName;

//----------------------------------------------------------------------------------------------------------------------

class StdDir {

private:  // members

    DIR* d_;
    struct dirent buf;

public:  // methods

    StdDir(const char* d);
    StdDir(const eckit::LocalPathName& p);

    StdDir(const StdDir&)            = delete;
    StdDir& operator=(const StdDir&) = delete;
    StdDir(StdDir&&)                 = delete;
    StdDir& operator=(StdDir&&)      = delete;

    ~StdDir(); /* noexcept(false) */

    operator DIR*() { return d_; }

    struct dirent* dirent();
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
