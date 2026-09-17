// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Baudouin Raoult - ECMWF Sep 96

#ifndef eckit_VerifyingDate_h
#define eckit_VerifyingDate_h

#include "eckit/types/DateTime.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class VerifyingDate : public DateTime {
public:

    // -- Contructors

    VerifyingDate(time_t = ::time(nullptr));
    VerifyingDate(const Date&, const Time&);
    VerifyingDate(const std::string&);
    VerifyingDate(const DateTime&);

#include "eckit/types/VerifyingDate.b"

    // -- Destructor

    ~VerifyingDate() {}

    // -- Operators

    operator std::string() const;

    // -- Methods

    // -- Class methods

protected:

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // -- Members

    // -- Methods

    void print(std::ostream&) const;

    // -- Class methods

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const VerifyingDate& p) {
        p.print(s);
        return s;
    }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
