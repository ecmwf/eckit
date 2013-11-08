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

namespace eckit {

class GribFile;
class GribFieldState;
class DataHandle;
class GribHandle;

class GribField : public Counted {
public:

// -- Exceptions
	// None

// -- Contructors

    GribField(GribFile *, const Offset&, const Length&);
    GribField(GribHandle*, GribFile *, const Offset&, const Length&);
    GribField(GribField*,double*,  size_t);

// -- Destructor

	~GribField(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

    const double *getValues(size_t&) const;
    GribHandle* getHandle(bool copy) const;

    void write(DataHandle&) const;

    void release() const;

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

    GribFieldState* state_;

// -- Methods

    void mutate(GribFieldState*) const;

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
