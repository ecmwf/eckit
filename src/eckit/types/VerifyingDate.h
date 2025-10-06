/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

    VerifyingDate(time_t = ::time(0));
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
