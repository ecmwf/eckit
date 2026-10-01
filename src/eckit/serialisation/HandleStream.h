// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File HandleStream.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef eckit_HandleStream_h
#define eckit_HandleStream_h

#include "eckit/io/DataHandle.h"
#include "eckit/serialisation/Stream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


// Adaptor: allow Stream operations on a DataHandle;

class HandleStream : public Stream {
public:

    // -- Contructors

    HandleStream(DataHandle& h) : handle_(h) {}

    // -- Destructor

    ~HandleStream() {}

private:

    // -- Members

    DataHandle& handle_;

    // -- Overridden methods

    // From Stream

    virtual long write(const void* buf, long len) { return handle_.write(buf, len); }

    virtual long read(void* buf, long len) { return handle_.read(buf, len); }

    virtual std::string name() const { return "HandleStream"; }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
