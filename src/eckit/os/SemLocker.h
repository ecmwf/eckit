// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Dec 2016

#ifndef eckit_os_SemLocker_h
#define eckit_os_SemLocker_h

#include "eckit/filesystem/PathName.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class SemLocker {

    static const int SLEEP = 1;

    int sem_;
    int maxWaitLock_;

    eckit::PathName path_;

public:

    SemLocker(int sem, const eckit::PathName& path, int maxWaitLock = 60);

    SemLocker(const SemLocker&)            = delete;
    SemLocker& operator=(const SemLocker&) = delete;
    SemLocker(SemLocker&&)                 = delete;
    SemLocker& operator=(SemLocker&&)      = delete;

    ~SemLocker();
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
