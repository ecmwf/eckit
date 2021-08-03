/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <iomanip>

#include "eckit/io/StdioBuf.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


StdioBuf::StdioBuf(FILE* file) :
    file_(file) {
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

StdioBuf::~StdioBuf() {
    sync();
}

int StdioBuf::sync() {
    size_t s = pptr() - pbase();
    if (s) {
        if (::fwrite(pbase(), 1, s, file_) != s)
            return EOF;
    }

    setp(pbase(), epptr());

    return 0;
}

int StdioBuf::overflow(int c) {
    if (sync())
        return EOF;

    if (c == EOF)
        return 0;

    sputc(c);
    return 0;
}

int StdioBuf::underflow() {
    if (gptr() < egptr())
        return *(unsigned char*)gptr();

#ifndef OLD_STREAMBUF
    int n = ::fread(in_, 1, sizeof(in_), file_);
#else
    int n = ::fread(base(), 1, sizeof(in_), file_);
#endif

    if (n == EOF || n == 0)
        return EOF;

#ifndef OLD_STREAMBUF
    setg(in_, in_, in_ + n);
#else
    setg(eback(), base(), base() + n);
#endif
    return *(unsigned char*)gptr();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
