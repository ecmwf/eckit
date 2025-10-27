/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Dec 2016

#ifndef eckit_os_SemLocker_h
#define eckit_os_SemLocker_h

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class SemLocker : private NonCopyable {

    static const int SLEEP = 1;

    int sem_;
    int maxWaitLock_;

    eckit::PathName path_;

public:

    SemLocker(int sem, const eckit::PathName& path, int maxWaitLock = 60);

    ~SemLocker();
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
