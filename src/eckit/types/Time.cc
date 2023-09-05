/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <regex>

#include "eckit/eckit.h"

#include "eckit/persist/DumpLoad.h"
#include "eckit/types/Time.h"
#include "eckit/utils/Hash.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

inline void printTime(std::ostream& s, long n) {
    if (n < 10) {
        s << '0';
    }
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

Time::Time(const std::string& s, bool extended) {
    long ss = 0, mm = 0, hh = 0, dd = 0;
    std::smatch m;
    
    if (std::regex_match (s, m, std::regex("^[0-9]+$"))) { // only digits
        long t = std::stol(s);
        if (s.length() < 3) {     // cases: h, hh
            hh = t;
        } else {
            if (s.length() < 5) { // cases: hmm, hhmm
                hh = t / 100;
                mm = t % 100;
            } else {              // cases: hmmss, hhmmss
                hh = t / 10000;
                mm = (t / 100) % 100;
                ss = t % 100;
            }
        }
    }
    else {
        if (std::regex_match (s, m, std::regex("^([0-9]+):([0-5]?[0-9])(:[0-5]?[0-9])?$"))) {
            for (int i=1; i<m.size(); i++) {
                if (m[i].matched) {
                    switch (i) {
                        case 1: hh = std::stol(m[i].str()); break;
                        case 2: mm = std::stol(m[i].str()); break;
                        case 3: std::string aux = m[i].str();
                                aux.erase(0,1);
                                ss = std::stol(aux); break;
                    }
                }
            }
        }
        else {
            if (std::regex_match (s, m, std::regex("^([0-9]+[dD])?([0-9]+[hH])?([0-9]+[mM])?([0-9]+[sS])?$"))) {
                for (int i=1; i<m.size(); i++) {
                    if (m[i].matched) {
                        std::string aux = m[i].str();
                        aux.pop_back();
                        long t = std::stol(aux);
                        switch (i) {
                            case 1: dd = t; break;
                            case 2: hh = t; break;
                            case 3: mm = t; break;
                            case 4: ss = t;
                        }
                    }
                }
                if (extended) {
                    ss += 60 * (mm + 60 * (hh + 24 * dd));
                    dd =  ss / 86400;
                    hh = (ss /  3600) % 24;
                    mm = (ss /    60) % 60;
                    ss =  ss          % 60;
                }
            } else {
                std::string msg = "Wrong input for time: ";
                msg += s;
                throw BadTime(msg);
            }
        }
    }

    if ((!extended && (hh >= 24 || dd > 0)) || mm >= 60 || ss >= 60 || hh < 0 || mm < 0 || ss < 0) {
        std::string msg = "Wrong input for time: ";
        Translator<long, std::string> t;
        if (dd>0) {
            msg += t(dd);
            msg += " days ";
        }
        msg += t(hh);
        msg += " hours ";
        msg += t(mm);
        msg += " minutes ";
        msg += t(ss);
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
