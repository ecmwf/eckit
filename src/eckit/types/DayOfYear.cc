/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iomanip>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/types/DayOfYear.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static long date2Value(const Date& date) {
    Date jan01(date.year(), 1, 1);
    Date feb28(date.year(), 2, 28);
    Date nextJan01(date.year() + 1, 1, 1);

    int numberOfDays = nextJan01 - jan01;

    long value = date - jan01;

    if (numberOfDays == 365 && value > feb28 - jan01)
        value++;

    return value;
}

DayOfYear::DayOfYear(const Date& date) :
    value_(date2Value(date)) {}

DayOfYear::DayOfYear(const std::string& s) :
    value_(date2Value(Date(s))) {}

DayOfYear::operator std::string() const {
    std::ostringstream os;
    os << *this;
    return os.str();
}

void DayOfYear::print(std::ostream& s) const {
    Date d(2004, value_ + 1);  // Take a leap year so it can accept 29th feb

    char oldfill = s.fill();
    s << std::setw(2) << std::setfill('0') << d.month() << '-' << std::setw(2) << std::setfill('0') << d.day()
      << std::setfill(oldfill);
}

void DayOfYear::dump(DumpLoad&) const {
    NOTIMP;
}

void DayOfYear::load(DumpLoad&) {
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
