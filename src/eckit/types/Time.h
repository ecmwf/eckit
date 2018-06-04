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



namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class DumpLoad;
class Bless;
class Hash;

// Forwarded declarations

typedef double Second;

class Time {

public:

// -- Exceptions
	// None

// -- Contructors

	Time(long, long, long);
	Time(long seconds = 0);
	Time(const std::string&);

#include "eckit/types/Time.b"

// -- Copy

	Time(const Time&);
	Time& operator=(const Time&);

// -- Destructor

	~Time();

// -- Convertors

	operator std::string() const;
	operator Second() const { return seconds_; }

// -- Operators

	bool operator==(const Time& other) const
		{ return seconds_ == other.seconds_; }

	bool operator!=(const Time& other) const
		{ return (seconds_ != other.seconds_); }

	bool operator>(const Time& other) const
		{ return (seconds_ > other.seconds_); }

	bool operator<(const Time& other) const
		{ return (seconds_ < other.seconds_); }

	bool operator>=(const Time& other) const
		{ return (seconds_ >= other.seconds_); }

	bool operator<=(const Time& other) const
		{ return (seconds_ <= other.seconds_); }

	Second operator-(const Time& other) const
		{ return seconds_ - other.seconds_; }

//  Does not make sens
//	Time operator+(const Time& other) const
//		{ return seconds_ + other.seconds_; }

	Time& operator+=(const Second& sec)
		{ seconds_ += sec; return *this; }

	Time& operator-=(const Second& sec)
		{ seconds_ -= sec; return *this; }

// -- Methods
	long hours() const;
	long minutes() const;
	long seconds() const;
	long hhmmss() const;

	void dump(DumpLoad&) const;
	void load(DumpLoad&);

    void hash(Hash&) const;

// -- Class Methods

	static Time now();

protected:

// -- Methods

	void print(std::ostream&) const;

private:

// -- Members

	Second seconds_;

	friend std::ostream& operator<<(std::ostream& s,const Time& t)
		{ t.print(s); return s; }

};


class BadTime: public BadValue {
public:
	BadTime(const std::string& t);

};


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
