/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ClimateDate.h
// Baudouin Raoult - ECMWF Sep 96

#ifndef eckit_ClimateDate_h
#define eckit_ClimateDate_h

#include "eckit/persist/Bless.h"
#include "eckit/types/Date.h"

namespace eckit {

//--------------------------------------------------------------------------------------------------

class ClimateDate {
public:

    // -- Contructors

    ClimateDate();
    ClimateDate(const Date& date);
    ClimateDate(const std::string& s);
    ClimateDate(long, long, long);

#include "eckit/types/ClimateDate.b"

    // -- Destructor

    ~ClimateDate();

    // -- Convertors

    operator std::string() const;

    // -- Operators

    bool operator==(const ClimateDate& other) const { return date_ == other.date_; }

    bool operator!=(const ClimateDate& other) const { return date_ != other.date_; }

    bool operator<(const ClimateDate& other) const { return date_ < other.date_; }

    bool operator>(const ClimateDate& other) const { return date_ > other.date_; }

    bool operator<=(const ClimateDate& other) const { return date_ <= other.date_; }

    bool operator>=(const ClimateDate& other) const { return date_ >= other.date_; }

    // -- Methods

    const Date& date() { return date_; }

    void dump(DumpLoad&) const;
    void load(DumpLoad&);

    // -- Class methods

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const ClimateDate& date) {
        date.print(s);
        return s;
    }

private:

    // -- Members

    Date date_;

    // -- Methods

    void print(std::ostream&) const;

    // -- Class methods
};

//--------------------------------------------------------------------------------------------------

}  // namespace eckit


#endif
