// File GribFieldStateResult.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribFieldStateResult_H
#define GribFieldStateResult_H

#include "GribFieldState.h"

#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

namespace eckit {

class GribField;

class GribFieldStateResult : public GribFieldState {
public:

// -- Exceptions
	// None

// -- Contructors

    GribFieldStateResult(GribField*, double*, size_t);

// -- Destructor

	~GribFieldStateResult(); // Change to virtual if base class

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

	GribFieldStateResult(const GribFieldStateResult&);
	GribFieldStateResult& operator=(const GribFieldStateResult&);

// -- Members

    double* values_;
    size_t  count_;
    GribField* headers_;

// -- Methods
	// None

// -- Overridden methods
	
    virtual GribFieldState* returnValues(double*&, size_t&) const;
    virtual GribFieldState* returnHandle(GribHandle*&) const;

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const GribFieldStateResult& p)
	//	{ p.print(s); return s; }

};

}

#endif
