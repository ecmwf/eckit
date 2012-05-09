/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Progress.h
// Baudouin Raoult - ECMWF Nov 96

#ifndef Progress_H
#define Progress_H

#include <string>

class Progress {
public:

// -- Exceptions
	// None

// -- Contructors

	Progress(const std::string&,unsigned long long,unsigned long long);

// -- Destructor

	~Progress(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators

	void operator()(unsigned long long);

// -- Methods
	// None

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

	// void print(ostream&) const; // Change to virtual if base class

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	Progress(const Progress&);
	Progress& operator=(const Progress&);

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

// -- Friends

	//friend ostream& operator<<(ostream& s,const Progress& p)
	//	{ p.print(s); return s; }

};


#endif
