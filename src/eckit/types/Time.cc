/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"

#include "eckit/persist/DumpLoad.h"
#include "eckit/types/Time.h"
#include "eckit/utils/Hash.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

inline void printTime(std::ostream& s, long n) {
    if (n < 10)
        s << '0';
    s << n;
}

Time::Time(long seconds) :
    seconds_(seconds) {
    if (seconds >= 86400 || seconds < 0) {
        std::string msg = "Time in seconds must be positive and cannot exceed 86400, seconds: ";
        Translator<long, std::string> t;
        msg += t(seconds);
        throw BadTime(msg);
    }
}

Time::Time(const std::string& s) {
    Tokenizer parse(":");
    std::vector<std::string> result;

    parse(s, result);

    long hh = 0, mm = 0, ss = 0;
    bool err = false;
    long t   = atol(s.c_str());

    switch (result.size()) {
        case 1:
            // hh or hhmm or hhmmss
            switch (s.length()) {
                case 2:
                    hh = t;
                    break;
                case 4:
                    hh = t / 100;
                    mm = t % 100;
                    break;
                case 6:
                    hh = t / 10000;
                    mm = (t % 10000) / 100;
                    ss = (t % 10000) % 100;
                    break;
                default:
                    err = true;
                    break;
            }
            break;

        case 2:
            // hh:mm
            err = result[0].length() != 2 || result[1].length() != 2;

            hh = atol(result[0].c_str());
            mm = atol(result[1].c_str());

            break;

        case 3:
            // hh:mm:ss

            err = result[0].length() != 2 || result[1].length() != 2 || result[2].length() != 2;

            hh = atol(result[0].c_str());
            mm = atol(result[1].c_str());
            ss = atol(result[2].c_str());

            break;

        default:
            err = true;
            break;
    }

    if (err)
        throw BadTime(std::string("Invalid time ") + s);

    if (hh >= 24 || mm >= 60 || ss >= 60 || hh < 0 || mm < 0 || ss < 0) {
        std::string msg = "Wrong input for time: ";
        Translator<long, std::string> t;
        msg += t(hh);
        msg += " hours ";
        msg += t(mm);
        msg += " minutes ";
        msg += t(ss);
        msg += " seconds";
        throw BadTime(msg);
    }

    seconds_ = hh * 3600 + mm * 60 + ss;
}

Time::operator std::string() const {
    std::ostringstream os;
    os << *this;
    return os.str();
}

Time::Time(const Time& other) :
    seconds_(other.seconds_) {}

Time& Time::operator=(const Time& other) {
    seconds_ = other.seconds_;
    return *this;
}

Time::Time(long hh, long mm, long ss) :
    seconds_(hh * 3600 + mm * 60 + ss) {
    if (hh >= 24 || mm >= 60 || ss >= 60 || hh < 0 || mm < 0 || ss < 0) {
        std::string msg = "Wrong input for time: ";
        Translator<long, std::string> t;
        msg += t(hh);
        msg += " hours ";
        msg += t(mm);
        msg += " minutes ";
        msg += t(ss);
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
    long l = seconds_;
    return l / 3600;
}

long Time::minutes() const {
    long l = seconds_;
    return (l % 3600) / 60;
}

long Time::seconds() const {
    long l = seconds_;
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

#ifdef eckit_HAVE_GMTIME_R
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

BadTime::BadTime(const std::string& s) :
    BadValue(s) {}

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
