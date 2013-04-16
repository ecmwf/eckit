/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_FileName_h
#define eckit_FileName_h

#include "eclib/machine.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Stream;
class DataHandle;

class FileName {
public:

// -- Contructors

	FileName(const string&);
//	FileName(const string&,const string&);
	

// -- Destructor

	~FileName(); // Change to virtual if base class

// -- Methods
    
    bool exists() const;

	DataHandle*  newWriteHandle() const;
	DataHandle*  newReadHandle(const OffsetList&, const LengthList&) const;
	DataHandle*  newReadHandle() const;
	
	const string& name() const { return name_; }
	const string& scheme() const { return scheme_; }

protected: // methods

	void print(ostream&) const;

private: // members

	string name_;
	string scheme_;

// -- Friends

	friend ostream& operator<<(ostream& s,const FileName& p)
		{ p.print(s); return s; }

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
