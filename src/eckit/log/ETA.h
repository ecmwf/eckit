// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
