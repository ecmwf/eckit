/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MoverTransfer.h
// Baudouin Raoult - (c) ECMWF Jun 11

#ifndef eckit_MoverTransfer_h
#define eckit_MoverTransfer_h

#include "eclib/DataHandle.h"
#include "eckit/io/Length.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TransferWatcher;

class MoverTransfer {
public:

// -- Exceptions
	// None

// -- Contructors

	MoverTransfer(TransferWatcher& = TransferWatcher::dummy());

// -- Destructor

	~MoverTransfer(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

    Length transfer(DataHandle&, DataHandle&);
    TransferWatcher& watcher() const { return watcher_; }

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

	MoverTransfer(const MoverTransfer&);
	MoverTransfer& operator=(const MoverTransfer&);

// -- Members
	// None
    TransferWatcher& watcher_;

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const MoverTransfer& p)
	//	{ p.print(s); return s; }

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
