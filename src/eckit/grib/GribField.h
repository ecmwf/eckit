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
class DataHandle;
class GribHandle;
class GribFieldMemoryStrategy;

class GribField : public Counted {
public:

// -- Exceptions
	// None

// -- Contructors

    GribField(GribFile *, const Offset&, const Length&, GribHandle *handle = 0);
    GribField(GribField*, double* values,  size_t count);

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

    static void setStrategy(GribFieldMemoryStrategy&);

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

    GribFile* file_;
    Offset    offset_;
    Length    length_;

    GribHandle* handle_;

    double*    values_;
    size_t     count_;
    GribField* headers_;

    time_t last_;
    size_t accesses_;
    size_t purges_;

    GribFieldMemoryStrategy* strategy_;


    void pack();

// -- Methods

    void purge(bool temp = false) ;

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

    friend class GribFieldMemoryStrategy;

    friend ostream& operator<<(ostream& s,const GribField& p)
        { p.print(s); return s; }

};
}

#endif
