// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File CommandStream.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_CommandStream_h
#define eckit_CommandStream_h


#include "eckit/serialisation/Stream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class CommandStream : public Stream {
public:

    // -- Contructors

    CommandStream(const std::string& name, const char* mode);

    // -- Destructor

    ~CommandStream();

    // -- Overridden methods

    // From Stream

    long read(void*, long) override;
    long write(const void*, long) override;

private:

    // -- Members

    FILE* file_;

    // -- Overridden methods

    // From Stream
    std::string name() const override;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
