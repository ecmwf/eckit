// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Sep 96

#ifndef eckit_DateTime_h
#define eckit_DateTime_h


#include "eckit/types/Date.h"
#include "eckit/types/Time.h"


namespace eckit {

class Hash;

//----------------------------------------------------------------------------------------------------------------------

class DateTime {
public:

    // Contructors

    DateTime(time_t = ::time(nullptr));
    DateTime(const Date&, const Time&);
    DateTime(const std::string&);
    DateTime(double);
    DateTime(const DateTime& other);

#include "eckit/types/DateTime.b"

    ~DateTime() {}

    bool operator<(const DateTime& other) const {
        return (date_ == other.date_) ? (time_ < other.time_) : (date_ < other.date_);
    }

    bool operator==(const DateTime& other) const { return (date_ == other.date_) && (time_ == other.time_); }

    bool operator!=(const DateTime& other) const { return (date_ != other.date_) || (time_ != other.time_); }

    bool operator>(const DateTime& other) const {
        return (date_ == other.date_) ? (time_ > other.time_) : (date_ > other.date_);
    }

    bool operator>=(const DateTime& other) const { return !(*this < other); }

    bool operator<=(const DateTime& other) const { return !(*this > other); }

    DateTime& operator=(const DateTime&);

    Second operator-(const DateTime&) const;
    DateTime operator+(const Second&) const;

    operator std::string() const;

    const Date& date() const { return date_; }
    const Time& time() const { return time_; }

    DateTime round(const Second& seconds) const;

    void dump(DumpLoad&) const;
    void load(DumpLoad&);

    std::string format(const std::string& fmt);

    std::string iso(bool UTC = true) const;

    void hash(Hash&) const;

protected:  // members

    Date date_;
    Time time_;

private:  // methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const DateTime& p) {
        p.print(s);
        return s;
    }
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
