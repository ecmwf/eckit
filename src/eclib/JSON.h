/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File JSON.h
// Baudouin Raoult - (c) ECMWF Aug 11

#ifndef JSON_H
#define JSON_H

#include "eclib/machine.h"

class JSON {
public:

// -- Exceptions
	// None

// -- Contructors

	JSON(ostream&);

// -- Destructor

	~JSON(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

	JSON& operator<<(bool);
	JSON& operator<<(char);
	JSON& operator<<(unsigned char);
	JSON& operator<<(int);
	JSON& operator<<(unsigned int);
	JSON& operator<<(long);
	JSON& operator<<(unsigned long);
	JSON& operator<<(long long);
	JSON& operator<<(unsigned long long);
	JSON& operator<<(float);
	JSON& operator<<(double);
	
	JSON& operator<<(const string&);
	JSON& operator<<(const char*);

// -- Methods
	// None
    //

    JSON& null();

    JSON& startObject();
    JSON& endObject();

    JSON& startList();
    JSON& endList();

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

	JSON(const JSON&);
	JSON& operator=(const JSON&);

// -- Members

    ostream& out_;
    vector<string> sep_;
    vector<bool> state_;
    bool null_;

// -- Methods
	// None

    void sep();
    bool inlist() { return !state_.back(); }
    bool indict() { return  state_.back(); }

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const JSON& p)
	//	{ p.print(s); return s; }

};

#endif
