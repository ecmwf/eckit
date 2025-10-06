/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file LogTarget.h
/// @author Tiago Quintino

#ifndef eckit_log_LogTarget_h
#define eckit_log_LogTarget_h

#include <iosfwd>

#include "eckit/memory/Counted.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class LogTarget : public Counted {
public:  // methods

    LogTarget();

    virtual void write(const char* start, const char* end) = 0;
    virtual void flush()                                   = 0;

    ~LogTarget() override;

protected:

    friend std::ostream& operator<<(std::ostream& os, const LogTarget& c) {
        c.print(os);
        return os;
    }

    virtual void print(std::ostream& s) const;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
