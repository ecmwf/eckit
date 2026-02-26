/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Dec 1997

#pragma once
#include "eckit/persist/DumpLoad.h"

#include <iosfwd>
#include <string>

namespace eckit {

//-----------------------------------------------------------------------------

class Double {
public:

    Double(double = 0);
    Double(const std::string&);


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
