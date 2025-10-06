/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Monitorable.h
// Baudouin Raoult - ECMWF Mar 97

#ifndef eckit_Monitorable_h
#define eckit_Monitorable_h

#include "eckit/container/ClassExtent.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class JSON;

class Monitorable : public ClassExtent<Monitorable> {
public:

    Monitorable();

    // -- Destructor

    virtual ~Monitorable();

    // -- Methods

    virtual void json(JSON&) const = 0;
    virtual void status(std::ostream&) const;

    // -- Class methods

    static void allStatuses(std::ostream&);
    static void allJSON(JSON&);

protected:

    // -- Methods

    virtual void print(std::ostream&) const;

private:

    friend std::ostream& operator<<(std::ostream& s, const Monitorable& p) {
        p.print(s);
        return s;
    }
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
