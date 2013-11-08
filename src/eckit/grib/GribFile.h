// File GribFile.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef GribFile_H
#define GribFile_H

#include "eckit/memory/Counted.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/Offset.h"
#include "eckit/io/Length.h"

namespace eckit {

class GribFieldSet;

class GribFile : public Counted {
public:

// -- Exceptions
	// None

// -- Contructors



// -- Destructor



// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

    GribFieldSet* getFieldSet(bool preload) const;
    void getFieldSet(GribFieldSet&, bool preload) const;
    void getBuffer(Buffer&, const Offset&, const Length&);

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods

    static GribFile* newGribFile(const PathName&);

protected:

// -- Members

    // Use "newGribFile" intead
    GribFile(const PathName&);
    ~GribFile(); // Change to virtual if base class

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
//
	PathName path_;

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
}

#endif
