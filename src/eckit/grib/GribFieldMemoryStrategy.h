/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file GribFieldMemoryStrategy.h
/// @author Baudouin Raoult
/// @date November 2013

#ifndef GribFieldMemoryStrategy_H
#define GribFieldMemoryStrategy_H

// namespace mpi;

// Headers
// #ifndef   machine_H
// #include "machine.h"
// #endif

// Forward declarations

// class ostream;

// 
#include "eckit/eckit.h"

namespace eckit {

class GribField;

class GribFieldMemoryStrategy {
public:



// -- Exceptions
	// None

// -- Contructors

    GribFieldMemoryStrategy();

// -- Destructor

    virtual ~GribFieldMemoryStrategy(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators


    void touch(GribField&);
    void purgeable(GribField&);
    void remove(GribField&);

    void newValues(GribField&);
    void newHandle(GribField&);

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods



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

// No copy allowed

// -- Members
//

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

    set<GribField*> released_;
    size_t max_;
    size_t count_;

// -- Class methods
	// None

// -- Friends

    friend ostream& operator<<(ostream& s,const GribFieldMemoryStrategy& p)
        { p.print(s); return s; }

};

} // namespace eckit



#endif
