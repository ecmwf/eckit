// File SmallestSphere.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef SmallestSphere_H
#define SmallestSphere_H

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


class SmallestSphere {
public:

// -- Exceptions
	// None

// -- Contructors

    SmallestSphere();

// -- Destructor

	~SmallestSphere(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods


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

// No copy allowed

	SmallestSphere(const SmallestSphere&);
	SmallestSphere& operator=(const SmallestSphere&);

// -- Members




// -- Methods


// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

    friend ostream& operator<<(ostream& s,const SmallestSphere& p)
        { p.print(s); return s; }

};


void smallestSphere(const vector<vector<double> >& points);
}

#endif
