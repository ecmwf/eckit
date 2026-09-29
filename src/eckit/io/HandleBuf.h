// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
