/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Grid.h
// Baudouin Raoult - ECMWF Dec 97

#ifndef eckit_Grid_h
#define eckit_Grid_h

#include <string>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/persist/Bless.h"

namespace eckit {

//-----------------------------------------------------------------------------

class DumpLoad;

class Grid {
public:

    enum {
        undef = -1
    };

    // -- Exceptions

    class InvalidGrid : public Exception {
    public:

        InvalidGrid(const std::string& s);
    };

    // -- Contructors

    Grid(const std::string&);

    Grid(double ns = undef, double ew = undef) : northSouth_(ns), eastWest_(ew) {}

    Grid(const std::vector<double>&);

#include "eckit/types/Grid.b"

    // -- Destructor

    ~Grid() {}

    // -- Convertors
    // None

    // -- Operators
    // None

    operator std::string() const;

    bool operator==(const Grid& other) const {
        return northSouth_ == other.northSouth_ && eastWest_ == other.eastWest_;
    }

    bool operator!=(const Grid& other) const {
        return northSouth_ != other.northSouth_ || eastWest_ != other.eastWest_;
    }

    bool operator<(const Grid& other) const;

    bool operator>(const Grid& other) const { return other < *this; }

    // -- Methods

    double northSouth() const { return northSouth_; }
    double eastWest() const { return eastWest_; }

    // Score return 2 if the grid is a multiple of the parameter
    // 1 if only one value is a multiple
    // or 0 if the grid are not compatible.

    long score(const Grid&) const;
    double distance(const Grid&) const;
    const Grid& bestMatch(const std::vector<Grid>&) const;

    bool undefined() const { return northSouth_ == undef && eastWest_ == undef; }

    void dump(DumpLoad&) const;
    void load(DumpLoad&);

    // -- Overridden methods
    // None

    // -- Class members

    static char gridName(const Grid&);
    static Grid grid(const char);

    // -- Class methods
    // None

protected:

    // -- Members
    // None

    // -- Methods

    void print(std::ostream& s) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed

    // -- Members

    double northSouth_;
    double eastWest_;


    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Grid& p) {
        p.print(s);
        return s;
    }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
