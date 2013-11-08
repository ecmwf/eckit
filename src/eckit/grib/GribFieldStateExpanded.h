// File GribFieldStateExpanded.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribFieldStateExpanded_H
#define GribFieldStateExpanded_H

#include "GribFieldState.h"

#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

namespace eckit {

class GribFile;

class GribFieldStateExpanded : public GribFieldState {
public:

// -- Exceptions
	// None

// -- Contructors

    GribFieldStateExpanded(double*, size_t, GribFieldState* next);

// -- Destructor

	~GribFieldStateExpanded(); // Change to virtual if base class

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

	GribFieldStateExpanded(const GribFieldStateExpanded&);
	GribFieldStateExpanded& operator=(const GribFieldStateExpanded&);

// -- Members

    double* values_;
    size_t  count_;

// -- Methods
	// None

// -- Overridden methods
	
    virtual double* returnValues(size_t&) const;
    virtual GribHandle* returnHandle(bool copy) const;
    virtual void write(DataHandle&) const;

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const GribFieldStateExpanded& p)
	//	{ p.print(s); return s; }

};

}

#endif
