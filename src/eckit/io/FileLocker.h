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
/// @date Feb 2002

#ifndef eckit_FileLocker_h
#define eckit_FileLocker_h

#include "sys/types.h"  // for off_t

#include "eckit/eckit.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FileLocker {
public:

    // -- Exceptions
    // None

    // -- Contructors

    FileLocker(int fd);

    // -- Destructor

    ~FileLocker();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    void lockShared(off_t off = 0, off_t len = 0);
    void lockExclusive(off_t off = 0, off_t len = 0);
    void unlock(off_t off = 0, off_t len = 0);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None


protected:

    // -- Members
    // None

    // -- Methods

    // void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed

    FileLocker(const FileLocker&);
    FileLocker& operator=(const FileLocker&);

    // -- Members

    int fd_;

    // -- Methods
    // None

    void lockRange(off_t, off_t, int cmd, int type);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    // friend std::ostream& operator<<(std::ostream& s,const FileLocker& p)
    //	{ p.print(s); return s; }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
