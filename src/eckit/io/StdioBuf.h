/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File StdioBuf.h
// Baudouin Raoult - ECMWF Mar 97

#ifndef eckit_StdioBuf_h
#define eckit_StdioBuf_h

#include <cstdio>

#include "eckit/eckit.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class StdioBuf : public std::streambuf {
public:

    // -- Contructors

    StdioBuf(FILE*);

    // -- Destructor

    ~StdioBuf();

private:

    // No copy allowed

    StdioBuf(const StdioBuf&);
    StdioBuf& operator=(const StdioBuf&);

    // -- Members

    char in_[1];
    char out_[80];
    FILE* file_;

    // -- Overridden methods

    // From streambuf

    virtual int overflow(int c);
    virtual int underflow();
    virtual int sync();
    //	virtual int uflow();
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
