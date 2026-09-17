// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
