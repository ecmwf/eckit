// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/HandleBuf.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

HandleBuf::HandleBuf(DataHandle& handle, bool throwOnError) : handle_(handle), throwOnError_(throwOnError) {
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
    if (sync()) {
        return EOF;
    }

    if (c == EOF) {
        return 0;
    }

    sputc(c);
    return 0;
}

int HandleBuf::underflow() {
    if (gptr() < egptr()) {
        return *(unsigned char*)gptr();
    }

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
