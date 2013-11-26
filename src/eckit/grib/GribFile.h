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
class EmosFile;

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

    GribFieldSet* getFieldSet() const;
    void getFieldSet(GribFieldSet&) const;
    void getBuffer(Buffer&, const Offset&, const Length&);

    const PathName& path() const { return path_; }

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
	
    void print(std::ostream&) const; // Change to virtual if base class

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
    EmosFile* file_;

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

    friend std::ostream& operator<<(std::ostream& s,const GribFile& p)
        { p.print(s); return s; }

};
}

#endif
