// File GribFieldStateHandle.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribFieldStateHandle_H
#define GribFieldStateHandle_H

#include "GribFieldState.h"
#include "GribHandle.h"

#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

namespace eckit {

class GribHandle;

class GribFieldStateHandle : public GribFieldState {
public:

// -- Exceptions
	// None

// -- Contructors

    GribFieldStateHandle(GribHandle*, GribFieldState* next);

// -- Destructor

	~GribFieldStateHandle(); // Change to virtual if base class

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

	GribFieldStateHandle(const GribFieldStateHandle&);
	GribFieldStateHandle& operator=(const GribFieldStateHandle&);

// -- Members

    GribHandle* handle_;

// -- Methods
	// None

// -- Overridden methods
	
    virtual GribFieldState* returnValues(double*&, size_t&) const;
    virtual GribFieldState* returnHandle(GribHandle*&, bool copy) const;
    virtual void write(DataHandle&) const;
    virtual GribFieldState* release() const;

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const GribFieldStateHandle& p)
	//	{ p.print(s); return s; }

};

}

#endif
