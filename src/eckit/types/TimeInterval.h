/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TimeInterval.h
// Manuel Fuentes - ECMWF Oct 96

#ifndef eckit_TimeInterval_h
#define eckit_TimeInterval_h

#include "eckit/types/DateTime.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TimeInterval {
public:

    // -- Exceptions
    // None

    // -- Contructors

    TimeInterval(const DateTime& b = DateTime(), const DateTime& e = DateTime());
    TimeInterval(const DateTime& b, const Second& interval);

    // -- Convertors

    const DateTime& begin() const { return begin_; }
    const DateTime& end() const { return end_; }

    // -- Operators
    bool operator<(const TimeInterval& other) const { return begin_ < other.begin_; }

    // -- Methods
    TimeInterval intersect(const TimeInterval& other) const;
    bool empty() const { return begin_ > end_; }

private:

    // -- Members

    DateTime begin_;
    DateTime end_;

    // -- Methods
    void print(std::ostream&) const;

    // -- Friends
    friend std::ostream& operator<<(std::ostream& s, const TimeInterval& p) {
        p.print(s);
        return s;
    }

    friend bool operator==(const TimeInterval& p1, const TimeInterval& p2) {
        return p1.begin_ == p2.begin_ && p1.end_ == p2.end_;
    }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
