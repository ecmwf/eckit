// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File StdioBuf.h
// Baudouin Raoult - ECMWF Mar 97

#ifndef eckit_StdioBuf_h
#define eckit_StdioBuf_h

#include <cstdio>
#include <streambuf>

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
