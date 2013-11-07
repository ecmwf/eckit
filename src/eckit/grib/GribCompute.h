// File GribCompute.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribCompute_H
#define GribCompute_H

// namespace mpi;

// Headers
// #ifndef   machine_H
// #include "machine.h"
// #endif

// Forward declarations

// class ostream;

// 
#include "eckit/memory/Counted.h"
#include "eckit/filesystem/PathName.h"

namespace eckit {

class GribField;

class GribCompute {
public:

// -- Exceptions
	// None

// -- Contructors

    GribCompute();
    GribCompute(const PathName& path);

// -- Destructor

	~GribCompute(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

    void add(GribField*);

    size_t count() const { return fields_.size(); }
    const GribField* get(size_t i) const { return fields_[i]; }

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

	GribCompute(const GribCompute&);
	GribCompute& operator=(const GribCompute&);

// -- Members
//
    std::vector<GribField*> fields_;

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

    friend ostream& operator<<(ostream& s,const GribCompute& p)
        { p.print(s); return s; }

};
}

#endif
