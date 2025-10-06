/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
