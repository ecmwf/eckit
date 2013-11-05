// File GribFile.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribFile_H
#define GribFile_H

#include "eckit/memory/Counted.h"

// namespace mpi;

// Headers
// #ifndef   machine_H
// #include "machine.h"
// #endif

// Forward declarations

// class ostream;

// 

class GribFile : public eckit::Counted {
public:

// -- Exceptions
	// None

// -- Contructors

	GribFile();

// -- Destructor

	~GribFile(); // Change to virtual if base class

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

	GribFile(const GribFile&);
	GribFile& operator=(const GribFile&);

// -- Members
	// None

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

    friend ostream& operator<<(ostream& s,const GribFile& p)
        { p.print(s); return s; }

};

#endif
