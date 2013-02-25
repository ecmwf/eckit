/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ClimateDate.h
// Baudouin Raoult - ECMWF Sep 96

#ifndef eclib_ClimateDate_h
#define eclib_ClimateDate_h

#include "eclib/Date.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------


class ClimateDate {
public:

// -- Contructors

	ClimateDate()                               {}
	ClimateDate(const Date& date) : date_(date) {}
	ClimateDate(const string& s);
	ClimateDate(long,long,long);

#include "eclib/ClimateDate.b"

// -- Destructor

	~ClimateDate() {}

// -- Convertors
	
	operator string() const;

// -- Operators

	bool operator==(const ClimateDate& other) const 
		{ return date_ == other.date_ ;}

	bool operator!=(const ClimateDate& other) const
		{ return date_ != other.date_ ;}

	bool operator<(const ClimateDate& other)  const
		{ return date_ < other.date_ ;}

	bool operator>(const ClimateDate& other)  const
		{ return date_ > other.date_ ;}

	bool operator<=(const ClimateDate& other)  const
		{ return date_ <= other.date_ ;}

	bool operator>=(const ClimateDate& other)  const
		{ return date_ >= other.date_ ;}

// -- Methods

	const Date& date() { return date_ ; }

	void dump(DumpLoad&) const;
	void load(DumpLoad&);

// -- Class methods

// -- Friends

	friend ostream& operator<< (ostream& s, const ClimateDate& date)
		{ date.print(s); return s; }

protected:

// -- Members
	// None

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// -- Members
	
	Date date_;

// -- Methods

	void print(ostream&) const;

// -- Class methods

};

//-----------------------------------------------------------------------------

} // namespace eclib


#endif
