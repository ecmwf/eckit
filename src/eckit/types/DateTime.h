// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Sep 96

#ifndef eckit_DateTime_h
#define eckit_DateTime_h

#include <string>
#include <string_view>

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

    /// Parses a date and time (always UTC), throwing BadValue on malformed input.
    ///
    /// A lenient parser is used, accepting any Date form followed by any Time form, separated by a
    /// space or 'T', the compact YYYYMMDDHHMMSS and YYYYMMDDHHMM forms, and a trailing 'Z'.
    ///
    /// If iso8601 is true (second constructor) only these fixed width forms are accepted, optionally followed by 'Z'
    /// for the ones that have a time:
    ///   YYYYMMDD, YYYY-MM-DD (time is midnight), YYYYMMDDHHMMSS, YYYYMMDDTHHMMSS, YYYY-MM-DDTHH:MM:SS and
    ///   YYYY-MM-DD HH:MM:SS
    DateTime(const std::string& str) { parse(str, false); }

    /// Same as above, but selecting the parser. Using std::string_view for the string (rather than a defaulted
    /// bool on the std::string constructor) keeps calls such as DateTime(date, seconds) unambiguous, as a Date
    /// converts to std::string but not to std::string_view.
    DateTime(std::string_view str, bool iso8601) { parse(std::string(str), iso8601); }

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

    void parse(const std::string&, bool iso8601);

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const DateTime& p) {
        p.print(s);
        return s;
    }
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
