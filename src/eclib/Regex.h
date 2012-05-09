/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Regex.h
// Baudouin Raoult - ECMWF Jan 98

#ifndef Regex_H
#define Regex_H

#include <regex.h>

#include "eclib/machine.h"

class Regex {
public:

// -- Exceptions
	// None

// -- Contructors

	Regex(const string& = ".*",bool shell = false);
	Regex(const Regex&);

	~Regex();

// -- Methods

	Regex& operator=(const Regex&);

	bool match(const string& s) const;

	operator const string&() const
		{ return str_; }

	bool operator==(const Regex& other) const
		{ return str_ == other.str_; }

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None


protected:

// -- Members
	// None

// -- Methods

	void print(ostream&) const; // Change to virtual if base class

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:


// -- Members

	string str_;
	regex_t re_;

// -- Methods

	void compile(const char*);

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	friend ostream& operator<<(ostream& s,const Regex& p)
		{ p.print(s); return s; }

};

#endif
