/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cctype>
#if 0
#include <charconv>
#endif
#include <cmath>
#include <cstddef>
#include <string>
#include <string_view>

#include "eckit/exception/Exceptions.h"
#include "eckit/persist/DumpLoad.h"
#include "eckit/types/Time.h"
#include "eckit/utils/Hash.h"
#include "eckit/utils/Tokenizer.h"

namespace {

//----------------------------------------------------------------------------------------------------------------------

enum class TimeFormat {
    INVALID,
    UNITS,
    COLON,
    DIGITS,
    DECIMAL
};

struct tokenized_time {
    TimeFormat type;
    bool minus{false};
    std::string_view integers[4];
};

tokenized_time checkTimeFormat(const std::string_view time) {
    tokenized_time tt;
    tt.minus     = time[0] == '-';
    size_t start = tt.minus ? 1 : 0;

    bool hasDigit   = false;
    bool hasDecimal = false;
    bool hasColon   = false;
    bool hasUnits   = false;

    size_t tokenId    = 0;
    size_t tokenStart = start;

    for (auto i = start; i < time.length(); i++) {
        switch (std::tolower(time[i])) {
            case 'd': {
                if (hasColon || hasDecimal || tokenId > 0) {
                    tt.type = TimeFormat::INVALID;
                    return tt;
                }
                hasUnits       = true;
                tt.type        = TimeFormat::UNITS;
                tt.integers[0] = time.substr(tokenStart, i - tokenStart);
                tokenId        = 1;
                tokenStart     = i + 1;
                break;
            }
            case 'h': {
                if (hasColon || hasDecimal || tokenId > 1) {
                    tt.type = TimeFormat::INVALID;
                    return tt;
                }
                hasUnits       = true;
                tt.type        = TimeFormat::UNITS;
                tt.integers[1] = time.substr(tokenStart, i - tokenStart);
                tokenId        = 2;
                tokenStart     = i + 1;
                break;
            }
            case 'm': {
                if (hasColon || hasDecimal || tokenId > 2) {
                    tt.type = TimeFormat::INVALID;
                    return tt;
                }
                hasUnits       = true;
                tt.type        = TimeFormat::UNITS;
                tt.integers[2] = time.substr(tokenStart, i - tokenStart);
                tokenId        = 3;
                tokenStart     = i + 1;
                break;
            }
            case 's': {
                if (hasColon || hasDecimal || tokenId > 3) {
                    tt.type = TimeFormat::INVALID;
                    return tt;
                }
                hasUnits       = true;
                tt.type        = TimeFormat::UNITS;
                tt.integers[3] = time.substr(tokenStart, i - tokenStart);
                tokenId        = 4;
                tokenStart     = i + 1;
                break;
            }
            case '.': {
                if (hasDecimal || hasColon || hasUnits || i == time.length() - 1) {
                    tt.type = TimeFormat::INVALID;
                    return tt;
                }
                hasDecimal = true;
                break;
            }
            case ':': {
                if (!hasColon) {  // First colon, skip the dd token
                    tt.type  = TimeFormat::COLON;
                    hasColon = true;
                    tokenId  = 1;
                }
                if (tokenId > 2 || i <= tokenStart) {
                    tt.type = TimeFormat::INVALID;
                    return tt;
                }
                tt.integers[tokenId] = time.substr(tokenStart, i - tokenStart);

                tokenId++;
                tokenStart = i + 1;
                break;
            }
            default: {
                if (!isdigit(time[i])) {
                    tt.type = TimeFormat::INVALID;
                    return tt;
                }
            }
        }
    }
    if (hasColon) {
        if (tokenStart < time.length()) {
            tt.integers[tokenId] = time.substr(tokenStart, time.length() - tokenStart);
        }
    }
    else if (!hasUnits) {
        tt.integers[1] = time.substr(tokenStart);
        tt.type        = hasDecimal ? TimeFormat::DECIMAL : TimeFormat::DIGITS;
    }
    return tt;
}

long s2l(const std::string_view& str) {
    if (str.empty()) {
        return 0;
    }
    long result;
#if 0
    auto [ptr, err] = std::from_chars(str.data(), str.data() + str.size(), result);
    ASSERT(err == std::errc());
#else
    result = std::stol(std::string(str));  // Clang does not support std::from_chars
#endif
    return result;
}
double s2d(const std::string_view& str) {
    if (str.empty()) {
        return 0;
    }
    double result;
#if 0
    auto [ptr, err] = std::from_chars(str.data(), str.data() + str.size(), result);
    ASSERT(err == std::errc());
#else
    result = std::stod(std::string(str));  // Clang does not support std::from_chars
#endif
    return result;
}

void printTime(std::ostream& s, long n) {
    if (n < 10) {
        s << '0';
    }
    s << n;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

Time::Time(Second seconds, bool extended) : seconds_(seconds) {
    if ((seconds >= Second{86400} && !extended) || seconds < Second{0}) {
        std::string msg = "Time in seconds must be positive and less than 86400 seconds (24h): ";
        msg += std::to_string(seconds);
        throw BadTime(msg);
    }
}
Time::Time(long seconds, bool extended) : Time(static_cast<Second>(seconds), extended) {}
Time::Time(int seconds, bool extended) : Time(static_cast<Second>(seconds), extended) {}

Time::Time(const std::string& s, bool extended) {
    long ss = 0;
    long mm = 0;
    long hh = 0;
    long dd = 0;

    auto tt = checkTimeFormat(s);

    switch (tt.type) {
        case TimeFormat::DIGITS: {
            long t   = (tt.minus ? -1 : 1) * s2l(tt.integers[1]);
            int sign = (tt.minus ? 1 : 0);
            if (extended || s.length() <= 2 + sign) {  // cases: h, hh, (or hhh..h for step parsing)
                hh = t;
            }
            else {
                if (s.length() <= 4 + sign) {  // cases: hmm, hhmm
                    hh = t / 100;
                    mm = t % 100;
                }
                else {  // cases: hmmss, hhmmss
                    hh = t / 10000;
                    mm = (t / 100) % 100;
                    ss = t % 100;
                }
            }
            break;
        }
        case TimeFormat::DECIMAL: {
            long sec = std::lround((tt.minus ? -1 : 1) * s2d(tt.integers[1]) * 3600);
            hh       = sec / 3600;
            sec -= hh * 3600;
            mm = sec / 60;
            sec -= mm * 60;
            ss = sec;
            break;
        }
        case TimeFormat::COLON:
            hh = s2l(tt.integers[1]);
            mm = s2l(tt.integers[2]);
            ss = s2l(tt.integers[3]);
            break;
        case TimeFormat::UNITS:
            dd = s2l(tt.integers[0]);
            hh = s2l(tt.integers[1]);
            mm = s2l(tt.integers[2]);
            ss = s2l(tt.integers[3]);

            ss += 60 * (mm + 60 * (hh + 24 * dd));
            if (tt.minus) {
                ss = -ss;
            }
            dd = ss / 86400;
            hh = (ss / 3600) % 24;
            mm = (ss / 60) % 60;
            ss = ss % 60;

            break;
        case TimeFormat::INVALID:
        default:
            throw BadTime("Unkown format for time: " + s);
    }

    if (mm >= 60 || ss >= 60 || (!extended && (hh >= 24 || dd > 0 || hh < 0 || mm < 0 || ss < 0))) {
        std::string msg = "Wrong input for time: ";
        if (dd > 0) {
            msg += std::to_string(dd);
            msg += " days ";
        }
        msg += std::to_string(hh);
        msg += " hours ";
        msg += std::to_string(mm);
        msg += " minutes ";
        msg += std::to_string(ss);
        msg += " seconds";
        throw BadTime(msg);
    }
    seconds_ = dd * 86400 + hh * 3600 + mm * 60 + ss;
}

Time::operator std::string() const {
    std::ostringstream os;
    os << *this;
    return os.str();
}

Time::Time(const Time& other) : seconds_(other.seconds_) {}

Time& Time::operator=(const Time& other) {
    seconds_ = other.seconds_;
    return *this;
}

Time::Time(long hh, long mm, long ss, bool extended) : seconds_(hh * 3600 + mm * 60 + ss) {
    if (mm >= 60 || ss >= 60 || (!extended && (hh >= 24 || hh < 0 || mm < 0 || ss < 0))) {
        std::string msg = "Wrong input for time: ";
        msg += std::to_string(hh);
        msg += " hours ";
        msg += std::to_string(mm);
        msg += " minutes ";
        msg += std::to_string(ss);
        msg += " seconds";
        throw BadTime(msg);
    }
}

Time::~Time() {}

bool Time::operator==(const Time& other) const {
    return seconds_ == other.seconds_;
}

bool Time::operator!=(const Time& other) const {
    return (seconds_ != other.seconds_);
}

long Time::hours() const {
    long l = std::lround(seconds_);
    return l / 3600;
}

long Time::minutes() const {
    long l = std::lround(seconds_);
    return (l % 3600) / 60;
}

long Time::seconds() const {
    long l = std::lround(seconds_);
    return l % 60;
}

long Time::hhmmss() const {
    return hours() * 10000 + minutes() * 100 + seconds();
}

void Time::print(std::ostream& s) const {
    printTime(s, hours());
    s << ':';
    printTime(s, minutes());
    s << ':';
    printTime(s, seconds());
}

Time Time::now() {
    time_t now;
    time(&now);
    struct tm* pt;

#if eckit_HAVE_GMTIME_R
    struct tm t;
    gmtime_r(&now, &t);
    pt = &t;
#else
    struct tm* t;
    t  = gmtime(&now);
    pt = t;
#endif

    return Time(pt->tm_hour, pt->tm_min, pt->tm_sec);
}

BadTime::BadTime(const std::string& s) : BadValue(s) {}

void Time::dump(DumpLoad& a) const {
    a.dump(seconds_);
}

void Time::load(DumpLoad& a) {
    a.load(seconds_);
}

void Time::hash(Hash& h) const {
    h.add(seconds_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
