/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File HandleBuf.h
// Baudouin Raoult - ECMWF Mar 97

#ifndef eckit_HandleBuf_h
#define eckit_HandleBuf_h

#include "eckit/io/DataHandle.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class HandleBuf : public std::streambuf {
public:

    // -- Contructors

    HandleBuf(DataHandle& handle, bool throwOnError = false);

    // -- Destructor

    ~HandleBuf();

private:

    // No copy allowed

    HandleBuf(const HandleBuf&);
    HandleBuf& operator=(const HandleBuf&);

    // -- Members

    char in_[1];
    char out_[80];
    DataHandle& handle_;
    bool throwOnError_;

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
