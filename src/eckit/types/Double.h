// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Dec 1997

#ifndef eckit_Double_h
#define eckit_Double_h

#include <string>

#include "eckit/persist/Bless.h"

namespace eckit {

//-----------------------------------------------------------------------------

class DumpLoad;

class Double {
public:

    Double(double = 0);
    Double(const std::string&);

#include "eckit/types/Double.b"

    ~Double() {}

public:  // operators

    operator std::string() const;

    bool operator==(const Double& other) const { return round() == other.round(); }

    bool operator!=(const Double& other) const { return round() != other.round(); }

    bool operator<(const Double& other) const { return round() < other.round(); }

    bool operator>(const Double& other) const { return round() > other.round(); }

public:  // methods

    void dump(DumpLoad&) const;
    void load(DumpLoad&);

    double round() const;

protected:  // methods

    void print(std::ostream& s) const;

private:  // members

    double value_;

    friend std::ostream& operator<<(std::ostream& s, const Double& p) {
        p.print(s);
        return s;
    }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
