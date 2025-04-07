/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File IfstreamStream.h
// Baudouin Raoult - ECMWF Aug 18

#ifndef eckit_IfstreamStream_h
#define eckit_IfstreamStream_h

#include <fstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/serialisation/Stream.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


// Adaptor: allow Stream operations on a DataHandle;

class IfstreamStream : public Stream {
public:

    // -- Contructors

    IfstreamStream(std::ifstream& f) : f_(f) {}

    // -- Destructor

    ~IfstreamStream() {}

private:

    // -- Members

    std::ifstream& f_;

    // -- Overridden methods

    // From Stream

    virtual long write(const void* buf, long len) { NOTIMP; }

    virtual long read(void* buf, long len) {
        f_.read(static_cast<char*>(buf), len);
        ASSERT(!f_.eof() && !f_.bad());
        return len;
    }

    virtual std::string name() const { return "IfstreamStream"; }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
