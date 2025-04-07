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
/// @author Tiago Quintino
/// @date   Sep 96


#ifndef eckit_Date_h
#define eckit_Date_h

#include "eckit/persist/Bless.h"

namespace eckit {

//--------------------------------------------------------------------------------------------------

class DumpLoad;
class Bless;
class Hash;

class Date {
public:

    enum {
        MONDAY    = 0,
        TUESDAY   = 1,
        WEDNESDAY = 2,
        THURSDAY  = 3,
        FRIDAY    = 4,
        SATURDAY  = 5,
        SUNDAY    = 6
    };

    // -- Contructors

    Date() : julian_(0) {}
    Date(long);
    Date(long, long, long);
    Date(const std::string&);
    Date(long, long);

#include "eckit/types/Date.b"

    // -- Copy

    Date(const Date& other) : julian_(other.julian_) {}

    Date& operator=(const Date& other) {
        julian_ = other.julian_;
        return *this;
    }

    // -- Destructor

    ~Date() {}

    // -- Convertors

    operator std::string() const;

    // -- Operators

    bool operator==(const Date& other) const { return julian_ == other.julian_; }

    bool operator!=(const Date& other) const { return julian_ != other.julian_; }

    bool operator<(const Date& other) const { return julian_ < other.julian_; }

    bool operator>(const Date& other) const { return julian_ > other.julian_; }

    bool operator<=(const Date& other) const { return julian_ <= other.julian_; }

    bool operator>=(const Date& other) const { return julian_ >= other.julian_; }


    Date& operator++() {
        julian_++;
        return *this;
    }
    Date& operator--() {
        julian_--;
        return *this;
    }

    Date& operator+=(long d) {
        julian_ += d;
        return *this;
    }
    Date& operator-=(long d) {
        julian_ -= d;
        return *this;
    }

    // -- Methods

    long year() const;
    long month() const;
    long day() const;
    long yyyymmdd() const;

    long julian() const { return julian_; }
    Date round(int n) { return Date((julian_ / n) * n, true); }
    std::string monthName() const;
    long dayOfWeek() const { return julian_ % 7; }

    void dump(DumpLoad&) const;
    void load(DumpLoad&);

    void hash(Hash&) const;

    // -- Class methods

    static long parse(const std::string&);

    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Date& date) {
        date.print(s);
        return s;
    }

protected:

    Date(long julian, bool) : julian_(julian) {}

private:

    // -- Members

    long julian_;

    // -- Methods

    void print(std::ostream&) const;

    // -- Class methods

    static long julianToDate(long);
    static long dateToJulian(long);
    static long today();

    // -- Friends

    friend long operator-(const Date& d1, const Date& d2) { return (d1.julian_ - d2.julian_); }

    //	friend long operator-(const Date& d1)
    //		{ NOTIMP; return 0; }

    friend Date operator+(const Date& d1, const long n) { return Date::julianToDate(d1.julian_ + n); }

    friend Date operator+(const long n, const Date& d1) { return d1 + n; }


    friend class DateTime;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
