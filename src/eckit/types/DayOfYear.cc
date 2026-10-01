// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

    if (numberOfDays == 365 && value > feb28 - jan01) {
        value++;
    }

    return value;
}

DayOfYear::DayOfYear(const Date& date) : value_(date2Value(date)) {}

DayOfYear::DayOfYear(const std::string& s) : value_(date2Value(Date(s))) {}

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
