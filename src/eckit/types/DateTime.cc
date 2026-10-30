// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <locale>

#include "eckit/eckit.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/types/DateTime.h"
#include "eckit/utils/Tokenizer.h"

namespace {

bool isDigits(const std::string& s, size_t pos, size_t len) {
    return std::all_of(s.begin() + pos, s.begin() + pos + len, [](unsigned char c) { return std::isdigit(c); });
}

long toLong(const std::string& s, size_t pos, size_t len) {
    return std::stol(s.substr(pos, len));
}

// Matches the pattern, where 'd' is a digit and any other character matches itself
bool matches(const std::string& s, const char* pattern) {
    if (s.length() != std::strlen(pattern)) {
        return false;
    }
    for (size_t i = 0; i < s.length(); ++i) {
        if (pattern[i] == 'd' ? !std::isdigit(static_cast<unsigned char>(s[i])) : s[i] != pattern[i]) {
            return false;
        }
    }
    return true;
}

}  // namespace

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------
DateTime::DateTime(const Date& d, const Time& t) : date_(d), time_(t) {}

DateTime::DateTime(const DateTime& other) : date_(other.date_), time_(other.time_) {}

DateTime::DateTime(double julian) : date_(long(julian), true), time_((julian - long(julian)) * 24 * 60 * 60) {}

void DateTime::parse(const std::string& str, bool iso8601) {
    // A trailing 'Z' is the UTC designator; DateTime is always UTC
    const bool hasZ     = !str.empty() && str.back() == 'Z';
    const std::string s = hasZ ? str.substr(0, str.length() - 1) : str;

    if (iso8601) {
        // Offsets into s of the date (always at 0) and the time, if there is one
        size_t timePos = 0;
        bool extended  = false;

        if (matches(s, "dddddddd")) {
            extended = false;
        }
        else if (matches(s, "dddd-dd-dd")) {
            extended = true;
        }
        else if (matches(s, "dddddddddddddd")) {
            timePos = 8;
        }
        else if (matches(s, "ddddddddTdddddd")) {
            timePos = 9;
        }
        else if (matches(s, "dddd-dd-ddTdd:dd:dd") || matches(s, "dddd-dd-dd dd:dd:dd")) {
            extended = true;
            timePos  = 11;
        }
        else {
            throw BadValue("Invalid ISO 8601 date time: " + str);
        }

        // 'Z' only makes sense after a time
        if (hasZ && timePos == 0) {
            throw BadValue("Invalid ISO 8601 date time: " + str);
        }

        const size_t m = extended ? 5 : 4;  // offset of the month
        const size_t d = extended ? 8 : 6;  // offset of the day
        date_          = Date(toLong(s, 0, 4), toLong(s, m, 2), toLong(s, d, 2));

        if (timePos == 0) {
            time_ = Time(0, 0, 0);
        }
        else {
            const size_t step = extended ? 3 : 2;
            time_ = Time(toLong(s, timePos, 2), toLong(s, timePos + step, 2), toLong(s, timePos + 2 * step, 2));
        }
        return;
    }

    // Compact forms: YYYYMMDDHHMMSS and YYYYMMDDHHMM
    if ((s.length() == 14 || s.length() == 12) && isDigits(s, 0, s.length())) {
        date_ = Date(s.substr(0, 8));
        time_ = Time(s.substr(8));
        return;
    }

    // Separator is a space or an upper case 'T' (ISO 8601); a lower case 't' is not accepted
    // because it can be part of a month name (e.g. "oct")
    Tokenizer parse(s.find(' ') != std::string::npos ? " " : "T");
    std::vector<std::string> result;

    parse(s, result);

    if (result.size() != 2) {
        throw BadValue("Invalid date time (expected a date and a time): " + str);
    }
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

#if eckit_HAVE_GMTIME_R
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
    Second date = (date_ - other.date_) * 86400;
    Second time = time_ - other.time_;

    return date + time;
}

DateTime DateTime::operator+(const Second& s) const {
    Date d    = date();
    double t  = time();
    long days = std::lround(s) / (86400);
    d += days;
    t += s - days * (86400);
    if (t >= 86400) {
        d += 1;
        t -= 86400;
    }

    return DateTime(d, Second(t));
}

DateTime DateTime::round(const Second& rnd) const {
    long long seconds = double(date_.julian_) * 24.0 * 3600 + Second(time_);
    seconds           = (seconds / long(rnd)) * rnd;

    long d   = seconds / (86400);
    Second t = seconds % (86400);

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
    if (UTC) {
        s << "Z";
    }
    return s.str();
}

void DateTime::hash(Hash& h) const {
    date_.hash(h);
    time_.hash(h);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
