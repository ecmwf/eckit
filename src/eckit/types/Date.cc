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

#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/persist/DumpLoad.h"
#include "eckit/types/Date.h"
#include "eckit/utils/Hash.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

class BadDate : public BadValue {
public:
    BadDate(const std::string& t);
};

//----------------------------------------------------------------------------------------------------------------------

static const char* months[] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};

static void check(const Date& date, long value) {
    if (value <= 999999)
        value += 19000000;

    if (value != date.yyyymmdd()) {
        std::ostringstream os;
        os << "Invalid date " << value << " becomes " << date;
        throw BadDate(os.str());
    }
}


Date::Date(long date) :
    julian_(dateToJulian(date)) {
    if (date > 0)
        check(*this, date);
}

Date::Date(long year, long month, long day) :
    julian_(dateToJulian(year * 10000 + month * 100 + day)) {
    check(*this, year * 10000 + month * 100 + day);
}

// Warning, unchecked...
Date::Date(long year, long dayOfYear) {
    julian_ = dateToJulian(year * 10000 + 101);  // 1 of jan
    julian_ += (dayOfYear - 1);
    ASSERT(this->year() == year);
}

long Date::parse(const std::string& s) {
    Tokenizer parse("-");
    std::vector<std::string> result;

    parse(s, result);

    bool err   = false;
    long value = 0;
    int i;

    switch (result.size()) {
        case 1:
            switch (s.length()) {
                case 3:
                    // For climatology
                    err = true;
                    for (i = 0; i < 12; i++)
                        if (s == months[i]) {
                            value = 1900 * 10000 + (i + 1) * 100 + 1;
                            err   = false;
                            break;
                        }
                    break;

                case 6:
                case 8:
                    value = atol(s.c_str());
                    break;

                default:
                    err = true;
                    break;
            }
            break;


        case 2:

            // Dates as mm-dd
            if (result[0].length() == 2 && result[1].length() == 2) {
                long month = atol(result[0].c_str());
                long day   = atol(result[1].c_str());

                Date date(2004, month, day);  // 2004 is a leap year

                value = date.yyyymmdd();
            }
            else {
                // Dates as yyyy-ddd

                if (result[0].length() != 2 && result[0].length() != 4)
                    err = true;

                if (result[1].length() != 3)
                    err = true;

                {
                    long year = atol(result[0].c_str());
                    long day  = atol(result[1].c_str());

                    Date date(year, 1, 1);
                    date += day - 1;
                    value = date.yyyymmdd();
                }
            }

            break;

            // Dates as yyyy-mm-dd

        case 3:

            if (result[0].length() != 2 && result[0].length() != 4)
                err = true;
            if (result[1].length() != 2)
                err = true;
            if (result[2].length() != 2)
                err = true;

            value = atol(result[0].c_str()) * 10000 + atol(result[1].c_str()) * 100 + atol(result[2].c_str());

            break;


        default:
            err = true;
            break;
    }

    if (err)
        throw BadDate(std::string("Invalid date ") + s);

    // Come back here....
    // temp patch for monthly means
    if ((value % 100) == 0)
        value++;

    return value;
}

Date::Date(const std::string& s) {
    long value = parse(s);
    julian_    = dateToJulian(value);
    check(*this, value);
}

Date::operator std::string() const {
    std::ostringstream os;
    os << *this;
    return os.str();
}

// Returns a date in the format yyyymmdd from a julian number

long Date::julianToDate(long jdate) {
    long x, y, d, m, e;
    long day, month, year;

    x = 4 * jdate - 6884477;
    y = (x / 146097) * 100;
    e = x % 146097;
    d = e / 4;

    x = 4 * d + 3;
    y = (x / 1461) + y;
    e = x % 1461;
    d = e / 4 + 1;

    x = 5 * d - 3;
    m = x / 153 + 1;
    e = x % 153;
    d = e / 5 + 1;

    if (m < 11)
        month = m + 2;
    else
        month = m - 10;


    day  = d;
    year = y + m / 11;

    return year * 10000 + month * 100 + day;
}

long Date::today(void) {
    time_t now;
    time(&now);

#ifdef eckit_HAVE_GMTIME_R
    struct tm t;
    gmtime_r(&now, &t);
    long td = (1900 + t.tm_year) * 10000 + (t.tm_mon + 1) * 100 + t.tm_mday;
#else
    struct tm* t;
    t       = gmtime(&now);
    long td = (1900 + t->tm_year) * 10000 + (t->tm_mon + 1) * 100 + t->tm_mday;
#endif
    return dateToJulian(td);
}

// Returns a julian number from a yyyymmdd date

long Date::dateToJulian(long ddate) {
    long m1, y1, a, b, c, d, j1;

    long month, day, year;

    // Negative dates are relative to today
    if (ddate <= 0)
        return today() + ddate;

    year = ddate / 10000;
    ddate %= 10000;
    month = ddate / 100;
    ddate %= 100;
    day = ddate;

    if (year < 100) {
        //		throw SeriousBug("Please, use 4 digits dates... 2000 is near");
        //      Log::warning() << "Please, use 4 digits dates... 2000 is near" << std::endl;
        year = year + 1900;
    }

    if (month > 2) {
        m1 = month - 3;
        y1 = year;
    }
    else {
        m1 = month + 9;
        y1 = year - 1;
    }
    a  = 146097 * (y1 / 100) / 4;
    d  = y1 % 100;
    b  = 1461 * d / 4;
    c  = (153 * m1 + 2) / 5 + day + 1721119;
    j1 = a + b + c;

    return (j1);
}


void Date::print(std::ostream& s) const {
    long ddate = julianToDate(julian_);
    long month, day, year;

    year = ddate / 10000;
    ddate %= 10000;
    month = ddate / 100;
    ddate %= 100;
    day = ddate;

    char oldfill = s.fill();
    s << year << '-' << std::setw(2) << std::setfill('0') << month << '-' << std::setw(2) << std::setfill('0') << day
      << std::setfill(oldfill);
}

long Date::year() const {
    long ddate = julianToDate(julian_);
    return ddate / 10000;
}

long Date::month() const {
    long ddate = julianToDate(julian_);
    ddate %= 10000;
    return ddate / 100;
}

long Date::day() const {
    long ddate = julianToDate(julian_);
    return ddate % 100;
}

long Date::yyyymmdd() const {
    return julianToDate(julian_);
}

std::string Date::monthName() const {
    long ddate = julianToDate(julian_);
    ddate %= 10000;
    long n = ddate / 100;
    ASSERT(n >= 1 && n <= 12);
    return months[n - 1];
}

BadDate::BadDate(const std::string& s) :
    BadValue(s) {}

void Date::dump(DumpLoad& a) const {
    a.dump(julian_);
}

void Date::load(DumpLoad& a) {
    a.load(julian_);
}

void Date::hash(Hash& h) const {
    h.add(julian_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
