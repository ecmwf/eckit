// File GribFieldMemoryStrategy.h
// Baudouin Raoult - (c) ECMWF Nov 13

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

} // Namespace



#endif
