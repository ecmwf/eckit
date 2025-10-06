/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ETA.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_ETA_h
#define eckit_ETA_h

#include <sys/time.h>

#include <iosfwd>

namespace eckit {

//--------------------------------------------------------------------------------------------------

class ETA {
public:

    // -- Contructors

    ETA(double);
    ETA(const struct ::timeval&);

    // -- Operators

    operator std::string() const;

    friend std::ostream& operator<<(std::ostream&, const ETA&);

private:

    // There is no private copy constructor as this will confuse g++ 4.x.x

    // -- Members

    double ETA_;
};


//--------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
