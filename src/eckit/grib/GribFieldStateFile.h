// File GribFieldStateFile.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribFieldStateFile_H
#define GribFieldStateFile_H

#include "GribFieldState.h"

#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

namespace eckit {

class GribFile;

class GribFieldStateFile : public GribFieldState {
public:

// -- Exceptions
	// None

// -- Contructors

    GribFieldStateFile(GribFile*, const Offset& offset, const Length& length, GribFieldState* next = 0);

// -- Destructor

	~GribFieldStateFile(); // Change to virtual if base class

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


// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	GribFieldStateFile(const GribFieldStateFile&);
	GribFieldStateFile& operator=(const GribFieldStateFile&);

// -- Members

    GribFile* file_;
    Offset    offset_;
    Length    length_;

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

	//friend ostream& operator<<(ostream& s,const GribFieldStateFile& p)
	//	{ p.print(s); return s; }

};

}

#endif
