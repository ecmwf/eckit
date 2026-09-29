// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
