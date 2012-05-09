/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef FileName_H
#define FileName_H

#include "eclib/machine.h"
#include "eclib/Offset.h"
#include "eclib/Length.h"

class Stream;
class DataHandle;

class FileName {
public:

// -- Exceptions
	// None

// -- Contructors

	FileName(const string&);
//	FileName(const string&,const string&);
	

// -- Destructor

	~FileName(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	DataHandle*  newWriteHandle() const;
	DataHandle*  newReadHandle(const OffsetList&, const LengthList&) const;
	DataHandle*  newReadHandle() const;
	
	const string& name() const { return name_; }
	const string& scheme() const { return scheme_; }

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
	
	void print(ostream&) const;

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	//FileName(const FileName&);
	//FileName& operator=(const FileName&);

// -- Members

	string name_;
	string scheme_;

// -- Methods
	// None


// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	friend ostream& operator<<(ostream& s,const FileName& p)
		{ p.print(s); return s; }

};


#endif
