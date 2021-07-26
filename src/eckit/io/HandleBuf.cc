/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/HandleBuf.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

HandleBuf::HandleBuf(DataHandle& handle, bool throwOnError) :
    handle_(handle), throwOnError_(throwOnError) {
    setg(in_, in_, in_);
    setp(out_, out_ + sizeof(out_));
}

HandleBuf::~HandleBuf() {
    sync();
}

int HandleBuf::sync() {
    int len = pptr() - pbase();
    if (len != 0) {
        int written = handle_.write(pbase(), len);

        if (len != written) {
            if (throwOnError_) {
                std::ostringstream oss;
                oss << "HandleBuf: failed to write to " << handle_;
                throw WriteError(oss.str());
            }

            return EOF;
        }
    }

    setp(pbase(), epptr());

    return 0;
}

int HandleBuf::overflow(int c) {
    if (sync())
        return EOF;

    if (c == EOF)
        return 0;

    sputc(c);
    return 0;
}

int HandleBuf::underflow() {
    if (gptr() < egptr())
        return *(unsigned char*)gptr();

    int n = handle_.read(in_, sizeof(in_));

    if (n == EOF || n == 0) {
        if (throwOnError_) {
            std::ostringstream oss;
            oss << "HandleBuf: failed to read from " << handle_;
            throw ReadError(oss.str());
        }

        return EOF;
    }

    setg(in_, in_, in_ + n);

    return *(unsigned char*)gptr();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
