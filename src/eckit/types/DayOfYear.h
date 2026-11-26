// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File DayOfYear.h
// Baudouin Raoult - ECMWF Sep 96

#ifndef eckit_DayOfYear_h
#define eckit_DayOfYear_h

#include "eckit/types/Date.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class DayOfYear {
public:

    // -- Contructors

    DayOfYear(const Date& = Date(0));
    DayOfYear(long, long);
    DayOfYear(const std::string&);

#include "eckit/types/DayOfYear.b"

    // -- Destructor

    ~DayOfYear() {}

    // -- Convertors

    operator std::string() const;

    // -- Operators

    bool operator==(const DayOfYear& other) const { return value_ == other.value_; }

    bool operator!=(const DayOfYear& other) const { return value_ != other.value_; }

    bool operator<(const DayOfYear& other) const { return value_ < other.value_; }

    bool operator>(const DayOfYear& other) const { return value_ > other.value_; }

    bool operator<=(const DayOfYear& other) const { return value_ <= other.value_; }

    bool operator>=(const DayOfYear& other) const { return value_ >= other.value_; }


    // -- Methods

    void dump(DumpLoad&) const;
    void load(DumpLoad&);

    // -- Class methods


    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const DayOfYear& month) {
        month.print(s);
        return s;
    }

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

    long value_;

    // -- Methods

    void print(std::ostream&) const;

    // -- Class methods
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
