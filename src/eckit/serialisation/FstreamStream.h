// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File FstreamStream.h
// Baudouin Raoult - ECMWF Aug 18

#ifndef eckit_FstreamStream_h
#define eckit_FstreamStream_h

#include <fstream>
#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/serialisation/Stream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


// Adaptor: allow Stream operations on a DataHandle;

class FstreamStream : public Stream {
public:

    // -- Contructors

    FstreamStream(std::fstream& f) : f_(f) {}

    // -- Destructor

    ~FstreamStream() {}

private:

    // -- Members

    std::fstream& f_;

    // -- Overridden methods

    // From Stream

    virtual long write(const void* buf, long len) {
        std::streampos here = f_.tellp();
        f_.write(static_cast<const char*>(buf), len);
        ASSERT(!f_.bad());
        return f_.tellp() - here;
    }

    virtual long read(void* buf, long len) {
        std::streampos here = f_.tellp();
        f_.read(static_cast<char*>(buf), len);
        ASSERT(!f_.eof() && !f_.bad());
        return f_.tellp() - here;
    }

    virtual std::string name() const { return "FstreamStream"; }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
