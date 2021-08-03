/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <locale>

#include "eckit/eckit.h"

#include "eckit/types/DateTime.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
DateTime::DateTime(const Date& d, const Time& t) :
    date_(d), time_(t) {}

DateTime::DateTime(const DateTime& other) :
    date_(other.date_), time_(other.time_) {}

DateTime::DateTime(double julian) :
    date_(long(julian), true), time_((julian - long(julian)) * 24 * 60 * 60) {}

DateTime::DateTime(const std::string& s) {
    Tokenizer parse(" ");
    std::vector<std::string> result;

    parse(s, result);

    ASSERT(result.size() == 2);
    date_ = Date(result[0]);
    time_ = Time(result[1]);
}

static std::locale& getLocale() {
    static std::locale loc = std::locale::classic();
    try {
        loc = std::locale("");
    }
    catch (...) {
        Log::error() << "Problem to setup the locale\n"
                     << "Check your LANG variable - current value: " << getenv("LANG") << std::endl;
        loc = std::locale::classic();
    }
    return loc;
}

std::string DateTime::format(const std::string& fmt) {
    std::ostringstream out;

    struct tm convert = {};
    convert.tm_mday   = date().day();
    convert.tm_mon    = date().month() - 1;
    convert.tm_year   = date().year() - 1900;

    int mm = date().month();
    int yy = date().year();
    if (mm < 3) {
        mm += 12;
        --yy;
    }

    convert.tm_wday = (date().day() + (13 * mm - 27) / 5 + yy + yy / 4 - yy / 100 + yy / 400) % 7;
    convert.tm_sec  = time().seconds();
    convert.tm_min  = time().minutes();
    convert.tm_hour = time().hours();

    out.imbue(getLocale());

    const std::time_put<char>& tfac = std::use_facet<std::time_put<char> >(getLocale());

    tfac.put(out, out, ' ', &convert, fmt.c_str(), fmt.c_str() + fmt.length());

    return out.str();
}

DateTime::DateTime(time_t thetime) {

    // prefer reentrant version ( gmtime_r )

#ifdef eckit_HAVE_GMTIME_R
    struct tm t;
    gmtime_r(&thetime, &t);
    long td   = (1900 + t.tm_year) * 10000 + (t.tm_mon + 1) * 100 + t.tm_mday;
    long hour = t.tm_hour;
    long min  = t.tm_min;
    long sec  = t.tm_sec;
#else
    struct tm* t;
    t         = gmtime(&thetime);
    long td   = (1900 + t->tm_year) * 10000 + (t->tm_mon + 1) * 100 + t->tm_mday;
    long hour = t->tm_hour;
    long min  = t->tm_min;
    long sec  = t->tm_sec;
#endif

    date_ = Date(td);
    time_ = Time(hour, min, sec);
}

void DateTime::print(std::ostream& s) const {
    s << date_ << ' ' << time_;
}

DateTime::operator std::string() const {
    std::ostringstream os;
    os << *this;
    return os.str();
}

DateTime& DateTime::operator=(const DateTime& other) {
    date_ = other.date_;
    time_ = other.time_;
    return *this;
}

Second DateTime::operator-(const DateTime& other) const {
    Second date = (date_ - other.date_) * 24 * 3600;
    Second time = time_ - other.time_;

    return date + time;
}

DateTime DateTime::operator+(const Second& s) const {
    Date d = date();
    long t = time();
    d += long(s) / (24 * 3600);
    t += long(s) % (24 * 3600);
    while (t >= 3600 * 24) {
        d += 1;
        t -= 3600 * 24;
    }

    return DateTime(d, Second(t));
}

DateTime DateTime::round(const Second& rnd) const {
    long long seconds = double(date_.julian_) * 24.0 * 3600 + Second(time_);
    seconds           = (seconds / long(rnd)) * rnd;

    long d   = seconds / (3600 * 24);
    Second t = seconds % (3600 * 24);

    return DateTime(Date(d, true), Time(t));
}

void DateTime::dump(DumpLoad& a) const {
    date_.dump(a);
    time_.dump(a);
}

void DateTime::load(DumpLoad& a) {
    date_.load(a);
    time_.load(a);
}

std::string DateTime::iso(bool UTC) const {
    std::stringstream s;
    s << date_ << "T" << time_;
    if (UTC)
        s << "Z";
    return s.str();
}

void DateTime::hash(Hash& h) const {
    date_.hash(h);
    time_.hash(h);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
