// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/SockBuf.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


SockBuf::SockBuf(net::TCPSocket& proto) : protocol_(proto) {
#ifndef OLD_STREAMBUF
    /* setg(in_,  in_,  in_  + sizeof(in_) );  */
    setg(in_, in_, in_);
    setp(out_, out_ + sizeof(out_));
#else
    setb(in_, in_ + sizeof(in_), 0);
    setg(in_, in_, in_);
    setp(out_, out_ + sizeof(out_));
#endif
}

SockBuf::~SockBuf() {
    sync();
}

int SockBuf::sync() {

    if (!protocol_.isConnected()) {
        setp(pbase(), epptr());
        return EOF;
    }

    if (protocol_.write(pbase(), pptr() - pbase()) < 0) {
        protocol_.close();
        return EOF;
    }

    setp(pbase(), epptr());

    return 0;
}

int SockBuf::overflow(int c) {
    if (sync()) {
        return EOF;
    }

    if (c == EOF) {
        return 0;
    }

    sputc(c);
    return 0;
}

int SockBuf::underflow() {
    if (gptr() < egptr()) {
        return *(unsigned char*)gptr();
    }

    if (!protocol_.isConnected()) {
        return EOF;
    }

#ifndef OLD_STREAMBUF
    int n = protocol_.read(in_, sizeof(in_));
#else
    int n = protocol_.read(base(), sizeof(in_));
#endif

    if (n == EOF || n == 0) {
        protocol_.close();
        return EOF;
    }

#ifndef OLD_STREAMBUF
    setg(in_, in_, in_ + n);
#else
    setg(eback(), base(), base() + n);
#endif
    return *(unsigned char*)gptr();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
