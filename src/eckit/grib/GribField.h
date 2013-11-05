// File GribField.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribField_H
#define GribField_H

// namespace mpi;

// Headers
// #ifndef   machine_H
// #include "machine.h"
// #endif

// Forward declarations

// class ostream;

// 
#include "eckit/memory/Counted.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

namespace "eckit" {

class GribFile;

class GribField : public eckit::Counted {
public:

// -- Exceptions
	// None

// -- Contructors

    GribField(GribFile *, const eckit::Offset&, const eckit::Length&);

// -- Destructor

	~GribField(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

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
	
    void print(ostream&) const; // Change to virtual if base class

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	GribField(const GribField&);
	GribField& operator=(const GribField&);

// -- Members

    eckit::Offset offset_;
    eckit::Length length_;
    GribFile*     file_;

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

    friend ostream& operator<<(ostream& s,const GribField& p)
        { p.print(s); return s; }

};
}

#endif
