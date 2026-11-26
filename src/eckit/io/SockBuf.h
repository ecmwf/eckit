// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File SockBuf.h
// Baudouin Raoult - ECMWF Mar 97

#ifndef eckit_SockBuf_h
#define eckit_SockBuf_h

#include "eckit/net/TCPSocket.h"


namespace eckit {


class SockBuf : public std::streambuf {
public:

    // -- Contructors

    SockBuf(net::TCPSocket& proto);

    // -- Destructor

    ~SockBuf();

private:

    // No copy allowed

    SockBuf(const SockBuf&);
    SockBuf& operator=(const SockBuf&);

    // -- Members

    char in_[1];
    char out_[80];
    net::TCPSocket& protocol_;

    // -- Overridden methods

    // From streambuf

    virtual int overflow(int c);
    virtual int underflow();
    virtual int sync();
    //	virtual int uflow();
};


}  // namespace eckit

#endif
