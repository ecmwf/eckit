// File Matrix.h
// Baudouin Raoult - (c) ECMWF Nov 13

#ifndef Matrix_H
#define Matrix_H

// namespace mpi;

// Headers
// #ifndef   machine_H
// #include "machine.h"
// #endif

// Forward declarations

// class ostream;

//
#include <eckit/eckit.h>
namespace eckit {

class Matrix {
public:

// -- Exceptions
	// None

// -- Contructors

    Matrix(size_t rows, size_t cols);

	Matrix(const Matrix&);
	Matrix& operator=(const Matrix&);

// -- Destructor

	~Matrix(); // Change to virtual if base class

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

    size_t index(size_t r, size_t c) const 
        { return r*cols_ + c; }

    double operator()(size_t r, size_t c)  const
        { return values_[index(r,c)]; }

    double& operator()(size_t r, size_t c)
        { 
            if(shared_) copy();
            return values_[index(r,c)] ;
        }


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


// -- Members

    size_t*    shared_;
    double*    values_;
    size_t     rows_;
    size_t     cols_;

// -- Methods

    void copy();
    void cleanup();

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

    friend ostream& operator<<(ostream& s,const Matrix& p)
        { p.print(s); return s; }

};
}

#endif
