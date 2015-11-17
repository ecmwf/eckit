/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_FileName_h
#define eckit_FileName_h

#include "eckit/eckit.h"
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

	FileName(const std::string&);
//	FileName(const std::string&,const std::string&);
	

// -- Destructor

	~FileName(); 

// -- Methods
    
    bool exists() const;

	DataHandle*  newWriteHandle() const;
	DataHandle*  newReadHandle(const OffsetList&, const LengthList&) const;
	DataHandle*  newReadHandle() const;
	
	const std::string& name() const { return name_; }
	const std::string& scheme() const { return scheme_; }

protected: // methods

	void print(std::ostream&) const;

private: // members

	std::string name_;
	std::string scheme_;

// -- Friends

	friend std::ostream& operator<<(std::ostream& s,const FileName& p)
		{ p.print(s); return s; }

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
