/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Seconds.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_Seconds_h
#define eckit_Seconds_h

#include <sys/time.h>
#include <ctime>
#include <iosfwd>

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// class Bless;

class Seconds {
public:

    // -- Contructors

    Seconds(double, bool compact = false);
    Seconds(const struct ::timeval&, bool compact = false);

    // #include "eckit/types/Seconds.b"

    // -- Operators

    operator std::string() const;
    operator double() const { return seconds_; }

    friend std::ostream& operator<<(std::ostream&, const Seconds&);

private:

    // There is no private copy constructor as this will confuse g++ 4.x.x

    // -- Members

    double seconds_;
    bool compact_;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
