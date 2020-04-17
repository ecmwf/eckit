/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Manuel Fuentes
/// @author Tiago Quintino
/// @date   Oct 96

#ifndef eckit_Time_h
#define eckit_Time_h

#include "eckit/exception/Exceptions.h"
#include "eckit/persist/Bless.h"

namespace eckit {

class DumpLoad;
class Bless;
class Hash;

typedef double Second;

//----------------------------------------------------------------------------------------------------------------------

class Time {

public:  // methods
    Time(long, long, long);
    Time(long seconds = 0);
    Time(const std::string&);

#include "eckit/types/Time.b"

    Time(const Time&);
    Time& operator=(const Time&);

    ~Time();

    operator std::string() const;

    operator Second() const { return seconds_; }

    bool operator==(const Time& other) const;

    bool operator!=(const Time& other) const;

    bool operator>(const Time& other) const { return (seconds_ > other.seconds_); }

    bool operator<(const Time& other) const { return (seconds_ < other.seconds_); }

    bool operator>=(const Time& other) const { return (seconds_ >= other.seconds_); }

    bool operator<=(const Time& other) const { return (seconds_ <= other.seconds_); }

    Second operator-(const Time& other) const { return seconds_ - other.seconds_; }

    //  Does not make sense
    //	Time operator+(const Time& other) const
    //		{ return seconds_ + other.seconds_; }

    Time& operator+=(const Second& sec) {
        seconds_ += sec;
        return *this;
    }

    Time& operator-=(const Second& sec) {
        seconds_ -= sec;
        return *this;
    }

    long hours() const;
    long minutes() const;
    long seconds() const;
    long hhmmss() const;

    void dump(DumpLoad&) const;
    void load(DumpLoad&);

    void hash(Hash&) const;

    static Time now();

protected:  // methods

    void print(std::ostream&) const;

private:  // members

    Second seconds_;

    friend std::ostream& operator<<(std::ostream& s, const Time& t) {
        t.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

class BadTime : public BadValue {
public:
    BadTime(const std::string& t);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
