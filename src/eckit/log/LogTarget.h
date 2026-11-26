// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
