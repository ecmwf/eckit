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

#include "eckit/exception/Exceptions.h"
#include "eckit/types/ClimateDate.h"

namespace eckit {


// Climate date is a 30 day date

static Date makeDate(long year, long month, long day) {
    ASSERT(month >= 1 && month <= 12);
    ASSERT(day >= 1 && day <= 30);

    long m = (month - 1) * 30 + day;
    ASSERT(m >= 1 && m <= 12 * 30);

    return Date(year, m);
}

ClimateDate::ClimateDate(long year, long month, long day) :
    date_(makeDate(year, month, day)) {}

ClimateDate::~ClimateDate() {}

static Date makeDate(const std::string& s) {
    Date date(s);
    long year = date.year();
    Date first(year, 1, 1);
    long day = (date - first + 1);

    ASSERT(day >= 1 && day <= 12 * 30);

    return Date(year, day);
}

ClimateDate::ClimateDate() {}

ClimateDate::ClimateDate(const Date& date) :
    date_(date) {}

ClimateDate::ClimateDate(const std::string& s) :
    date_(makeDate(s)) {}

ClimateDate::operator std::string() const {
    std::ostringstream os;
    os << *this;
    return os.str();
}

void ClimateDate::print(std::ostream& s) const {
    long year      = date_.year();
    long dayOfYear = date_ - Date(year, 1, 1);
#if 0
	long month     = dayOfYear / 30 + 1;
	long day       = dayOfYear % 30 + 1;

	char oldfill = s.fill();
	s << year << '-' << std::setw(2) << std::setfill('0') << month
		<< '-' << std::setw(2) << std::setfill('0') << day << std::setfill(oldfill);
#else

    char oldfill = s.fill();
    s << year << '-' << std::setw(3) << std::setfill('0') << (dayOfYear + 1) << std::setfill(oldfill);
#endif
}

void ClimateDate::dump(DumpLoad& a) const {
    date_.dump(a);
}

void ClimateDate::load(DumpLoad& a) {
    date_.load(a);
}

}  // namespace eckit
