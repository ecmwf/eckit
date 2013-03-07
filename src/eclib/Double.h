/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Double.h
// Baudouin Raoult - ECMWF Dec 97

#ifndef eckit_Double_h
#define eckit_Double_h

#include "eclib/machine.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class DumpLoad;

class Double {
public:

// -- Exceptions
	// None

// -- Contructors

	Double(double = 0);
	Double(const string&);

#include "eclib/Double.b"

// -- Destructor

	~Double() {}

// -- Convertors
	// None

// -- Operators

	operator string() const;

	bool operator==(const Double& other) const
		{ return round() == other.round(); }

	bool operator!=(const Double& other) const
		{ return round() != other.round(); }

	bool operator<(const Double& other) const
		{ return round() < other.round(); }

	bool operator>(const Double& other) const
		{ return round() > other.round(); }

// -- Methods

	void dump(DumpLoad&) const;
	void load(DumpLoad&);

	double round() const;

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods


protected:

// -- Members
	// None

// -- Methods

	void print(ostream& s) const;

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

// -- Members

	double value_;

// -- Friends

	friend ostream& operator<<(ostream& s,const Double& p)
		{ p.print(s); return s; }

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
