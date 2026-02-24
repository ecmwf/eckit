/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
