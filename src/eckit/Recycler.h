/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Recycler.h
// Baudouin Raoult - ECMWF Apr 97

#ifndef eckit_Recycler_h
#define eckit_Recycler_h

#include "eckit/filesystem/PathName.h"
#include "eckit/types/Types.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template<class T>
class Recycler {
public:

// -- Exceptions
    // None

// -- Contructors

    Recycler(const PathName&);


// -- Destructor

    ~Recycler(); // Change to virtual if base class

// -- Convertors
    // None

// -- Operators
    // None

// -- Methods

    void lock();
    void unlock();


    void push(const T&);

    template<class Iter>
    void push(Iter begin, Iter end);

    bool pop(T& value);


    template<class Iter>
    Ordinal pop(Iter begin, Ordinal count);

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

    Recycler(const Recycler<T>&);
    Recycler<T>& operator=(const Recycler<T>&);

// -- Members
    // None


    PathName path_;
    int fd_;

// -- Methods


// -- Overridden methods
    // None

// -- Class members
    // None

// -- Class methods
    // None

// -- Friends

    friend ostream& operator<<(ostream& s,const Recycler<T>& p)
    {
        p.print(s);
        return s;
    }

};

//-----------------------------------------------------------------------------

} // namespace eckit

#include "Recycler.cc"

#endif
